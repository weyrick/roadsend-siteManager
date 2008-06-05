<?php
/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001-2008 Roadsend, Inc.(http://www.roadsend.com)
 *******************************************************************
 *
 * This source file is subject to version 1.0 of the Roadsend Public
 * License, that is bundled with this package in the file 
 * LICENSE, and is available through the world wide web at 
 * http://www.roadsend.com/license/rpl1.txt
 *
 *******************************************************************
 * Author(s): Shannon Weyrick (weyrick@roadsend.com)
 *
 */
 
interface SM_inputFrobber {

    public function frobInput($data);

}

class SM_baseFrob extends SM_object {

    public function __construct() {
        $this->_smoConfigure();
    }
    
}

class SM_trimFrob extends SM_baseFrob implements SM_inputFrobber  {

    public function frobInput($data) {
        return trim($data);
    }

}

class SM_toLatin1Frob extends SM_baseFrob implements SM_inputFrobber  {

    public function frobInput($data) {
        // this is cp1252
        $d = strtr($data,
              array(
                    "\x80" => "e",  "\x81" => " ",    "\x82" => "'", "\x83" => 'f',
                    "\x84" => '"',  "\x85" => "...",  "\x86" => "+", "\x87" => "#",
                    "\x88" => "^",  "\x89" => "0/00", "\x8A" => "S", "\x8B" => "<",
                    "\x8C" => "OE", "\x8D" => " ",    "\x8E" => "Z", "\x8F" => " ",
                    "\x90" => " ",  "\x91" => "`",    "\x92" => "'", "\x93" => '"',
                    "\x94" => '"',  "\x95" => "*",    "\x96" => "-", "\x97" => "--",
                    "\x98" => "~",  "\x99" => "(TM)", "\x9A" => "s", "\x9B" => ">",
                    "\x9C" => "oe", "\x9D" => " ",    "\x9E" => "z", "\x9F" => "Y"));
        return $d;
    }

}


?>
