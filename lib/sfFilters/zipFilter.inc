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
 * Author(s): (gollum@roadsend.com)
 *
 */

/**
 *
 *  data must be a zip code. Severak default values
 *  are supplied, but more can be added.
 *
 *  directives:
 *   
 *      format      -  Which format to use. We can ues
 *						any of the built-in ones, or any added
 *						on the fly. The default format is the
 *						'US_BOTH' option.  Current choices
 * 					include:
 *
 *							US_SHORT ('XXXXX')
 *							US_LONG ('XXXXX-XXXX')
 *							US_BOTH ('XXXXX','XXXXX-XXXX');
 * 
 *  Useful functions:
 *
 *		addFormat($formatName, $value)
 *			This function takes the format name as one parameter
 *			and a comma seporated list of masks as the second. an
 *			'X' or an 'x' are treated as any number and every other
 *			symbol is taken as is.
 *
 *			Examles:
 *				With a mask of 'XXXXX-XXXX' valid input would be 
 *				'12345-6789' or '98765-4321' but not '123456789' or '12345-abcd'
 *
 *				With a mask of '-XXXX,XXXX-' valid imput would be
 *				'-1234,5678-' or '-8765,4321-' but not '1234,5678'  
 *  
 * @author <gollum@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class zipFilter extends SM_entityFilter {
	
	//Array of format choices
	var $formatChoiceList;


    // constructor 
    function filterConfig() {

		//add our 'standard' choices
		$this->addFormat('US_SHORT',"'XXXXX'");
		$this->addFormat('US_LONG',"'XXXXX-XXXX'");
		$this->addFormat('US_BOTH',"'XXXXX','XXXXX-XXXX'");
		
        $this->directive['format'] = 'US_BOTH'; 

    }


    // apply this filter 
    function filterThink() {

		//get our format
		$formatList = $this->getFormat();

    	//check each format in the list
    	$results = false;
    	foreach ($formatList as $filter)  {
        	$pat = '/^'.$filter.'$/';
        	$pat = preg_replace(array("/[Xx]/","/\-/","/\(/","/\)/","/\s/"),array("\\d","\\-","\\(","\\)","\s*"),$pat);
        	$results = preg_match($pat, $this->data);

        	//if we have a match, we're done here.
        	if($results) break;

    	}

        return $results; 

    }


	//generate the format.
	function getFormat(){
		
		//make sure we have the upper case version.
		$myFormat = strtoupper($this->directive['format']);

		//if we don't have anything, make it 'US_BOTH'
		if(empty($myFormat)) $myformat = 'US_BOTH';

		//generate an array form the string.
		eval("\$formatList = array({$this->formatChoiceList[$myFormat]});");
		
		if(empty($formatList)) eval("\$formatList = array({$this->formatChoiceList['US_BOTH']});");

      	// show them the desired format if it's bad
      	$display = '';
      	foreach ($formatList as $format) {
      		if(strlen($display) > 0) $display .= ' or ';
      		$display .= $format;	
      	}
      	$this->directive['badMessage'] .= "<br />Format: $display";

		//reutrn our array
		return $formatList;
		
	}


	//Simple function to add items to our 'format choice list'
	function addFormat($formatName, $value) {
		
		$this->formatChoiceList[strtoupper($formatName)] = $value;
		
	}

}

?>