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
        
        fwrite($op, $line);
        
    }
    fclose($op);
    fclose($fp);

    rename($newFile, $name);

}


?>