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
 * class representation of a SF (smartForm) tag 
 */
class SM_smTag_SF extends SM_smTag {

    /**
     * main think method. 
     * this will run the module
     */
    function tagThink() {

        // parent form
        $myForm = $this->directive['parentForm'];

        if (empty($myForm)) {
            $this->debugLog("parent smartForm was null");
            return;
        }

        // check for parent smart form
        if ((!is_object($myForm))&&(get_class($myForm))) {
            $this->debugLog("parent smartForm wasn't specified for SF tag");
            return;
        }

        switch ($this->attributes['DATA']) {
        case 'ENDFORM':
            $myForm->addDirective('formIsEnded',true);
            $this->say('</form>');
            break;
        case 'HEADER':
            $this->say($myForm->header);
            break;
        case 'TITLE':
            $ent = $myForm->getFormEntity($this->attributes['VAR']);
            if (is_object($ent)) {
                $this->say($ent->getTitle());
            }
            else {
                $this->debugLog("requested SF variable [{$this->attributes['VAR']}] was not found in parent smartForm");
                return;
            }
            break;
        case 'ENTITY':
            $ent = $myForm->getFormEntity($this->attributes['VAR']);
            if (is_object($ent)) {
                $this->say($ent->output());
            }
            else {
                $this->debugLog("requested SF variable [{$this->attributes['VAR']}] was not found in parent smartForm");
                return;
            }
            break;
        case 'SUBMIT':
            if (!empty($this->attributes['TITLE'])) {
                $title = $this->attributes['TITLE'];
            }
            else {
                $title = 'Submit Form';
            }            
            SM_sfLoadEntity('submit');
            $submitButton = new SM_formEntity('_submitBut'.'_'.$myForm->formID, '', 'submit', false, $myForm);
            $submitButton->inputEntity->addDirective('value',$title);
            if (!empty($myForm->directive['submitImage'])) {            
                $submitButton->inputEntity->addDirective('image',true);
                $submitButton->inputEntity->addDirective('src',$myForm->directive['submitImage']);
            }
            $this->say($submitButton->output());
            break;
        case 'RESET':
            if (!empty($this->attributes['TITLE'])) {
                $title = $this->attributes['TITLE'];
            }
            else {
                $title = 'Reset Form';
            }            
            SM_sfLoadEntity('reset');
            $resetButton = new SM_formEntity('_resetBut'.'_'.$myForm->formID, '', 'reset', false, $myForm);
            $resetButton->inputEntity->addDirective('value',$title);
            if (!empty($myForm->directive['resetImage'])) {            
                $resetButton->inputEntity->addDirective('image',true);
                $resetButton->inputEntity->addDirective('src',$myForm->directive['resetImage']);
            }
            $this->say($resetButton->output());
            break;
        default:
            $this->debugLog("invalid DATA type: {$this->attributes['DATA']} - ignoring");
            break;
        }

    }

    /**
     * tag config. handles parsing of name and directives, as well
     * as the loading and instantiation of the module
     */
    function tagConfig() {

        global $SM_siteManager;

        // module tag
        // requires DATA tag
        if (empty($this->attributes['DATA'])) {
            $this->debugLog("warning: SF tag type SF had no DATA parameter (TITLE,ENTITY,SUBMIT,RESET)");
            return;
        }

        // uppercase data tag
        $this->attributes['DATA'] = strtoupper($this->attributes['DATA']);

        // check for valid data
        if ( ($this->attributes['DATA'] != 'TITLE')&& 
             ($this->attributes['DATA'] != 'ENTITY')&&
             ($this->attributes['DATA'] != 'SUBMIT')&&
             ($this->attributes['DATA'] != 'HEADER')&&
             ($this->attributes['DATA'] != 'RESET') )
             {
            $this->debugLog("invalid SF data type ({$this->attributes['DATA']}). must be: (TITLE,ENTITY,SUBMIT,RESET)");
            return;
        }

        // if it's a TITLE or ENTITY, check for required VAR
        if ($this->attributes['DATA'] == 'TITLE' || $this->attributes['DATA'] == 'ENTITY') {
            if (empty($this->attributes['VAR'])) {
                $this->debugLog("SF tag with DATA of {$this->attributes['DATA']} did not specify a VAR attribute");
                return;
            }
        }

    }

}
  
?>
