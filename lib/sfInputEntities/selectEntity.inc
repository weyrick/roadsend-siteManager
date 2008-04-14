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
 *
 *  select input object. may be muliple select. used as base class
 *  for many other entities.
 *  
 *  directives:
 *  
 *      multiple  - when set to true, the select box will
 *                allow multiple values to be selected
 *                at once. the $$varName will end up
 *                being an array once all input has
 *                been verified.
 *                default (false)
 *
 *  size        - the size of the select box (verticle)
 *            only valid for multiple selects
 *            default (1)
 *            
 *  addNoOption - when true, one of the options will be "no option",
 *                  ie, you can select a blank option to specify
 *                  the value is set with noOptionData
 *                  
 *  noOptionData    - defaults to blank.
 *  noOptionDisplay - defaults to -----         
 *                  
 *  cutoff  - defaults to 0. If set, determines the length at which the title
 *            string in our select drop down is cut off. helps prevent 
 *            huge select boxes which may screw up your layout. (haniotak)
 *
 *  cutoffPad   
 *          - defaults to '...', is the string added to the end of the title
 *            string when cutoff kicks in (haniotak)
 *
 *  allowCommaValues    
 *          - defaults to true. If set we will check for comma-separated multiple values
 *          in value string to use as preselected in a multiple select box.
 *          when defined as false, we'll leave commas alone. (haniotak)
 *          unset if your values contain commas and you want setDefaultValue() to work
 *
 *  populateList()  (optional)
 *  
 *      if this function is defined, it will be run by
 *      the output function before doing any work. it is
 *      usefull for classes that inherit this one, that
 *      need to populate the select list in various ways
 *      (ie, see dbSelectEntity or stateListentity)
 *      
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class selectEntity extends SM_inputTypeEntity {

    //  setup entity 
    function entityConfig() {

        // defaults
        $this->directive['multiple']            = false;
        $this->directive['size']                = 1;
        $this->directive['addNoOption']         = false;
        $this->directive['noOptionData']        = "";
        $this->directive['noOptionDisplay']     = "-----";
        $this->directive['optionList']          = NULL;

        $this->directive['enableOnSubmit']      = false;
        
        $this->directive['allowCommaValues']    = true;
        $this->directive['cutoff']              = 0;
        $this->directive['cutoffPad']           = '...';
    }

    /**     
     * add an option to the list
     * @param string $title title of the new option
     * @param string $data data of the new option. if same as title, leave blank
     */
    function addOption($title, $data='') {  
    
        if ($data === '')
            $data = $title;

        if ($this->directive['cutoff'] && strlen($title) > $this->directive['cutoff']) { 
            $title = substr($title, 0, $this->directive['cutoff']).$this->directive['cutoffPad'];
        }

        $this->directive['optionList'][] = array($title=>$data); 

    }

    /**
     * sort the option list in alphabetical order by title
     * this is rather ineffecient...
     */
    function sortByTitle() {

        if (sizeof($this->directive['optionList'])) {
            foreach ($this->directive['optionList'] as $sV) {
                $title = key($sV);
                $data = $sV[$title];
                $oL[$title] = $data;
            }
            ksort($oL);
            $this->directive['optionList'] = NULL;
            foreach ($oL as $title => $data) {
                $this->directive['optionList'][] = array($title => $data);
            }
        }

    }
    
    /**
     * return the number of options currently in the list
     * @return int number of options in the list
     */
    function optionCount() {
        return count($this->directive['optionList']);
    }

    /**
     * set the list directly with an array (hash) passed to us
     * array should be $title => $data
     * @param array $newList complete data set that should appear in the list
     */
    function setList($newList) {
        
        if (!is_array($newList)) {
            $this->debugLog("setList: variable passed was not an array");
            return false;
        }

        foreach ($newList as $title => $data) {
            $this->addOption($title,$data);
        }
        
    }
        
    /**
     * output the entity
     */
    function entityThink() {
        
        // if we have a function in our class to populate the list, run it
        // this is useful for class that inherit this class, for them to setup
        // the list (after it has been configured)
        if (method_exists($this, 'populateList'))
            $this->populateList();

        
        // if no options, dont display
        if ( ((!isset($this->directive['optionList']))||(!is_array($this->directive['optionList']))) &&
              (!$this->directive['addNoOption']) ) {
            $this->debugLog("output: no options for select to output");
            return 'N/A';
        }
    
        if ($this->directive['multiple']) {
            $braces = '[]';
            $mult   = 'multiple="multiple"';
            if ($this->directive['size'] == 1)
                $this->directive['size'] = 3;
        }
        else {
            $braces = '';
            $mult = '';
        }

        /* do we already have one (or more) selected? */
        // check passed vars
        $selArray = $this->inVarH->getPOST($this->varName);        
        if (is_array($selArray)) {
            foreach($selArray as $v) 
                $selected[$v] = 'selected="selected"';             
        }
        
        // check for array or comma delimated list in $this->value
        if (!empty($this->value) || ($this->value == '0')) {
            if (is_array($this->value)) {
                foreach ($this->value as $sV) 
                    $selected[$sV] = 'selected="selected"';                 
            } elseif (strstr($this->value,',') && $this->directive['allowCommaValues']) {
                $sList = explode(',',$this->value);
                foreach ($sList as $sV) 
                    $selected[$sV] = 'selected="selected"'; 
            } else {
                $selected[$this->value] = 'selected="selected"';
            }
        }        

        if ($this->directive['enableOnSubmit']) {
            $onSubmit = "onchange='this.form.submit()'";
        }
        
        $attr = $this->getCommonAttr();
        if ($this->isLegacyLayout()) {
            $this->say("<select name=\"{$this->varName}{$braces}\" $attr $mult $onSubmit size=\"{$this->directive['size']}\">\n");
        }
        else {
            $this->say("<select name=\"{$this->varName}{$braces}\" $attr $onSubmit>\n");
        }

        if ($this->directive['addNoOption']){

            // No Option Data
            if (isset($this->directive['noOptionData'])) {
               $noOptionData = $this->directive['noOptionData'];
            } else {
               $noOptionData = "";
            }
            // No Option Display Value
            if (isset($this->directive['noOptionDisplay'])) {
               $noOptionDisplay = $this->directive['noOptionDisplay'];
            } else {
               $noOptionDisplay = "------";
            }
            $this->say("<option value=\"$noOptionData\">$noOptionDisplay</option>\n");
        }
        foreach ($this->directive['optionList'] as $item) {

            $title = key($item);
            $data  = $item[$title];

            if (isset($selected[$data]))
                $sel = $selected[$data];
            else
                $sel = '';

            $data = htmlspecialchars($data);
            $this->say("<option value=\"$data\" $sel>$title</option>\n");

        }
        $this->say("</select>\n");
        
    }
}

?>
