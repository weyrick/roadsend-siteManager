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
* class representation of a HTMLBODY tag 
*/
 
class SM_smTag_HTMLBODY extends SM_smTag {

    /**
     * tag config. this tag needs to be delayed,
     * because other code might change its directives
     * before it should output
     */
    function tagConfig() {
        $this->delayThink = true;
    }


    /**
    * main think method. 
    */
     
    function tagThink() {
        
        $hHead = "<body";
        if (!empty($this->directive['bodyBGCOLOR']))
            $hHead .= " bgcolor=\"{$this->directive['bodyBGCOLOR']}\" ";
        if (!empty($this->directive['bodyLINK']))
            $hHead .= " link=\"{$this->directive['bodyLINK']}\" ";
        if (!empty($this->directive['bodyALINK']))
            $hHead .= " alink=\"{$this->directive['bodyALINK']}\" ";
        if (!empty($this->directive['bodyVLINK']))
            $hHead .= " vlink=\"{$this->directive['bodyVLINK']}\" ";
        if (!empty($this->directive['bodyBACKGROUND']))
            $hHead .= " background=\"{$this->directive['bodyBACKGROUND']}\" ";
        if (!empty($this->directive['bodyONLOAD']))
            $hHead .= " onload=\"{$this->directive['bodyONLOAD']}\" ";
        if (!empty($this->directive['bodyOther']))
            $hHead .= " {$this->directive['bodyOther']} ";
        $hHead .= ">\n\n";

        $this->say($hHead);

    }


    /**
    * called when template is reset
    */
     
    function tagReset() {
        $this->directive = array();
    }

}

?>
