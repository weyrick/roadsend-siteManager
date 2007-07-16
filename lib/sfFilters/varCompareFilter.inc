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
 *       11/29/2001 - all directives not already initilized were set to 'Null'.
 *                     This was done so that it would be posible to get a 
 *                     list of directives on the fly. (gollum)
 *
 */

/**                     
 *  compare data with contents of another variable in this form
 *
 *  directives:
 *   
 *      compVar     - variable in this form to compare against
 *      cType       - compare type: equal,notEqual
 *      allowBlank  - allow comparisons to blank values
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class varCompareFilter extends SM_entityFilter {

    // setup filter 
    function filterConfig() {
            
        $this->directive['compVar']     = NULL;
        $this->directive['cType']       = 'equal';
        $this->directive['allowBlank']  = false;
    
    }

    // apply this filter 
    function filterThink() {
            
        if (($this->directive['cType'] != 'equal')&&($this->directive['cType'] != 'notEqual')) {
            $this->debugLog("cType was invalid: must be 'equal' or 'notEqual'");
            return;
        }       
        
        $realVar = $this->directive['compVar'];
        
        // get the sumbmitted data from the variable we're looking for
        if ((isset($_POST[$realVar]))&&($_POST[$realVar] != '')) {
            if ($this->directive['cType'] == 'equal')
                return ($this->data == $_POST[$realVar]);
            else                
                return ($this->data != $_POST[$realVar]);
        }
        elseif ($this->directive['allowBlank']) {       
            if ($this->directive['cType'] == 'equal')
                return ($this->data == '');
            else                
                return ($this->data != '');
        }
        else {      
            if ($this->directive['cType'] == 'equal')
                return false;
            else
                return true;
        }
    
    }

}

?>