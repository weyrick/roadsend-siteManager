#!/usr/local/bin/php
<?php

array_shift($argv);

foreach ($argv as $name) {
    print "conforming $name to php5 calls...\n";

    $fp = fopen($name,'r');
    $newFile = $name.'-new';
    $op = fopen($newFile,'w');
    $madeChange = false;
    while ($str = fgets($fp)) {

        $line = $str;        
        $line = str_replace('=&','=',$line);
        $line = str_replace('fetchRow','fetch',$line);
        $line = str_replace('->free()',' = null',$line);
        $line = str_replace('MDB2::isError','empty',$line);
        $line = str_replace('DB::isError','empty',$line);
        $line = str_replace('function &','function ',$line);
        $line = str_replace('numRows','rowCount',$line);
        $line = str_replace('rollback','rollBack',$line);
        
        fwrite($op, $line);
        
    }
    fclose($op);
    fclose($fp);

    rename($newFile, $name);

}


?>