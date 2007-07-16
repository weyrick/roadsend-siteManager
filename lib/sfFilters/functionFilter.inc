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
 *       11/29/2001 - filterConfig added and directives initilized to 'Null'
 *                     This was done so that it would be posible to get a 
 *                     list of directives on the fly. (gollum)
 *
 */

/**
 *   take a custom function and run it on the data.
 *   the data should return either true or false
 *
 *   directives:
 *   
 *      function    - required paramter. the function to run. it takes
 *                  
 *   
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class functionFilter extends SM_entityFilter {

    //configure this filter
    function filterConfig() {
        $this->directive['function'] = NULL;
        $this->directive['formData'] = NULL;
    }

    // apply this filter
    function filterThink() {
    
        if (empty($this->directive['function']))
            return false;
            
        return call_user_func($this->directive['function'],$this->data, $this->dbH,$this->directive['formData']);

    }

}

?>