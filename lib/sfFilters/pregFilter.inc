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
 */

/**
 *
 *  data must match regular expression
 *
 *  directives:
 *  
 *      pattern     - perl compatible regular
 *                expression data must match
 *                against    /PATERN/
 *                default (undef)
 * 
 *
 *     reverse     - returns opposit value for match 
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 *
 */
class pregFilter extends SM_entityFilter {

    //configure this filter
    function filterConfig() {
        $this->directive['pattern'] = NULL;
        $this->directive['reverse'] = false;
    }

    // apply this filter 
    function filterThink() {
        if ($this->directive['pattern'] == '') {
            $this->debugLog("apply(): warning: pattern is empty, assuming success");
            return true;
        }
        else {
            if ($this->directive['reverse'] == true) {
                return !(preg_match($this->directive['pattern'], $this->data));
            } 
            return preg_match($this->directive['pattern'], $this->data);
        }
    }

}

?>