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
 *  text area input box.
 *  
 *  directives:
 *  
 *      rows        - rows high
 *                  default (5)
 *
 *      cols        - columns wide
 *                  default (40)
 *            
 *       wrap       - can be sort of hard
 *                  default (soft)
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class textAreaEntity extends SM_inputTypeEntity {

    /**
    * constructor 
    */

    function entityConfig() {
            
        // setup some defaults
        $this->directive['rows']    = 5;
        $this->directive['cols']    = 40;
        $this->directive['wrap']    = 'soft';
        
    }

    /**
    * output 
    */

    function entityThink() {

        $attr = $this->getCommonAttr();
        $this->value = htmlspecialchars($this->value);
        $this->say("<textarea $attr name=\"$this->varName\" rows=\"{$this->directive['rows']}\" cols=\"{$this->directive['cols']}\" wrap=\"{$this->directive['wrap']}\">$this->value</textarea>");
    
    }

}

?>
