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
 * class representation of a VAR tag
 */
class SM_smTag_DATE extends SM_smTag {

    /**
     * main think method.
     */
    function tagThink() {



        $format = $this->getAttr('FORMAT','M. d, Y');
        $class= $this->getAttr('CLASS','date');
        $op = date($format);
        if ($class == "nospan") {
            $this->say($op);
        } else {
            $this->say("<span class='$class'>".$op."</span>");    
        }       
    }

}

?>
