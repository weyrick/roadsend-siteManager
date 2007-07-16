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
 *                     list of directives on the fly.(gollum)
 *
 *       1/15/2002 - Numeric and minValue directives added.(pym)
 *       3/2/2002  - add directive for decimalCharacter (weyrick)
 */

/**
 *  data must consist entirely of digits    
 *                          
 *  directives:                     
 *   
 *  length      - must be exactly n digits long
 *            default (undef)
 *
 *  sizeMinMax  - must be between min and 
 *            max digits long. in the format 'min,max'
 *            default (undef)
 *
 *  decimal     - when true, must be a decimal number
 *            default (false)
 *
 *  numeric     - allows any numeric type (int or float)
 *            
 *
 *  minValue     - lowest valid result (applys only to numeric checks)
 *               - problems occur when set to 0. 
 *
 *  decimalCharacter - character used for decimal place (default: period [US style])
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class numberFilter extends SM_entityFilter {

    // Configure this filter
    function filterConfig() {
        $this->directive['length']=NULL;
        $this->directive['sizeMinMax']=NULL;
        $this->directive['decimal']=NULL;
        $this->directive['numeric']=NULL;
        $this->directive['minValue']=NULL;    
        $this->directive['decimalCharacter'] = '.'; 
    }

    // apply this filter 
    function filterThink() {

        // build the pattern
        if (!isset($this->directive['numeric']))
            $this->directive['numeric'] = false;
        
        // Testing on a numeric value 
        if ($this->directive['minValue']) {
        
            // test for min value value
            if ($this->directive['minValue']) {
                if ($this->data <= $this->directive['minValue']) {
                    return false;
                }
            }
            
            // test for a numeric value
            return (is_numeric ($this->data));
        }

        // testing on a patern value
        $pat = "/^\d";
        
        if (!isset($this->directive['decimal']))
            $this->directive['decimal'] = false;
        
        if (!$this->directive['decimal']) {
            if (isset($this->directive['length']))
                $pat .= '{'.$this->directive['length'].'}';
            elseif (isset($this->directive['sizeMinMax'])) {
                $pat .= '{'.$this->directive['sizeMinMax'].'}';
            }
            else {
                $pat .= '+';
            }
        }
        else {
            $pat .= '*\\'.$this->directive['decimalCharacter'].'\d+';
        }
        
        $pat .= "$/";

        return preg_match($pat, $this->data);
    }

}

?>