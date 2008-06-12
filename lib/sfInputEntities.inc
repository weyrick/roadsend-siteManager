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
 * input entity base class                      
 * all input entities should extend this class 
 * NOTE -- setupEntity() is a child function that can exist, and will be run after setValue is through running in the 
 *         base class. setValue should not be overridden! (weyrick) 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class SM_inputTypeEntity extends SM_object {

    /** 
    * the filter list for this entity 
    * @var SM_inputTypeEntity
    */
    var $filterList = array(); 

    /**
    * the frobber list for this entity
    * @var SM_inputFrobber
    */
    var $frobberList = array();
    
    /** 
    * the current value of this entity 
    * @var mixed
    */
    var $value = NULL; 
    
    /** 
    * is this entity current valid? 
    * @var bool 
    */
    var $isValid = true;
    
    /** 
    * my variable name 
    * @var string
    */
    var $varName = '';  
       
    /** 
    * instructions on how a user can fix their form 
    * @var string 
    */
    var $fixMessage = '';  
        
    /** 
    * does this input entity REQUIRE javascript to work? 
    * @var bool
    */
    var $requiresJS = false; 
    
    /** 
    * are we allowed to use javascript in this input entity? 
    * @var bool
    */
    var $useJS = true;     
    
    /** 
     * javascript output. this is the stuff that needs to go at the top,
     * not the inline stuff
     * @var string
     */
    var $jsOutput = ''; 
     
    /**
     * main input entity output string     
     * @var string
     */
    var $output = '';

    /**
     * reference back to the form entity this input entity is contained in
     * @var SM_inputTypeEntity
     */
    var $parentFormEntity = NULL;

    /**
      * base constructor. setup class.this should NOT be overriden! 
      * use entityConfig() for class initialization
      * @param SM_inputTypeIntity $fE parent form entity reference
      */
    function SM_inputTypeEntity($fE) {
    
        // setup handlers
        $this->_smoConfigure();

        // parent form entity
        $this->parentFormEntity = $fE;
    
        // default directives
        $this->addDirective('javaScript','');
        $this->addDirective('extraAttributes','');

    }

    function isLegacyLayout() {
        return !($this->parentFormEntity->parentSmartForm->directive['legacyLayout'] === false);
    }
    
    /**
     * a method for retrieving common attributes for input entities     
     * @return string attribute list suitable for adding to an input entities tag
     */
    function getCommonAttr() {

        $attrs = '';

        if ($this->directive['javaScript'] != '') {
            $attrs .= ' '.$this->directive['javaScript'].' ';
        }

        if ($this->directive['extraAttributes'] != '') {
            $attrs .= ' '.$this->directive['extraAttributes'].' ';
        }

        if (!$this->isLegacyLayout()) {
            // new css forms: add id to input entities
            $id = ($this->varName{0} == '_') ? substr($this->varName,1) : $this->varName; // id's break when they start with a _
            $attrs .= ' id="'.$id.'" ';
        }
        elseif ($this->directive['entityAttributeClassTag'] != '') {
            $attrs .= ' class="'.$this->directive['entityAttributeClassTag'].'" ';
        }
        
        return $attrs;

    }

    /**
     * wrapper for adding javascript output to entity
     * @param string $output the javascript text to add
     */
    function sayJS($output) {
        $this->jsOutput .= $output;
    }

    /**
     * wrapper for adding output to entity output text
     * @param string $output the text to add
     */
    function say($output) {
        $this->output .= $output;
    }

    /** 
     * return javascript output from filters attaached to this entity,
     * as well as our own
     * @return string javascript output
     */
    function getJS() {
        
        // if we have no filters, just return our javascript code
        if (!isset($this->filterList)) {
            return $this->jsOutput;     
        }
            
        // get javascript from each entity      
        $fjsOutput = '';
        $fjsOutputL = '';
        foreach($this->filterList as $filter) {

            $fjsOutputL .= $filter->getJS();
            
            // save list for last use
            if ($fjsOutputL != '') {
                $fName = get_class($filter);                
                $fName = str_replace("filter","Filter",$fName); // capitlize Filter, as the class won't for us
                $fjsList[] = $fName;
            }
            
            $fjsOutput .= $fjsOutputL;
        
        }

        // if we had javascript output, we need to setup a function to run
        // onSubmit for this entity, to run client side filter functions
        if ($fjsOutput != '') {
        
            $this->jsOutput .= "function check_{$this->varName}() {\n\n";
            
            
            // for each filter that has javascript, fun it's code
            $formID = $this->parentFormEntity->parentSmartForm->formID;
            foreach ($fjsList as $filt) {
                $this->jsOutput .= "\tif (!".$filt."_JS(document.SF{$formID}.".$this->varName.".value)) return false;\n";
            }   
            
            $this->jsOutput .= "\treturn true;\n";
            $this->jsOutput .= "}\n\n";
        
        }

        // return javascript for entites and filters only once
        return $fjsOutput.$this->jsOutput;
    
    }

     /**
      * set this entities variable name, and possibly it's originating value
      * after setting some values, it will run a function called setupEntity() if it's
      * part of this class. child classes should use it to do extra setting up at this point
      * THIS FUNCTION SHOULD NOT BE OVERRIDEN
      * @param string $varName the variable we're working with
      * @param string $value initial value
      * @param bool $uJS use javaScript or not
      */
    function setValue($varName, $value='', $uJS) {

        $this->varName      = $varName;
        $this->value        = $value;
        $this->useJS        = $uJS;
                        
        // run entityConfig method if it exists
        if (method_exists($this, 'entityConfig')) {
            $this->entityConfig();
        }
        
    }   

     /**
      * change a set default value for an input entity    
      * @param   $default the new value to display
      * @param bool $forceNotFTL when true, it will set default value even if we're not FTL
      */
    function setDefaultValue($default, $forceNotFTL=false) {
   
        // we only set defaults on FTL, unless force is on
        if ($this->parentFormEntity->parentSmartForm->ftl || $forceNotFTL) {

            // set the value
            $this->value = $default;
        
            // run eventNewValue method
            $this->eventNewValue();
            
            return true;
        }
        else {
            return false;
        }

    }

    /**
     * this method will be run if an entity had it's default value changed after
     * it's entityConfig() method has run. it allows the entity to do any necesary
     * processing on the new value. if needed, it should be overridden.
     */
    function eventNewValue() {
        // virtual
    }
    
     /**
      * add a filter to this input entity
      * @param string $fType filter type to add
      * @param string $msg message to output if filter fails
      * @param   $varName sometimes an entity will want to override the variable name (see dateEntity)
      */
    function addFilter($fType, $msg='', $varName='') {

        if ($varName == '')
            $varName = $this->varName;
        
        // verify and load this type
        if (!SM_sfLoadFilter($fType)) {
            $this->debugLog("add: bad/unknown filter passed (filter module not found): $fType");
            return NULL;
        }
        
        // good filter, add it
        // create the filter and add it to this entity
        $filterClass = $fType.'Filter';
        $newFilter = new $filterClass($this);
        $newFilter->setInfo($varName, $msg, $this->useJS);
        
        // save to this entity
        $this->filterList[$fType] = $newFilter;
        return $newFilter;
    }


     /**
      * set a filters arguments
      * @param   $fType the filter to edit 
      * @param   $args a hash of key/value arguments
      */
    function setFilterArgs($fType, $args) {
    
        if (!is_array($args)) {
            $this->debugLog("setFilterArgs: args variable was not an array");
            return false;
        }
        
        if (!isset($this->filterList[$fType])) {
            $this->debugLog("setFilterArgs: filter type [$fType] not found for this entity");
            return false;
        }
        else {
            $this->filterList[$fType]->configure($args);
        }
        
        return true;
    
    }

     /**
      * remove a filter
      * @param   $fType filter type to remove
      * @return bool false if not found
      */
    function removeFilter($fType) {
    
        if (isset($this->filterList[$fType])) {
            // found, remove 
            unset($this->filterList[$fType]);
            return true;
        }
        else {
            // not found
            return false;
        }
    
    }
        
        
     /**
     * apply filters. set's whether this entity is valid based on filter checks.
     */
      
    function applyFilters() {

        // if filterList isn't an array, this entity has no filters 
        if (!isset($this->filterList)) {
            $this->isValid = true;
            $this->debugLog("no filters for this entity ($this->varName)");
            return;
        }
    
        // apply each filter on this input entity 
        foreach($this->filterList as $filter) {
            if (!$filter->filterThink()) {
                // data was NOT valid
                $this->isValid = false;
                // compile fix message
                $this->fixMessage .= $filter->directive['badMessage'].'<br />';
                // debug warn
                $this->debugLog($this->varName.': failed filter check');
                // if we've found one bad, quit now
                return;
            }
        }   
    
    }

     /**
     * output function for this entity. this should be overridden
     */
      
    function entityThink() {
    
        // virtual
    
    }

    public function addFrobber($f) {

        // XXX we do this by string for now to be consistent with e.g. filters and entites,
        // but we may switch to passing a new SM_frobber object directly
        $cName = 'SM_'.$f.'Frob';
        if (!class_exists($cName))
            $this->fatalErrorPage("unknown frobber: $f");
        $fH = new $cName();
        $this->frobberList[] = $fH;

    }

    public function applyFrobbers() {

        foreach ($this->frobberList as $fH) {
            $this->value = $fH->frobInput($this->value);
        }

    }
    
    public function getValue() {

        return $this->value;

    }
    
}


/**
 * load (include) an input entity module
 * if it's already loaded, ignore request
 * @param string $iName the input entity to load
 * @returns bool if false, couldn't find it. true means successful load
 */
function SM_sfLoadEntity($iName) {
    
    // if already here, don't reload    
    if (class_exists($iName.'Entity'))
        return true;

    // globalize
    global $SM_siteManager;

    // find file
    $iName = $SM_siteManager->findSMfile($iName.'Entity', 'sfEntities', 'inc', true);

    // load it up
    // include_once will not let duplicate modules be loaded
    include_once($iName);   
    
    // all good and loaded, return
    return true;

}

?>