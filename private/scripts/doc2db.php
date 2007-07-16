#!/usr/local/bin/php -q
<?php

require("DB.php");
require("File/Find.php");

$DSN = "mysql://develUser:d3v3lpa55@localhost/roadsend";
$docDir = '/home/weyrick/workspace/sitemanager/private/docs/siteManager_HTML';


// parse index.html to get titles
$fp = fopen($docDir.'/index.html','r');

while ($line = fgets($fp,4096)) {


    if (preg_match("/>(.+)<\/A/",$line,$matches)) {
        if ($fn != '') {
            if (!isset($titleList[$fn])) {            
                $titleList[$fn] = $matches[1];
            }
        }
    }

    if (preg_match("/HREF=\"(.+)\"/",$line,$matches)) {
        if (!strstr($matches[1],"#")) {
            $fn = $matches[1];            
        }
        else {
            $fn = '';
        }
    }
    else {
        $fn = '';
    }

}

fclose($fp);


// create a PEAR database object
$baseDB = new DB;
$dbH = $baseDB->connect($DSN, false);


if (MDB2::isError($dbH)) {
	echo "***** can't connect to database through /tmp/mysql.sock *****\n";
	exit;
}


$dbH->simpleQuery("DELETE FROM ma_manual where ma_project_idxNum =1");

// if it was a PEAR database error, puke
if (MDB2::isError($dbH)) {       
  echo("$dbH->toString()\n");
  exit;
}

// set to associative fetch by default
$dbH->setFetchMode(DB_FETCHMODE_ASSOC);

$fFind = new File_Find();

$docList = $fFind->search(".",$docDir);

foreach ($docList as $doc) {

    $dfName = basename($doc);
    echo "cataloging $doc, ".basename($dfName)."\n";

    $df = file($doc);
    $dfText = addslashes(join('',$df));
    $dfStripText = addslashes(strip_tags(join('',$df)));

    $SQL = "INSERT INTO ma_manual SET ma_project_idxNum =1, docTitle='$titleList[$dfName]', docFileName='$dfName', docText=\"$dfText\", docTextStripped=\"$dfStripText\", dateCreated=NOW()";
    $rh = $dbH->simpleQuery($SQL);
    if (MDB2::isError($rh)) {
        echo "error with query:\n$SQL\n".mysql_error()."\n";
        exit;
    }

}

?>
doc2db complete.

