#!/usr/bin/perl
#######################
#
# This is a quick perl script to convert 2.2.x SmartForm layout templates
# to the new 2.4.x compatible layout templates
#
#

open (IN,$ARGV[0]);

while (<IN>) {

   $_ =~ s/\{(\w+)\.(\w+)\}/<SM TYPE="SF" DATA="\2" VAR="\1">/g;
   $_ =~s/input/entity/g;
   
   print $_;
 
}


