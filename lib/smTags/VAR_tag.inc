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
 * class representation of a VAR tag 
 */
class SM_smTag_VAR extends SM_smTag {

    /**
     * tag config
     */
    function tagConfig() {

        // always require name
        if (!isset($this->attributes['NAME'])) {
            $this->fatalErrorPage("VAR tag missing NAME attribute");
        }

        // var type will be 'global' or 'local'
        // default to local
        $this->directive['varType'] = 'local';
        if (isset($this->attributes['VARTYPE'])) {
            $this->directive['varType'] = $this->attributes['VARTYPE'];
        }

    }

    /**
     * called when template is reset
     */
    function tagReset() {
        $this->directive['varType'] = 'local';
    }

    /**
     * main think method. 
     */
    function tagThink() {
        
        // setup
        if (isset($this->attributes['INDEX']))
            $index = $this->attributes['INDEX'];
        else
            $index = NULL;

        // if it doesn't exist...
        if (isset($this->attributes['DEFAULT']))
            $default = $this->attributes['DEFAULT'];
        else
            $default = NULL;

        // addition by pascal van hecke 2003/03/13
        // setup
        if (isset($this->attributes['PROPERTY']))
            $property = $this->attributes['PROPERTY'];
        else
            $property = NULL;
        // end_addition by pascal van hecke 2003/03/13
        
        
        $var = $this->attributes['NAME'];
        $vVar = '';

        
        // if it's a global variable, make it global and output
        if ($this->directive['varType'] == 'global') {
            
            global $$var;
            $vVar = $$var;

        }

        // local
        if ($this->directive['varType'] == 'local') {
            
            // key was a single variable
            if (isset($this->directive[$var])) {
                $vVar = $this->directive[$var];
            }
            else {
                $this->debugLog("KEY specified for VAR tag [{$this->attributes['NAME']}] wasn't set as a directive");
                return;
            }

        }


        // output
        // if it's an array, and INDEX is set, use that
        if (is_array($vVar) && isset($index)) {
            $op = $vVar[$index];
        }
        elseif (is_object($vVar) && isset($property)) {
            $op = $vVar->{$property};
        }
        else {
            $op = $vVar;
        }
        
        // on match
        if (!empty($this->attributes['ONMATCH']) && !empty($this->attributes['DISPLAY'])) {

            if ($op == $this->attributes['ONMATCH']) {
                // matches, show display
                $op = $this->attributes['DISPLAY'];
            }
            else {
                // doesn't match, clear it out
                $op = '';
            }

        }


        // format 
        $formatOption = $this->attributes['FORMATOPTION'];

        if (!empty($this->attributes['FORMAT']) && !empty($op)) {
            $format = $this->attributes['FORMAT'];
            switch ($format) {
            
            // convert from mysqldate to english date
            case 'date':
                $op = SM_fromMysqlDate($op);
               if (!empty($formatOption)) {
                    $format = $formatOption;
                    $op = date($format,strtotime($op));
                } 
                break;
            case 'm/d/y':
                $format = 'm/d/y';
                $op = date($format,strtotime($op));
                break;
            case 'number':
                $op = number_format($op);
                break;
            case 'title':
                $op = ucwords(strtolower($op));
                break;
            case 'escaped':
                $op = addslashes($op);
                break;
            case 'prettyName':
                $f= substr($op,0,1);
                if($f == strtolower($f)) {
                    $op = SM_prettyName($op);
                }
                break;

            case 'ellipsis':
                if (empty($formatOption)) {
                    $formatOption = 25; // Default Length
                }

                if (strlen($op) > $formatOption) {
                    $op = substr($op,0,$formatOption-3). "...";
                }
                break;
            default:
                break;
            }
        
        }
        


        // prefix/postfix
        $prefix = '';
        $postfix = '';
        if (!empty($this->attributes['PREFIX']) && !empty($op)) {
            $prefix = html_entity_decode($this->attributes['PREFIX']);
        }
        if (!empty($this->attributes['POSTFIX']) && !empty($op)) {
            $postfix = html_entity_decode($this->attributes['POSTFIX']);
        }

        if (empty($op) && !empty($default))
            $op = $default;

        $op = $prefix.$op.$postfix;

        if ( (!empty($op)) || ((string)$op == '0')) {
            $this->say($op);
        }

    }

}

?>
