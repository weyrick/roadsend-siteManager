#!/usr/bin/perl -I/home/weyrick/src/dbScripts/
#
#  remember to put in the correct path to perl & make this file u+x
#
# JIRC  Jerry's IRC Bot  1.02
#  2000 July 6
#   Scott "Jerry" Lawrence   jirc@absynth.com
#

#  This is meant to be a simple bot structure onto which you can put 
#  whatever logic you want.  

#  I have some simple code in here now, such that if you /msg it, it
#  will spit out the text msg'd to it out to the channel it is in.  
#  If you /describe to it, it will emote what you ask.
#
#  This is probably enough to get a simple bot off the ground.  Just add code
#  into the *_handler functions at the bottom...
#
#  LIMITATIONS:
#   - error codes from the server are NOT CHECKED yet
#   - only tested on slashnet and undernet.
#
#  FEATURES:
#   - reads in configuration from an external file.
#   - connects to multiple channels on the server.
#   - will cycle through multiple servers 3 times, until it gives up.
#   - decodes CTCP messages.
#   - automatically PONG's the server.
#
################################################################################

################################################################################
# version info
#
# 1.02 2000 July 07
#	added irc_server_notice sub
#	changed the "welcome.." text to be a NOTICE
# 	added channel rejoin if kicked
# 	added cycling through servers if a connection is lost.
#	moved the configuration into jirc.conf
#	added config_... subs.
#
# 1.01 2000 July 06
# 	re-organzied the irc response handlers
#	it now correctly logs into warped.net.  yay!
#
# 1.00 2000 July 06
#	basic functionality implemented
#
################################################################################

################################################################################
# perl configuration stuff

use DBD::mysql;
require db_helper;

use IO::Socket;    # for sockets
#use Net::hostent;  # for OO version of gethostbyaddr

$|=1;   # turn off output buffering


################################################################################
#  client settings & behavior

$servername       = "JIRC/1.02 (Perl/Unix)";
$copyright_notice = "(c)2000 Scott \"Jerry\" Lawrence";
$credits          = "jirc\@absynth.com";

$suppress_server = 1;		# display messages from the server?

################################################################################
# configuration file handlers...

sub config_fileread
{
    local $filename = shift;
    local $line, $last;
    local $x = 0;
    local @keys = ();

    if (!-e $filename  ||  !-r $filename)
    {
	printf "$filename: not readable!\nExiting.";
	exit(1);
    }

    # read in the file...
    open IF, "$filename";
    foreach $line (<IF>)
    {
	chomp $line;
	$line =~ s/^\s+//g;
	$line =~ s/\s+$//g;
	$line =~ s/\n\r//g;
	next if ("#" eq substr $line, 0, 1);

	# store data aside...
	push @config_file, $line;
	if ($last eq "" && $line ne "")
	{
	    # make a hash of keys...
	    $keys{lc $line} = $x;
	}
	$last = $line;
	$x++;
    }
    close IF;

    # now pull out the info from the file that we need...
    $nick = config_readsingle("nick");
    $name = config_readsingle("name");
    @channels = config_readlist("channels");
    @servers  = config_readlist("servers");

    # dump out what we read in.
    printf <<EOB;
Configuration:
    nickname	$nick
    name	$name
    channels	@channels
    servers	@servers

EOB
}

sub config_readsingle
{
    # this assumes that the config file is in @config_file
    # it also assumes that the keyhash %keys is initialized
    local $key = shift;

    return $config_file[(int $keys{$key}) +1];
}

sub config_readlist
{
    # this assumes that the config file is in @config_file
    # it also assumes that the keyhash %keys is initialized
    local $key = shift;
    local $pos = (int $keys{$key}) +1;
    local @values = ();

    while ($config_file[$pos] ne "")
    {
	push @values, $config_file[$pos];
	$pos++;
    }
    return @values;
}


################################################################################
# connection routines

sub connect_to_server
{
    if (0 == scalar @servers)
    {
	die "ERROR: No servers selected!";
    }

    if (0 == scalar @channels)
    {
	die "ERROR: No channels selected!";
    }

    if ("" eq $nick || 4 != scalar (split " ",$name) )
    {
	die "ERROR: 'nick' or 'name' are incomplete!\n";
    }

    ($server, $port) = split ":", $servers[$serverNo];

    $remote = IO::Socket::INET->new( Proto     => "tcp",
				PeerAddr  => $server,
				PeerPort  => $port,
			       );
    unless ($remote) { die "ERROR: Cannot connect to $server" }
    $remote->autoflush(1);

    irc_server_raw("NICK", "$nick");

    irc_server_raw("USER", "$name");

    foreach $channel (@channels)
    {
	next if (length $channel < 1);

	irc_server_raw("JOIN", sprintf "#%s", $channel);
	$default_channel = "#" . $channel;
    }
    printf "Default channel is %s\n\n", $default_channel;

    # eventually, we'll check the responses from the server, but for now,
    # assume that it connected.

    $serverNo++;
    if ($serverNo >= scalar @servers) { $serverNo = 0; }
}

