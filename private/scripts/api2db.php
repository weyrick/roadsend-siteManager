#!/usr/local/bin/php -q
<?php

require("DB.php");
require("File/Find.php");

$pw = `cat /home/weyrick/.repw`;
$DSN = "mysql://siteManager:".$pw."@localhost/siteManager";
$docDir = '/home/weyrick/workspace/siteManager/private/docs/siteManager_API/lib';


// get docTree
$fName = $docDir.'/docTree.bin';
$fp = fopen($fName,'r');
$contents = fread ($fp, filesize ($fName));
fclose($fp);

$docTree = unserialize($contents);

//var_dump($docTree);
//exit;

// create a PEAR database object
$baseDB = new DB;
$dbH = $baseDB->connect($DSN, false);

if (empty($dbH)) {
	echo "***** can't connect to database through /tmp/mysql.sock *****\n";
	exit;
}

$dbH->simpleQuery("DELETE FROM apiClass");

// if it was a PEAR database error, puke
if (empty($dbH)) {       
  echo("$dbH->toString()\n");
  exit;
}
$dbH->simpleQuery("DELETE FROM apiMethod");
// if it was a PEAR database error, puke
if (empty($dbH)) {       
  echo("$dbH->toString()\n");
  exit;
}
$dbH->simpleQuery("DELETE FROM apiClassVar");
// if it was a PEAR database error, puke
if (empty($dbH)) {       
  echo("$dbH->toString()\n");
  exit;
}
$dbH->simpleQuery("DELETE FROM apiMethodParam");
// if it was a PEAR database error, puke
if (empty($dbH)) {       
  echo("$dbH->toString()\n");
  exit;
}

// set to associative fetch by default
$dbH->setFetchMode(DB_FETCHMODE_ASSOC);

foreach ($docTree as $className=>$class) {


    $desc = addslashes($class['description']);

    // apiClass entry
    $SQL = "INSERT INTO apiClass SET name='$className', description='$desc', extends='{$class['extend']}'";    
    
    $rh = $dbH->simpleQuery($SQL);
    if (empty($rh)) {
        echo "error with query:\n$SQL\n".mysql_error()."\n";
        exit;
    }
    $dbH->freeResult($rh);

    // get class_idxNum
    $apiClass_idxNum = mysql_insert_id();

    // class variables
    if (is_array($class['vars'])) {
        foreach ($class['vars'] as $vName=>$vals) {

            $desc = addslashes($vals['description']);

            $SQL = "INSERT INTO apiClassVar SET name='$vName', description='$desc', apiClass_idxNum=$apiClass_idxNum";    

            $rh = $dbH->simpleQuery($SQL);
            if (empty($rh)) {
                echo "error with query:\n$SQL\n".mysql_error()."\n";
                exit;
            }
            $dbH->freeResult($rh);
        }
    }

    // methods and their params
    if (is_array($class['functions'])) {
        foreach ($class['functions'] as $fName=>$func) {

            // write function
            $desc = addslashes($func['description']);

            $SQL = "INSERT INTO apiMethod SET name='$fName', description='$desc', apiClass_idxNum=$apiClass_idxNum";    

            $rh = $dbH->simpleQuery($SQL);
            if (empty($rh)) {
                echo "error with query:\n$SQL\n".mysql_error()."\n";
                exit;
            }
            $dbH->freeResult($rh);

            $apiMethod_idxNum = mysql_insert_id();

            // get params
            if (is_array($func['paralist'])) {
                foreach ($func['paralist'] as $pNum=>$param) {
        

                    $param = addslashes($param);
                    $desc = addslashes($func['param'][$pNum]);
       
                    if ($param == '')
                        continue;
                        

                    $SQL = "INSERT INTO apiMethodParam SET name='$param', description='$desc', apiMethod_idxNum=$apiMethod_idxNum";    
        
                    $rh = $dbH->simpleQuery($SQL);
                    if (empty($rh)) {
                        echo "error with query:\n$SQL\n".mysql_error()."\n";
                        exit;
                    }
                    $dbH->freeResult($rh);
                }
            }


        }
    }



}

?>
api2db complete.

