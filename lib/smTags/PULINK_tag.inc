<?php

/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
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

/**
 * class representation of a PULINK tag 
 */
class SM_smTag_PULINK extends SM_smTag {


    /**
     * main think method. 
     * this will run the module
     */
    function tagThink() {

        $url = $this->getAttr('HREF');
        $title = $this->getAttr('TITLE');
        if (empty($title)) {
            $title = $url;
        }

        $h = $this->getAttr('HEIGHT');
        $w = $this->getAttr('WIDTH');
        $class = $this->getAttr('CLASS');

        if (empty($h)) {
            $h = 480;
        }

        if (empty($w)) {
            $w = 640;
        }

        if (empty($class)) {
            $class = '';
        }

        $this->say($this->sessionH->puLink($url, $title, $w, $h, $class));

    }

}
  
?>
