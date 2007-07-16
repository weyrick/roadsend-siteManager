#!/usr/local/bin/php
#
# ../siteManager/bin/conformPEAR.php `find ./ | grep '.[impc][nohp][cdpt]$'`
#
#
# this script will search & replace through all files on the command line, 
# changing improprer PEAR DB API calls (the ones pervasive throughout
# SiteManager 2.2.x / 2.4.x code) into the documented, proper versions.
#
# this means fetchRow(), numRows() and freeResult()
#
#
<?php


foreach ($argv as $name) {
    print "conforming $name to proper PEAR DB calls...\n";

    $fp = fopen($name,'r');
    $newFile = $name.'-new';
    $op = fopen($newFile,'w');
    $madeChange = false;
    while ($str = fgets($fp)) {

        $line = $str;
        
        if (preg_match('/(^.+(\$[a-zA-Z0-9\-_]+)\s*=\s*)(\$this->dbH->simpleQuery.+$)/',$str,$m)) {

            $madeChange = true;
            $line = $m[1].str_replace('simpleQuery','query',$m[3])."\n";
            $rhVar = $m[2];
            
        }

        if (($madeChange) && (strpos($str,'$this->dbH->numRows('.$rhVar.')') !== false)) {

            $line = str_replace('$this->dbH->numRows('.$rhVar.')', $rhVar.'->numRows()', $str);
            
        }

        if (($madeChange) && (strpos($str,'$this->dbH->fetchRow('.$rhVar.')') !== false)) {

            $line = str_replace('$this->dbH->fetchRow('.$rhVar.')', $rhVar.'->fetchRow()', $str);
            
        }

        if (($madeChange) && (strpos($str,'$this->dbH->freeResult('.$rhVar.')') !== false)) {

            $line = str_replace('$this->dbH->freeResult('.$rhVar.')', $rhVar.'->free()', $str);
            
        }
        
        fwrite($op, $line);
        
    }
    fclose($op);
    fclose($fp);

    if (!$madeChange) {
        unlink($newFile);
    }
    else {
        rename($newFile, $name);
    }
}


?>