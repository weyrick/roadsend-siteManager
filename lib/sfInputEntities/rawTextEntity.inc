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
 * Change Log
 * 
 *       11/29/2001 - Added entityConfig and initilized directives to 'Null'.
 *                     This was done so that it would be posible to get a 
 *                     list of directives on the fly. (gollum)
 *
 */

/**
 *  raw text, no input.
 * 
 *  directive: 
 *
 *     staticText - the text to display
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class rawTextEntity extends SM_inputTypeEntity {
    
    /**
    *Configure the entity
    */

    function entityConfig() {
        $this->directive['staticText'] = NULL;
        if ($this->value != NULL) {
            $this->directive['staticText'] = $this->value;
        }
    }

    /**
    * output:  return static text 
    */

    function entityThink() {
        if (isset($this->directive['staticText']))        
            $this->say($this->directive['staticText']);
    }

}

?>
