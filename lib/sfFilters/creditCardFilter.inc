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
 *                      
 *  input must be a valid credit card number
 *  cc MOD 10 check routine adapted from  Brett  Error (brett@interwebdesign.com)
 *
 *  directives:
 *  cardType    - credit card type. must be mc, visa, amex or discover (optional)  
 *  cardVar     - variable to look up at run time of credit card type (this is probably what you want)
 *    
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class creditCardFilter extends SM_entityFilter {


    /** setup filter */
    function filterConfig() {

	global $SM_develState;
	
        $this->directive['cardType']        = 'unknown';
        $this->directive['cardVar']         = '';

	$this->directive['allowTestCards']  = ($SM_develState) ? true : false;
    
    }

    /**  takes  a  string  and  returns  an  array  of  characters (private) */
    function  toCharArray($input){ 
         $len  =  strlen($input); 
         for  ($j=0;$j<$len;$j++){ 
                 $char[$j]  =  substr($input,  $j,  1);        
         } 
         return  ($char); 
    } 

    /** apply this filter */
    function filterThink() {
                    
        // if cardVar is set, try to get the card type from it
        if (!empty($this->directive['cardVar'])) {
            $realVar = $this->directive['cardVar'];
            if ((isset($_POST[$realVar]))&&($_POST[$realVar] != ''))
                $this->directive['cardType'] = $_POST[$realVar];
        }       
        
        // Clean  up  input
        $type  =  strtolower($this->directive['cardType']); 
        $ccnum  =  ereg_replace( '[-[:space:]]',  '',$this->data);  
        
        // Do  type  specific  checks
        switch  ($type)  { 
        case 'unknown':
            // Skip  type  specific  checks
            break;
        case ('mc'):
        case ('mastercard'):
             if  (strlen($ccnum)  !=  16  ||  !ereg( '^5[1-5]',  $ccnum))  return false; 
             break;
        case ($type  ==  'visa'):
             if  ((strlen($ccnum)  !=  13  &&  strlen($ccnum)  !=  16)  ||  substr($ccnum,  0,  1)  !=  '4')  return false; 
             break;
        case ($type  ==  'amex'):
             if  (strlen($ccnum)  !=  15  ||  !ereg( '^3[47]',  $ccnum))  return false; 
             break;
        case ($type  ==  'discover'):
             if  (strlen($ccnum)  !=  16  ||  substr($ccnum,  0,  4)  !=  '6011')  return false; 
             break;
        default: 
             /* invalid  type  entered */
             $this->debugLog("invalid credit card type: $type");
             return false; 
        } 
                
        //  Start  MOD  10  checks
        $dig  =  $this->toCharArray($ccnum); 
        $numdig  =  sizeof  ($dig); 
        $j  =  0; 
        for  ($i=($numdig-2);  $i>=0;  $i-=2){ 
             $dbl[$j]  =  $dig[$i]  *  2; 
             $j++; 
        }        
        $dblsz  =  sizeof($dbl); 
        $validate  =0; 
        for  ($i=0;$i<$dblsz;$i++){ 
             $add  =  $this->toCharArray($dbl[$i]); 
             for  ($j=0;$j<sizeof($add);$j++){ 
                     $validate  +=  $add[$j]; 
             } 
        $add  =  ''; 
        } 
        for  ($i=($numdig-1);  $i>=0;  $i-=2){ 
             $validate  +=  $dig[$i];  
        } 


	// test cards?
	if (!$this->directive['allowTestCards']) {
	    switch ($ccnum) {
	    case '4111111111111111': // visa
	    case '5500000000000004': // mc
	    case '340000000000009':  // amex
	    case '6011000000000004': // discover
		return false;
	    }
	}
	
        if  (substr($validate,  -1,  1)  ==  '0')  
            return  true; 
        else  
            return  false; 
                        
    }

}

?>
