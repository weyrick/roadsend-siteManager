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
 *  directives:
 *  
 *   secondSelect   - should seconds be displayed
 *   
 *   hourFormat     - 12 hour or 24 hour in display
 *   hourStart      - 0-24 range for time
 *   hourEnd        - 0-24 range for time is 24 hours by default but can be truncted
 *             
 *   minuteType   - increments (5,10,15,30 ect)
 *
 *   time MUST be in format HH:MM:SS 
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class timeEntity extends SM_inputTypeEntity {

    var $hSel;
    var $mSel;
    var $sSel;

    var $hVarName;
    var $mVarName;
    var $sVarName;
    
    /**
    * setup this input entity
    */

    function entityConfig() {
        
        $varName   = $this->varName;

        // defaults
        $this->directive['secondSelect']  = false;
        $this->directive['hourStart']  = 0;
        $this->directive['hourEnd']  = $this->directive['hourStart'] + 24;
        
        // 12 hour clock
        $this->directive['hourFormat']   = "g A";
        // 24 hour clock $this->directive['hourFormat']   = "H";
        
        $this->directive['minuteIncrement']   = 15;
                
        $this->hSel         = '';
        $this->mSel         = '';
        $this->sSel         = '';

        $this->hVarName = $varName.'-H';
        $this->mVarName = $varName.'-i';
        $this->sVarName = $varName.'-s';
        
        // setup date variables: trigger newValue event by hand
        $this->eventNewValue();
        
    }   

    /**
    * run in entityConfig(), and also if user changes default value after entityConfig() runs
    * (through SM_inputEntity::setDefaultValue)
    */

    function eventNewValue() {

        // if seperate date vars are available for this varName, grab them here 
        if (isset($_POST[$this->hVarName]))
            $this->hSel = $_POST[$this->hVarName];
        if (isset($_POST[$this->mVarName]))
            $this->mSel = $_POST[$this->mVarName];
        if (isset($_POST[$this->sVarName]))
            $this->sSel = $_POST[$this->sVarName];

        // if we already have a value, and no single values, parse it out
        if (($this->value != '')&&(($this->hSel == '')||($this->mSel=='')||($this->sSel==''))) {
                
            // try to identify different formats
            // 
            if (preg_match("/(\d\d*)[:\/](\d\d*)[:\/](\d\d)/",$this->value, $regs)) {
                $this->hSel = $regs[1];
                $this->mSel = $regs[2];
                $this->sSel = $regs[3];
            }
        
            settype($this->hSel,'integer');
            settype($this->mSel,'integer');
            settype($this->sSel,'integer');
                                       
        }
        

        // if we have any of the *Sel vars now, setup the main one as well
        if (($this->hSel != '')&&($this->mSel!='')&&($this->sSel!='')) {
            $_POST[$this->varName]     = $this->hSel.':'.$this->mSel.':'.$this->sSel;
        }

    }


    /**
    * output
    */

    function entityThink() {
           
        $hSelO[$this->hSel] = 'selected="selected"';
        $mSelO[$this->mSel] = 'selected="selected"';
        $sSelO[$this->sSel] = 'selected="selected"';

        $attr = $this->getCommonAttr();

        $this->say("Hour: <select name=\"{$this->hVarName}\" $attr>\n");
        for ($i=$this->directive['hourStart']; $i<$this->directive['hourEnd']; $i++) {
            
            $hourName = date($this->directive['hourFormat'], mktime($i,0,0));
            (isset($hSelO[$i])) ? $s = $hSelO[$i] : $s = '';
            $this->say("<option value=\"$i\" $s>$hourName\n");

        }
        $this->say("</select>\n");
        $this->say("Min: <select name=\"{$this->mVarName}\" $attr>\n");
        for ($i=0; $i<60; $i=$i + $this->directive['minuteIncrement']) {
            (isset($mSelO[$i])) ? $s = $mSelO[$i] : $s = '';
            $this->say("<option value=\"$i\" $s>$i\n");
        }
        $this->say("</select>\n");

        // Second select or hidden seconds
        if ($this->directive['secondSelect']) {
            $this->say("Sec: <select name=\"{$this->sVarName}\" $attr>\n");
            for ($i=0; $i<60; $i++) {
                (isset($sSelO[$i])) ? $s = $sSelO[$i] : $s = '';
                $this->say("<option value=\"$i\" $s>$i\n");
            }
            $this->say("</select>\n");
        }
        else {
            // text box. there will be a filter on it
            $val = $this->ySel;
            $this->say("<input $attr type=\"hidden\" name=\"{$this->sVarName}\" value=\"00\" />\n");
        }
    
    
    }

}

?>