################################################################################
# local display info.

sub server_print
{
    if ($suppress_server == 0)
    {
	print shift;
    }
}


################################################################################
# server response parsers

sub irc_server_poll
{
    local $resp;
    if ( ($resp = <$remote>) )
    { 
	irc_check_command($resp); 
    }
}


sub irc_check_command
{
    local $response = shift;

    $ctcp_command = $ctcp_params = "";

    chomp $response;

    ($immediate, $svrmsg, @tl) = split ":", $response;

    $text = join ":", @tl;
    $text =~ s/[\n\r]//g;

    if ("PING" eq substr $immediate, 0, 4)
    {
	print $remote "PONG $svrmsg";
	
	server_print("SERVER PING");
    } else {
	@bits = split " ", $svrmsg;

	$remote_nick = (split "!", $svrmsg)[0];

	if ($bits[1] eq "QUIT")
	{
	    # someone quit
	    printf "*** %s QUIT: %s\n", 
			$remote_nick, $text;

	} elsif ($bits[1] eq "PART") 
	{
	    # someone left the channel
	    printf "*** %s left channel %s (%s)\n", 
			$remote_nick, $bits[2], $text;

	} elsif ($bits[1] eq "JOIN") 
	{
	    # someone joined the channel
	    printf "*** %s joined channel %s\n", $remote_nick, $text;

	    # be welcoming...
	    if (lc $remote_nick ne lc $nick)  # ...but not to ourselves. ;)
	    {
		irc_server_notice ($remote_nick, 
			sprintf "Welcome to %s, %s!", 
			$text, $remote_nick);
	    }

	} elsif ($bits[1] eq "NICK") 
	{
	    # someone changed their nick
	    printf "*** %s is now known as %s\n", $remote_nick, $text;

	} elsif ($bits[1] eq "KICK") 
	{
	    # someone was kicked.
	    printf "*** %s kicked %s on channel %s: %s\n", 
			$remote_nick, $bits[3], $bits[2], $text;

	    # auto rejoin...
	    if ( (lc $bits[3]) eq (lc $nick) )
	    {
		# yipe!  it was me that was kicked!
		irc_server_raw("JOIN", "$bits[2]");
		irc_server_speech($text, ":(");
	    }

	} elsif ($bits[1] eq "NOTICE") 
	{
	    # simple notices.
	    printf "*N* %s: -%s- %s\n", 
			$bits[2], $remote_nick, $text;

	} elsif ($bits[1] eq "TOPIC") 
	{
	    # topic change.
	    printf "*** %s changed the topic on %s to %s\n", 
		    $remote_nick, $bits[2], $text;

	} elsif ($bits[1] eq "MODE") 
	{
	    # someone changed mode.
	    printf "*** %s on %s: %s\n\n", 
		    $remote_nick, $bits[2], $response;

	} elsif ($bits[1] eq "PRIVMSG") 
	{
	    # someone said something
	    $msg_channel = lc $bits[2];

	    if ("" eq substr $text, 0, 1)
	    {
		# we should really check to see if the string starts and ends
		# with the ctrl-a's, but this works decent enough for now. :)

		$text =~ s/[]//g;
		$text =~ m/^([\w]+)[ ]/;
		$ctcp_command = $1;
		$ctcp_params  = substr $text, 
					(length $ctcp_command)+1, 
					length $text;
	    }

	    if ($ctcp_command eq "")
	    {
		# speak on channel
		if ($msg_channel eq lc $nick)
		{
		    private_speech_handler($remote_nick, $text);
		} else {
		    channel_speech_handler($msg_channel, 
					    $remote_nick, $text);
		}

	    } else {
		# ctcp command

		#printf "CTCP COMMAND [$ctcp_command] [$ctcp_params]\n";
		if ($ctcp_command eq "ACTION")
		{
		    # private message
		    if ($msg_channel eq lc $nick)
		    {                        
			private_emote_handler($remote_nick, $ctcp_params);
		    } else {                            
			channel_emote_handler($msg_channel,
				    $remote_nick, $ctcp_params);
		    }


		} elsif ($ctcp_command eq "PING") {

		    # ping
		    printf "CTCP PING from %s\n", $remote_nick;
		    printf $remote "NOTICE %s :PING %s\n",
			    $remote_nick, $ctcp_params;
		}
	    } 
	} elsif ($bits[2] eq "$nick") 
	{
	    server_print(sprintf "SERVER> %s\n", $text);
	} else {
	    printf "??? %s\n", $response;
	}
    }
}


################################################################################
# these are some simple subs to communicate back to the server

sub irc_server_raw
{
    local $command = shift;
    local $params  = shift;

    print $remote "$command $params\n"; 

    irc_server_poll();
}

sub irc_server_speech
{
    local $channel = shift; 
    local $text    = shift;

    printf "PRIVMSG %s :%s\n", 
		    $channel, 
		    $text;

    printf $remote "PRIVMSG %s :%s\n", 
		    $channel, 
		    $text;
}

