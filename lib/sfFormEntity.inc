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
 * form entity class. a form entity contains a title and and input entity,
 * and the input entity optionall contains filters.
 * 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class SM_formEntity extends SM_object {

    /** 
    * the title of this entity, which will be displayed to the user 
    * @var string
    */
    var $title = '';

    /** 
    * the variable name we'll use, which will be the POST variable 
    * @var string
    */
    var $varName = '';

    /** 
    * the type of input form this is 
    * @var string
    */
    var $type = '';

    /**
    * the variables current value 
    * @var mixed 
    */
    var $value = NULL;

    /**
    * object of the actual entity 
    * @var SM_formEntity
    */
    var $inputEntity = NULL;

    /** 
    * is this entity required to complete the form successfully?
    * @var bool  
    */
    var $required = false;

    /** 
    * entity layout
    * @var const int 
    */
    var $layout = SF_LAYOUT_UNDEFINED;

    /** 
    * alignment of the title. CENTER, RIGHT, or LEFT 
    * @var string 
    */
    var $titleAlign = 'LEFT';

    /** 
    * alignment of the input, CENTER, RIGHT, or LEFT 
    * @var string
    */
    var $inputAlign = 'LEFT';

    /** 
    * show background color or not 
    * @var bool
    */
    var $showBgColor = true;
        
    /**
    * use javascript for this input entity 
    * @var bool
    */
    var $useJS = true;
    
    /** 
    * parent smartform 
    * @var SM_formEntity
    */
    var $parentSmartForm = NULL;

    /** 
    * if true, the filters will always be run for this input entity 
    * @var bool
    */
    var $forceFilter = false;

    /** 
    * when alternating color rows is in effect, this determines which row number we are 
    * @var int
    */
    var $rowAltNum = 0;

     /**
      * initialize a formEntity
      * @param string $vn the variable name
      * @param string $tt the variable title
      * @param string $ty input entity type
      * @param bool $req required or not. if true, adds required filter
      * @param   $parentSmartForm the main smartForm parent
      * @param int $lay layout
      * @param mixed $val initial value
      * @param string $tA title alignment
      * @param string $iA input entity alignment
      * @param bool $uJS use javascript or not for this form entity
      */
    function SM_formEntity($vn, $tt, $ty, $req, $parentSmartForm, $lay=0, $val='', $tA="LEFT", $iA="LEFT", $uJS=true) {

        // setup handlers
        $this->_smoConfigure();

        // parent smartForm
        $this->parentSmartForm = $parentSmartForm;

        // configure self
        $this->configure($this->parentSmartForm->directive);

        // setup some locals
        $this->varName      = $vn;
        $this->title        = $tt;
        $this->type         = $ty;
        $this->required     = $req;
        $this->value        = $val; 
        $this->layout       = $lay;
        $this->titleAlign   = $tA;
        $this->inputAlign   = $iA;
        $this->useJS        = $uJS;
    
        // configure row color
        $this->rowAltNum    = 1;
        if ($this->directive['normalClassTagAlt1'] == '')
            $this->directive['normalClassTagAlt1'] = $this->directive['normalClassTag'];
        if ($this->directive['normalClassTagAlt2'] == '')
            $this->directive['normalClassTagAlt2'] = $this->directive['normalClassTag'];

        // load entity
        $this->inputEntity = $this->loadEntity($ty, $req);

    }


    /**
     * function to load and setup an instance of an entity     
     * @param string $eClass the entity class to load
     * @param bool $req whether this entity is required or not
     * @param string $vName override variable name to something besides the one for this form entity
     *
     */
    function loadEntity($eClass, $req=false, $vName='') {

        if ($vName == '')
            $vName = $this->varName;

        // create the input entity. it should be valid since smartForm does the checking
        $entityClass = $eClass."Entity";
        $newEntity = new $entityClass($this);

        // if $val isn't set, but $$vn is, use that
        if (isset($_POST[$vName])) {
            if (!is_array($_POST[$vName]))
                $val = stripslashes($_POST[$vName]);      // prevent recuring slashes
            else 
                $val = '';
            $newEntity->setValue($vName, $val, $this->useJS);
        }
        else {
            $newEntity->setValue($vName, $this->value, $this->useJS);
        }

        // setup default class tag for the entities use
        $newEntity->addDirective('entityClassTag',$this->directive['entityClassTag']);
        $newEntity->addDirective('entityAttributeClassTag',$this->directive['entityAttributeClassTag']);

        // if this entity is required input, automatically add a requiredFilter
        if ($req) {
            $newEntity->addFilter('required', $this->parentSmartForm->getDirective('requiredBadMsg'));
        }

        return $newEntity;

    }

    
     /**
      * return output from our input entity
      * @return string output from entity
      */
    function output() {
        
        (isset($this->directive['entityPrefixWrap'])) ?
            $_prefix = $this->directive['entityPrefixWrap'] :
            $_prefix = '';

        (isset($this->directive['entityPostfixWrap'])) ?
            $_postfix = $this->directive['entityPostfixWrap'] :
            $_postfix = '';

        $this->inputEntity->entityThink();
        $eOP = $this->inputEntity->output;

        return $_prefix.$eOP.$_postfix; 
    }
    
    /** 
     * return any javascript code this entity and filter contains
     * @return string javascript output from entity and filters
     */
    function getJS() {
        return $this->inputEntity->getJS();
    }   
  
     /**
      * apply filters that are attatched to this entity
      * don't apply them, however, if there was no data entered,
      * and it wasn't required
      */
    function applyFilters() {

        $vName = $this->varName;
        $data = $this->inVarH->getPOST($vName);

        // only run filter if this field is required OR data isn't blank
        if ((($this->required)||($data != ''))||($this->forceFilter))
            return $this->inputEntity->applyFilters();
    }
        
     /**
      * get title from entity. if the inputEntity tells us the current value is bad, we'll mark it
      * as red. also, if its invalid, we should have a fix
      * message which we'll place underneith the title
      * @return string title from entity, formatted with class tags and such
      */
    function getTitle() {

        // if the title is blank, return blank
        if ($this->title == '')
            return '';
        
        if (($this->required)&&($this->directive['showRequiredHelp']))
            $prefix = $this->directive['requiredStar'].' ';
        else
            $prefix = '';
        
        $title = '';

        $titleWrapTag = $this->directive['titleWrapTag'];
        if (empty($titleWrapTag)) {
            if ($this->inputEntity->isValid){
                if ($this->parentSmartForm->directive['legacyLayout']) {
                    // legacy style
                    $title .= $prefix.$this->title;
                }
                else {
                    // new style: label tag
                    $title .= $prefix.'<label for="'.$this->varName.'">'.$this->title.'</label>';
                }
            }
            else {
                if ($this->parentSmartForm->directive['legacyLayout']) {
                    $title .= $prefix.$this->title.'<br/>'.$this->inputEntity->fixMessage;
                }
                else {
                    // new style
                    $title .= $prefix.'<label for="'.$this->varName.'">'.$this->title.'</label>'.$this->inputEntity->fixMessage;
                }
            }            
        }
        else {
            if ($this->inputEntity->isValid){        
                $ct = 'normalClassTagAlt'.$this->rowAltNum;
                $title .= "<$titleWrapTag class=\"".$this->directive[$ct]."\">$prefix".$this->title."</$titleWrapTag>";
            }
            else {
                $title .= "<$titleWrapTag class=\"{$this->directive['badClassTag']}\">{$prefix}{$this->title}<$titleWrapTag class=\"{$this->directive['correctMsgClassTag']}\">{$this->inputEntity->fixMessage}</$titleWrapTag></$titleWrapTag>";
            }
        }
        

        return $title;
    
    }
    
     /**
      * return the layout order of this entity
      * @return int current layout
      */      
    function getLayout() {
        return $this->layout;
    }
    
     /**
      * return the title alignment
      * @return string title alignment
      */      
    function getTitleAlign() {
        return $this->titleAlign;
    }
    
                                 
     /**
      * return the input alignment
      * @return string input entity alignment
      */      
    function getInputAlign() {
        return $this->inputAlign;
    }
    
    /**
     * show bgColor or not for this form entity
     * @param bool $val true or false
     */
    function setShowBgColor($val) {
        $this->showBgColor = $val;
    }
         
    /**
     * return whether we want a background color or not
     * @return bool whether we should show background color or not
     */
    function getShowBgColor() {
        return $this->showBgColor;
    }
     
    /**
     * return the type of input entity we maintain
     * @return string input entity type
     */
    function getType() {
        return $this->type;
    }

    /**
     * set a new title
     *
     */
    function setTitle($newTitle) {
        $this->title = $newTitle;
    }

    /**
     * turn this entity into presetText entity
     */
    function setReadOnly($defaultValue=NULL) {

        // possibly save old default value
        if ($defaultValue == NULL) {
            $defaultValue = $this->inputEntity->value;
        }

        // reset
        $this->required = false;

        // load entity
        if (!SM_sfLoadEntity('presetText')) {
            $this->fatalErrorPage("Unable to set entity read-only: presetText entity not found");
        }
        $this->inputEntity = $this->loadEntity('presetText', false);
        $this->inputEntity->value = $defaultValue;

        return true;

    }

}

?>
