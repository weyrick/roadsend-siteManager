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
 *  input object for radio buttons. almost identical
 *  to checkBoxes, however $$varName will NOT be an
 *  array when data is verified.
 *  
 *  directives:
 *      preOut, postOut - wrap INPUT tags between these strings
 *      optionBreak - when set to true, when the options are 
 *                printed, they will each be on their own
 *                line, ie, each will be followed by a 
 *                <BR>
 *            default (false)
 * 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class radioEntity extends SM_inputTypeEntity {


    /**
    * configure the entity 
    */

    function entityConfig() {

        // setup some defaults
        $this->directive['optionBreak'] = false;
        $this->directive['optionList']  = NULL;
        $this->directive['preOut']      = '';
        $this->directive['postOut']     = '';
        $this->directive['entityClassTag']     = NULL;
        $this->directive['optionAlign'] = false;

        // hash of n => string, where "string" is output every nTH row
        // set to array(3, '</tr><td>') for example, to change rows every third item
        // in combo with preOut/postOut of <td></td>
        $this->directive['onCntOutput']   = NULL; 
        
    }

    /**  
     * add an option
     * @param string $title title of new option
     * @param string $data data of new option. if same as title, leave blank
     */
    function addOption($title, $data='') {  
    
        if ($data === '')
            $data = $title;
        $this->directive['optionList'][] = array($title=>$data);

    }

    /** 
     * set the list directly with an array (hash) passed to us
     * array should be $data => $title
     * @param array $newList has of option list to use
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
    * output 
    */

    function entityThink() {


        // go ahead and return if there's no options
        if (!is_array($this->directive['optionList'])) {
            $this->debugLog("output: no options for radio to output");
            return 'N/A';                      
        }
                            
        ($this->directive['optionBreak'] && (!$this->directive['optionAlign'])) ? $br = '<br />' : $br = '';

        $tStart = '';
        $tEnd = '';
        $trStart = '';
        $trEnd = '';
        $ltdStart = '';
        $tdStart = '';
        $tdEnd = '';
        if ($this->directive['optionAlign']) {
            $tStart = '<table>';
            $tEnd = '</table>';
            $trStart = '<tr>';
            if (!empty($this->directive['entityClassTag']))
                $tdStart = "<td class='".$this->directive['entityClassTag']."'>";
            else 
                $tdStart = '<td>';
            $ltdStart = '<td valign="top">';
            $trEnd = '</tr>';
            $tdEnd = '</td>';
        }
        
        // do we already have one (or more) selected? 
        $selected[$this->value] = 'checked="checked"';
        
        $attr = $this->getCommonAttr();        
        $this->say($tStart);
        $cnt = 0;
        foreach ($this->directive['optionList'] as $item) {

            $title = key($item);
            $data  = $item[$title];
            (isset($selected[$data])) ? $sel = 'checked="checked"' : $sel = '';

            $this->say("{$trStart}{$ltdStart}{$this->directive['preOut']}<input $attr type=\"radio\" name=\"$this->varName\" value=\"$data\" $sel /> {$tdEnd}{$tdStart}");
            
            if ((!empty($this->directive['entityClassTag'])) && (!$this->directive['optionAlign']))
                $this->say('<span class="'.$this->directive['entityClassTag'].'">');

            $this->say($title); 

            if (!empty($this->directive['entityClassTag']))
                $this->say("</span>{$tdEnd}{$trEnd}");

            $this->say("$br{$this->directive['postOut']}\n");

            $cnt++;
            if (is_array($this->directive['onCntOutput']) &&
               (isset($this->directive['onCntOutput'][$cnt])))
               $this->say($this->directive['onCntOutput'][$cnt]);

        }
        $this->say($tEnd);

    }

}

?>
