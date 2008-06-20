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
 *  input entity suitable for date entry. the month
 *  and day are select boxes - the year may either
 *  be a select box or years, or text input box
 *   
 *  the final date returned is in the format YYYY/DD/MM
 *  
 *  directives:
 *  
 *   yearSelect  - when set to true, the year will be a select
 *             box rather than a text input box. if using
 *             this feature, be sure to set yearStart and
 *             yearEnd, which will define what years are
 *             included in the select box.
 *             default (true)
 *             
 *   yearStart   - the year the select box will start at
 *             default (current year)
 *   
 *   yearEnd     - the year the select box will end at
 *             default (current year+5)
 *             
 *   monthType   - either 'long' or 'short'. affects displayed month name
 *
 *   addNoOption - when true, one of the options will be "no option",
 *                  ie, you can select a blank option to specify
 *                  the value is set with noOptionData
 *
 *   noOptionData    - defaults to 0.
 *   noOptionDisplay - defaults to --
 *
 *	 showMonth - Display the month select box
 *	 showDay   - Display the day select box
 *   showYear  - Display the year select box
 *  
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class dateEntity extends SM_inputTypeEntity {

    var $dSel;
    var $mSel;
    var $ySel;

    var $dVarName;
    var $mVarName;
    var $yVarName;
    
    /**
    * setup this input entity
    */

    function entityConfig() {
        
        $varName   = $this->varName;

        // defaults
        // changed from $this->directive to $this->addDirective (gollum 10/25/2004)
        $this->addDirective('yearSelect',true);
        $this->addDirective('yearStart',date("Y"));
        $this->addDirective('yearEnd',date("Y")+5);
        $this->addDirective('monthType','M');
        
        // New Directives for a "no option" on the date (gollum 10/25/2004)
        $this->addDirective('addNoOption',FALSE);
        $this->addDirective('noOptionData','0000-00-00');
        $this->addDirective('noOptionDisplay','--');
                
        $this->dSel         = '';
        $this->mSel         = '';
        $this->ySel         = '';

        $this->dVarName = $varName.'-d';
        $this->mVarName = $varName.'-m';
        $this->yVarName = $varName.'-y';
        
        // setup date variables: trigger newValue event by hand
        $this->eventNewValue();
        
        // Select which components we want to use
        // Added 1/19/2005 (gollum)
        $this->addDirective('showMonth',TRUE);
        $this->addDirective('showDay',TRUE);
        $this->addDirective('showYear',TRUE);
        
    }   

    /**
    * run in entityConfig(), and also if user changes default value after entityConfig() runs
    * (through SM_inputEntity::setDefaultValue)
    */

    function eventNewValue() {

        // if seperate date vars are available for this varName, grab them here 
        if (isset($_POST[$this->mVarName]))
            $this->mSel = $_POST[$this->mVarName];
        if (isset($_POST[$this->dVarName]))
            $this->dSel = $_POST[$this->dVarName];
        if (isset($_POST[$this->yVarName]))
            $this->ySel = $_POST[$this->yVarName];

        // if we already have a value, and no single values, parse it out
        if (($this->value != '')&&(($this->mSel == '')||($this->dSel=='')||($this->ySel==''))) {
                
            // try to identify different formats
            // [M]M-[D]D-YY[YY]
            if (preg_match("/(\d\d*)[-\/](\d\d*)[-\/](\d\d\d*\d*)/",$this->value, $regs)) {
                $this->mSel = $regs[1];
                $this->dSel = $regs[2];
                $this->ySel = $regs[3];
            }
            
            // YYYY-[M]M-[D]D 
            if (preg_match("/(\d\d\d\d)[-\/](\d\d*)[-\/](\d\d*)/",$this->value, $regs)) {
                $this->ySel = $regs[1];
                $this->mSel = $regs[2];
                $this->dSel = $regs[3];
            }
        
            settype($this->mSel,'integer');
            settype($this->dSel,'integer');
            settype($this->ySel,'integer');
                                       
        }
        

        // if we have any of the *Sel vars now, setup the main one as well
        if (($this->mSel != '')&&($this->dSel!='')&&($this->ySel!='')) {
            $_POST[$this->varName]     = $this->ySel.'-'.$this->mSel.'-'.$this->dSel;
        }

        // if the YEAR is not all numeric, complain
        if (($this->ySel != '')&&(!preg_match("/^\d\d\d\d$/",$this->ySel))) {
            $this->isValid = false;
            $this->fixMessage = "Year is invalid";
        }

        if (!empty($this->mSel) &&
            !empty($this->dSel) &&
            !empty($this->ySel) &&
            !checkdate($this->mSel, $this->dSel, $this->ySel)) {
            $this->isValid = false;
            $this->fixMessage = "The date is invalid";
        }

    }

    function getValue() {
        // we override input entity and always return POST version, which is set in eventNewValue
        return $_POST[$this->varName];
    }


    /**
    * output
    */

    function entityThink() {
           
        $dSelO[$this->dSel] = 'selected="selected"';
        $mSelO[$this->mSel] = 'selected="selected"';
        $ySelO[$this->ySel] = 'selected="selected"';

        // Get these for later (gollum 10/25/2004)
        list($noDataYear,$noDataMonth,$noDataDay) = explode("-",$this->getDirective('noOptionData'));
        $noOptionDisplay = $this->getDirective('noOptionDisplay');
        
        $attr = $this->getCommonAttr();

		// Only show this pulldown if the directive is set to 'TRUE'
		// added 1/19/2005 (gollum)
		if ($this->getDirective('showMonth')){
	        $this->say("<select name=\"{$this->mVarName}\" $attr>\n");
	        
	        // If we want to add the "no option" (gollum 10/25/2004)
	        if ($this->getDirective('addNoOption')) {
	            (isset($mSelO[$i])) ? $s = $mSelO[$i] : $s = '';
	            $this->say("<option value=\"$noDataMonth\" $s>$noOptionDisplay</option>\n");
	        }
	        
	        for ($i=1; $i<13; $i++) {
	            $monthName = date($this->directive['monthType'], mktime(0,0,0,$i,1,1999));
	            (isset($mSelO[$i])) ? $s = $mSelO[$i] : $s = '';
	            $this->say("<option value=\"$i\" $s>$monthName</option>\n");
	        }
	        $this->say("</select>\n");
		} else {
			// set the var to '00' so that it doesn't actually break anything
			$this->say("<input type=\"hidden\" name=\"{$this->mVarName}\" value=\"00\" $attr>\n");
		}
		
		// Only show this pulldown if the directive is set to 'TRUE'
		// added 1/19/2005 (gollum)
        if ($this->getDirective('showDay')) {
	        $this->say("<select name=\"{$this->dVarName}\" $attr>\n");
	
	        // If we want to add the "no option" (gollum 10/25/2004)
	        if ($this->getDirective('addNoOption')) {
	            (isset($dSelO[$i])) ? $s = $dSelO[$i] : $s = '';
	            $this->say("<option value=\"$noDataDay\" $s>$noOptionDisplay</option>\n");
	        }
	
	        for ($i=1; $i<32; $i++) {
	            (isset($dSelO[$i])) ? $s = $dSelO[$i] : $s = '';
	            $this->say("<option value=\"$i\" $s>$i</option>\n");
	        }
	        $this->say("</select>\n");
	    } else {
	    	// set the var to '00' so that it doesn't actually break anything
			$this->say("<input type=\"hidden\" name=\"{$this->dVarName}\" value=\"00\" $attr>\n");
		}

		// Only show this pulldown if the directive is set to 'TRUE'
		// added 1/19/2005 (gollum)
        if ($this->getDirective('showYear')) {
	        // year select or text box
	        if ($this->directive['yearSelect']) {
	            $this->say("<select name=\"{$this->yVarName}\" $attr>\n");
	
	            // If we want to add the "no option" (gollum 10/25/2004)
	            if ($this->getDirective('addNoOption')) {
	                (isset($ySelO[$i])) ? $s = $ySelO[$i] : $s = '';
	                $this->say("<option value=\"$noDataYear\" $s>$noOptionDisplay</option>\n");
	            }
	
	            for ($i=$this->directive['yearStart']; $i<$this->directive['yearEnd']; $i++) {          
	                (isset($ySelO[$i])) ? $s = $ySelO[$i] : $s = '';
	                $this->say("<option value=\"$i\" $s>$i</option>\n");
	            }
	            $this->say("</select>\n");
	        }
	        else {
	            // text box. there will be a filter on it
	            $val = $this->ySel;
	            $this->say("<input $attr type=\"text\" size=\"4\" maxlength=\"4\" name=\"{$this->yVarName}\" value=\"$val\" />\n");
	        }
        } else {
        	// set the var to '0000' so that it doesn't actually break anything
			$this->say("<input type=\"hidden\" name=\"{$this->yVarName}\" value=\"0000\" $attr>\n");
		}
    
    
    }

}

?>
