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
 *
 *  a simple entity whose purpose is to output a static
 *  text field as the value and not let the person change it.
 *  useful when you want to have a entity that may not be
 *  changed, but still shown in the same format as the rest
 *  of the form
 *  
 *  directives:
 *  
 *     style - HTML text that gets prepended to the value before output.
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class presetTextEntity extends SM_inputTypeEntity {

    /**
    * configure the entity
    */
     
    function entityConfig() {

        // Some defaults
        $this->directive['style']       = "<B>";

        // if makeHidden is true, this entity will output a hidden based on the varName/value
        $this->directive['makeHidden']  = false;

    }

    /**
    * output 
    */

    function entityThink() {

        // output text
        $this->say($this->directive['style'] . $this->value);

        // output hidden, if turned on
        if ($this->directive['makeHidden']) {
            $this->say("<input type=\"hidden\" name=\"$this->varName\" value=\"$this->value\" />\n");
        }

    }

}

?>
