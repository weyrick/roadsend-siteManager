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

SM_sfLoadEntity('select');      // load our parent


/**
 *
 * a select box with a list of options, plus a text field to
 * input a new option. INHERITS selectEntity
 *
 * directives:
 *      same as selectEntity
 *          - plus -
 *          textSize        - size of the text input
 *          textMaxLength   - maxlen of the text input
 *          textFirst       - boolean; makes text box output first (haniotak@ucnet.uoc.gr)
 *     
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */ 
class comboBoxEntity extends selectEntity {

    /** 
     * set to not show value in the text box, in the case where the entity     
     * has a default value which we want to be default in the select box,
     * but no value in the text box for easy editing
     * @var bool
     */
    var $noShowText; 
        
    /**
    * setup this input entity 
    */
    function entityConfig() {

        $vName  = $this->varName;
        
        $tName = $vName.'-t';
        $sName = $vName.'-s';

        // setup some defaults
        $this->directive['textSize']        = 20;
        $this->directive['textMaxLength']   = 20;
        $this->directive['textFirst']       = false;
        $this->noShowText                   = false;

        // if our text box is set, set that to the main value
        if ($_POST[$tName] != '') {
            $_POST[$this->varName] = $_POST[$tName];
        }
        elseif (isset($_POST[$sName]))  {
            // otherwise, set the select box to it's previous value
            $_POST[$this->varName] = $_POST[$sName];         
        }
        else {          
            // show default value, not forms value
            $_POST[$this->varName] = $this->value;
            $this->noShowText = true;
        }

    }   


    /**
    * output function, overridden from selectEntity 
    */

    function entityThink() { 
	   
        // setup
        $tName = $this->varName.'-t';
        $sName = $this->varName.'-s';        

        // must reset value, because it might have been updated in setupEntity
        $this->value = $_POST[$this->varName];
        
        // if we have a function in our class to populate the list, run it
        // this is useful for class that inherit this class, for them to setup
        // the list (after it has been configured)
        if (method_exists($this, 'populateList'))
            $this->populateList();
    
        // if no options, display only the text box (haniotak@ucnet.uoc.gr)
        if ((!isset($this->directive['optionList']))||(!is_array($this->directive['optionList']))) {
            $this->showTextBox($this->varName);
            return 'N/A';
        }
    
        // do we already have one (or more) selected?
        // check passed vars
        $selArray = $this->inVarH->getPOST($this->varName);
        
        if ((!is_array($selArray))&&(!preg_match("/\w+\,*\w*/",$this->value))) {
	
            $selected[$this->value] = 'selected="selected"';
        }
        elseif (is_array($selArray)) {
            foreach($selArray as $v) 
                $selected[$v] = 'selected="selected"';             
        }
        
        // check for array or comma delimated list in $this->value
        if (isset($this->value)) {
            if (is_array($this->value)) {
                foreach ($this->value as $sV) 
                    $selected[$sV] = 'selected="selected"';                 
            }
            elseif (strstr($this->value,',')) {
                $sList = explode(',',$this->value);
                foreach ($sList as $sV) 
                    $selected[$sV] = 'selected="selected"'; 
            }
            else {
                $selected[$this->value] = 'selected="selected"';
            }
        }        

        // check which box renders first, text or select (haniotak@ucnet.uoc.gr)
        if ($this->directive['textFirst']) {
            $this->showSelectBox($selected, $sName);
            $this->showTextBox($tName);
        } else {
            $this->showTextBox($tName);
            $this->showSelectBox($selected, $sName);
        }

    }

    /**
    * showSelectBox($selected, $sName)
    * render HTML for our select box
    * $selected     array of pre-selected values
    * $sName        name of var in $_POST
    * added by haniotak@ucnet.uoc.gr
    */
    function showSelectBox($selected, $sName) {
        $this->say("<select $attr name=\"{$sName}\" size=\"{$this->directive['size']}\">\n");
        $this->say("<option value=\"\">-----</option>\n");        
        foreach ($this->directive['optionList'] as $item) {
            $title = key($item);
            $data  = $item[$title];
            if (isset($selected[$data]))
                $sel = $selected[$data];
            else
                $sel = '';
            $this->say("<option value=\"$data\" $sel>$title</option>\n");
        }
        $this->say("</select>\n");
    }

    /**
    * showTextBox($tName)
    * render HTML for our text box
    * $tName        name of var in $_POST
    * added by haniotak@ucnet.uoc.gr
    */
    function showTextBox($tName) {
        if (!$this->noShowText)
            $val = $this->value;
        else
            $val = '';

        $attr = $this->getCommonAttr();
        $this->say("<input $attr name=\"{$tName}\" value=\"$val\" type=\"text\" size=\"{$this->directive['textSize']}\" maxlength=\"{$this->directive['textMaxLength']}\" /><br />");
    }

}

?>
