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
 *                     list of directives on the fly.
 *
 */

/**
 *
 *  input object for check boxes. $$varName will end
 *  up being an array when the data has been verified.
 *  
 *  directives:
 *  
 *      optionBreak - (bool) when set to true, when the options are 
 *                      printed, they will each be on their own
 *                      line, ie, each will be followed by a 
 *                      <BR> (default false)
 *  
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class checkBoxEntity extends SM_inputTypeEntity {
    

    /**
    * configure this entity. run in constructor of inputTypeEntity
    */
     
    function entityConfig() {

        // setup some defaults
        $this->directive['optionBreak'] = false;
        $this->directive['preOut']         = '';
        $this->directive['postOut']        = '';
        $this->directive['optionList']     = NULL;
        $this->directive['entityClassTag'] = NULL;
        $this->directive['forceArray']     = false; // if true, return value will always be array, even if single item
        $this->directive['numPerRow']      = 0; // if > 0, arrange in table
        $this->directive['tableExtra']     = '';
                
    }

    /**
     * add an option to the list
     * @param string $title title to use for new option
     * @param string $data data to use for new option. if same as title, leave blank
     */
    function addOption($title, $data='') {  
    
        if ($data === '')
            $data = $title;

        $this->directive['optionList'][] = array($title=>$data); 

    }

    /**
     * set the list directly with an array (hash) passed to us
     * array should be $title => $data
     * @param array $newList new option list
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


        // go ahead and return if there's no options
        if (!is_array($this->directive['optionList'])) {
            $this->debugLog("output: no options for checkbox to output");
            return 'N/A';
        }
        
        if ($this->directive['optionBreak'])
            $br = '<br />';
        else
            $br = '';

        // do we already have one (or more) selected? 
        // check passed vars
        $selArray = $this->inVarH->getPOST($this->varName);
        if (is_array($selArray)) {
            foreach($selArray as $v) 
                $selected[$v] = 'checked="checked"';              
        }
        
        // check for array or comma delimated list in $this->value
        if (!empty($this->value) || ($this->value == '0')) {
            if (is_array($this->value)) {
                foreach ($this->value as $sV) 
                    $selected[$sV] = 'checked="checked"';                 
            }
            elseif (strstr($this->value,',')) {
                $sList = explode(',',$this->value);
                foreach ($sList as $sV) 
                    $selected[$sV] = 'checked="checked"'; 
            }
            else {
                $selected[$this->value] = 'checked="checked"';
            }
        }        
                
        if (!$this->directive['forceArray']) {
            // if there is more than one checkbox, make it an array. otherwise, just a variable
            (sizeof($this->directive['optionList']) > 1) ? $makeArray = '[]' : $makeArray = '';
        }
        else {
            // force array
            $makeArray = '[]';
        }
        
        $attr = $this->getCommonAttr();

        if ($this->directive['numPerRow'] > 0) {
            $this->say("<table {$this->directive['tableExtra']}><tr>");
        }

        $cnt = 0;
        foreach ($this->directive['optionList'] as $item) {       
            
            if ($this->directive['numPerRow'] > 0) {
                $this->say('<td>');
            }
            
            $title = key($item);
            $data  = $item[$title];
            (isset($selected[$data])) ? $sel = 'checked="checked"' : $sel = '';
            
            $this->say("{$this->directive['preOut']}<input type=\"checkbox\" $attr name=\"{$this->varName}{$makeArray}\" value=\"$data\" $sel /> ");

            if (!empty($this->directive['entityClassTag']))
                $this->say('<span class="'.$this->directive['entityClassTag'].'">');

            $this->say($title);

            if (!empty($this->directive['entityClassTag']))
                $this->say('</span>');

            $this->say("$br{$this->directive['postOut']}\n");
            
            if ($this->directive['numPerRow'] > 0) {
                $this->say('</td>');
                $cnt++;
                if ($cnt >= $this->directive['numPerRow']) {
                    $this->say('</tr><tr>');
                    $cnt = 0;
                }
            }
            
        }
        
        if ($this->directive['numPerRow'] > 0) {
            $this->say('</tr></table>');
        }

    }

}

?>