sub irc_server_emote
{
    local $channel = shift; 
    local $text    = shift;

    irc_server_ctcp($channel, "ACTION", $text);
}


sub irc_server_ctcp
{
    local $channel = shift; 
    local $ctcpcmd = shift; 
    local $text    = shift;

    irc_server_speech($channel, "" . $ctcpcmd . " " . $text . "");
}


sub irc_server_notice
{
    local $nick = shift; 
    local $text = shift;

    printf $remote "NOTICE %s :%s\n", $nick, $text;
}



################################################################################

sub main
{
    $iteration = 0;

    printf "%s\n\t%s\n\t%s\n\n",
		$servername,
		$copyright_notice,
		$credits ;

    config_fileread("jirc.conf");


   # setup variables
   &setupVars("siteManager", "siteManager", "gFi24zz7");
   
   # connect to the database
   &db_connect;

    while ($iteration < (3 * scalar @servers))
    {
	connect_to_server;

	while ( $resp = <$remote> )
	{
	    irc_check_command($resp);
	}
	close $remote;
	$iteration++;
    }

    printf "Couldn't properly reconnect to the server.\n";
    printf "Sorry it didn't work out for you...\n";

   &db_close;

}

&main;


################################################################################
# these get called by the above processing loop.

# put your code into these handlers!

sub channel_speech_handler
{
    local $channel    = shift;
    local $remotenick = shift;
    local $text       = shift;

    printf "%s: <%s> %s\n", $channel, $remotenick, $text;

    if ("yow" eq lc substr $text, 0, 3)
    {
	# someone said "Yow"
	irc_server_speech($channel, "Yowza!");
    }

    if (-1 != index ":P", $text)
    {
	# someone said ":P"
	irc_server_speech($channel, sprintf "ungh, %s...", $remotenick);
    }


    if ($text =~ /^\!(.+)$/) {

      if ($1 =~ /^(.+)::(.+)$/) {
         $class = $1;
         $method = $2;
      }
      else {
         $class = '';
         $method = $1;
      }

      &sm_ref($class, $method);

    }

}

sub sm_ref {

   local $class = shift;
   local $method = shift;

   my @pList, @dArray;

   $SQL = "SELECT 
                  apiClass.name, 
                  apiMethod.description,
                  apiMethod.idxNum,
                  apiMethod.name
                FROM 
                  apiMethod,
                  apiClass 
                WHERE 
                  (apiMethod.name='$method' OR apiMethod.name='&$method') AND 
                  apiClass.idxNum=apiClass_idxNum
                  ";

   if ($class ne '') {
      $SQL .= "AND apiClass.name='$class'";
   }

   &executeSQL($SQL);

   if ($sth->rows == 0) {
      irc_server_speech($channel, "no matches for $method");
   }
   elsif ($sth->rows == 1) {

      # one match 
      $rr = $sth->fetchrow_arrayref;

      $class = $rr->[0];
      $desc = $rr->[1]; 
      $apiMethod_idxNum = $rr->[2];
      $methodName = $rr->[3];


      # params
      $psList = '';
      &executeSQL2("SELECT name, description FROM apiMethodParam WHERE apiMethod_idxNum=$apiMethod_idxNum ORDER BY idxNum");
      if ($sth2->rows) {

         while ($rr2 = $sth2->fetchrow_arrayref) {
            $psList .= $rr2->[0].',';
            push (@pList, $rr2->[1]);
         }
         chop($psList);

      }
      $sth2->finish;

      if ($desc =~ /\n/) {
         @dArray = split(/\n/, $desc);
      }
      else {
         push (@dArray, $desc);
      }

      irc_server_speech($channel, $class.'::'.$methodName.'('.$psList.')');
      foreach $p (@pList) {
         sleep 1;
         irc_server_speech($channel, $p);
      }

      foreach $d (@dArray) {      
         sleep 1;
         irc_server_speech($channel, $d);
      }

   }
   else {

      # multiple methods
      while ($rr = $sth->fetchrow_arrayref) {
         
         $class = $rr->[0];
         $methodName = $rr->[3];
         irc_server_speech($channel,  $class.'::'.$methodName);

      }

   }

   $sth->finish;
   

}

sub channel_emote_handler
{
    local $channel    = shift;
    local $remotenick = shift;
    local $text       = shift;

    printf "%s: * %s %s\n", $channel, $remotenick, $text;
}


sub private_speech_handler
{
    local $remotenick = shift;
    local $text       = shift;

    printf "-%s- %s\n", $remotenick, $text;

    if (($text eq 'dienow')&&($remotenick eq 'GRiD')) {
       die;
    }

    irc_server_speech($default_channel, $text);
}


sub private_emote_handler
{
    local $remotenick = shift;
    local $text       = shift;

    printf "-%s %s\n", $remote_nick, $text;

    irc_server_emote($default_channel, $text);
}

