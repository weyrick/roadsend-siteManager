<?php

/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001-2009 Roadsend, Inc.(http://www.roadsend.com)
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
    var $layout = NULL;

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
      * @param SM_smartForm $parentSmartForm the main smartForm parent
      * @param int $lay layout
      * @param mixed $val initial value
      */
    function SM_formEntity($vn, $tt, $ty, $req, $parentSmartForm, $lay=NULL, $val='') {

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

        if (isset($_POST[$vName])) {
            if (!is_array($_POST[$vName]))
                $val = stripslashes($_POST[$vName]);      // prevent recuring slashes
            else 
                $val = '';
            $newEntity->setValue($vName, $val);
        }
        else {
            $newEntity->setValue($vName, $this->value);
        }

        // setup default class tag for the entities use
        $newEntity->addDirective('entityClassTag',$this->directive['entityClassTag']);

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
                
        $title = '';

        $titleWrapTag = $this->directive['titleWrapTag'];
        if (empty($titleWrapTag)) {
            if ($this->inputEntity->isValid){
                $title .= '<label for="'.$this->varName.'">'.$this->title.'</label>';
            }
            else {
                $title .= '<label for="'.$this->varName.'">'.$this->title.'</label>';
                if ($this->directive['correctMsgClassTag']) {
                    $title .= "<span class=\"{$this->directive['correctMsgClassTag']}\">".
                               $this->inputEntity->fixMessage.
                               "</span>";
                }
                else {
                    $title .= $this->inputEntity->fixMessage;
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
