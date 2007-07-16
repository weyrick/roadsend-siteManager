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
 *  data must be a phone number. default is
 *  US in the format XXX-XXX-XXXX but may be
 *  modified through directives
 *
 *  directives:
 *   
 *      format      - the format of the phone number
 *                should be a string of characters (X)
 *                where the numbers should go.
 *                default (XXX-XXX-XXXX)
 *   
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class phoneFilter extends SM_entityFilter {

    // constructor 
    function filterConfig() {
            
        $this->directive['format'] = "XXX-XXX-XXXX";
    
    }


    // apply this filter 
    function filterThink() {
    
        $pat = '/^'.$this->directive['format'].'$/';
        $pat = preg_replace(array("/[Xx]/","/\-/","/\(/","/\)/","/\s/"),array("\\d","\\-","\\(","\\)","\s*"),$pat);

        // show them the format if it's bad
        $this->directive['badMessage'] .= "<br />Format: {$this->directive['format']}";
        
        return preg_match($pat, $this->data);
            
    }

}

?>