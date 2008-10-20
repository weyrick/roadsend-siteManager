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

/** title on left, input on right */
define("SF_LAYOUT_NORMAL",     1);
/** title on right, input on left */
define("SF_LAYOUT_FLIPPED",    2);
/** "title <br> input" in one cell */
define("SF_LAYOUT_SINGLE",     3);
/** free form layout - no table tags will wrap around a form entity */
define("SF_LAYOUT_FREEFORM",   4);
/** "titleinput" in one cell */
define("SF_LAYOUT_SINGLENOBR", 5);
/** undefined layout */
define("SF_LAYOUT_UNDEFINED",  100);   

global $SM_siteManager;

// input frobbers
$SM_siteManager->includeLib('sfInputFrob');

// include input entities
$SM_siteManager->includeLib('sfInputEntities');

// filters
$SM_siteManager->includeLib('sfFilters');

// form entities
$SM_siteManager->includeLib('sfFormEntity');


/**
 * SmartForms abstract HTML forms to code.
 * With SmartForms, you define a form by telling it exactly what you need and how
 * the information needs to be formatted. SmartForms handles all other aspects of the form.
 *
 * SmartForms should not be instantiated directly, but reather created in modules
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 * @see SM_module::newSmartForm
 */
class SM_smartForm extends SM_object {
    
    /** 
     * array of formEntities in this form, keyed by varName
     * @see SM_formEntity
     * @var array
     */
    var $entityList = NULL;

    /**
     * double hash of groups of form variables 
     * @var array 
     */
    var $groupList = array();

    /**
     * first time form load or not
     * @var bool  
     */
    var $ftl = true;

    /** 
     * raw text counter. used internally by {@link addText}
     * @var int  
     */
    var $rt = 0;

    /** 
     * final buffered output
     * @var string  
     */
    var $sfOutput = '';
    
    /** 
     * final buffered javascript output, collected from form, entities and filters
     * @var string  
     */
    var $jsOutput = '';
    
    /** 
     * a header to place at the top of the form table. set with {@link setHeader}
     * @var string  
     */ 
    var $header = '';
    
    /** 
     * a footer to place at the bottom of the form table. set with {@link setFooter}
     * @var string  
     */ 
    var $footer = '';
        
    /** 
     * the template we either built or loaded, that form entities will appear in
     * @var string
     */
    var $template = '';

    /** 
     * an expanded array of hashes used to mimic the data structure
     * when the XML file is read in
     * @var array
     */
    var $xEntity = array();
   
    /** 
     * a list of variables available from getVar, but that are not available through
     * entityList because they are inside of another entity
     * @var array
     */
    var $internalVarList = array();

    /**
     * a unique form ID     
     * @var string
     */
    var $formID = '';

    /** 
     * SmartForm reader, for creating forms from XML or database 
     * @see SM_configReader_SMARTFORM
     * @var SM_configReader_SMARTFORM
     */
    var $cFormReader = NULL;

    /**
     * Track if this form has run. This is set to true
     * at the end of the runForm function. 
     * @var boolean
     **/
     var $formHasRun = FALSE;
     
     /**
      * create new SmartForm using the ID passed, which is used in the form name. SmartForms
      * should NOT be instantiated directcly, see {@link SM_module::newSmartForm} 
      * @param string $formID unique (to this page) ID, used to make formName
      */
    function SM_smartForm($formID) {

        // setup session handlers
        $this->_smoConfigure();
        
        // setup defaults for output
        // setup default required directive
        $this->directive['tableBorder']         = 0;
        $this->directive['tableWidth']          = "100%";
        $this->directive['tableCellPadding']    = 0;
        $this->directive['tableCellSpacing']    = 0;
        $this->directive['tableBgColor']        = '';

        // visual "required" notification settings
        $this->directive['showRequiredHelp']    = true; // if this is false, neither Star nor Tag will show up
        $this->directive['requiredTag']         = true; // should we add a Required Field notice?
        $this->directive['requiredStar']        = '*';  // what symbol denotes a required field. may be IMG tag.
        $this->directive['requiredText']        = ' = Required Field'; // text shown to describe what requiredStar means
        $this->directive['requiredBadMsg']      = 'Required Field';  // text for badMsg on required filters

        // style sheet classes used in the display of the form
        // these should NOT include the CLASS= part. just the style info
        $this->directive['normalClassTag']      = 'sfNormal';   // class tag for normal entity title
        $this->directive['badClassTag']         = 'sfBad';      // class tag for "bad" entity title
        $this->directive['correctMsgClassTag']  = 'sfMessage';  // class tag for correct message on entity title
        $this->directive['requiredClassTag']    = 'sfNormal';   // class tag for required tag
        $this->directive['entityClassTag']      = '';           // default class tag for input entity text

        // css style
        $this->directive['legacyLayout']        = true;         // whether to use the latest and greatest CSS technology
        
        // if this is set to true, ALL entities will be presetText, regardless of what is
        // specified as the entity type in the add() call
        $this->directive['readOnly']            = false;

        // this is the CLASS= tag for the main FORM tag
        $this->directive['formClassTag']        = '';

        // sessions vars to be excluded from propagating in this form
        $this->directive['varExcludeList']      = NULL;

        // row color alternator. every other row will have these background color. if blank, it ignores it.
        $this->directive['rowColorAlt1']        = '';       
        $this->directive['rowColorAlt2']        = '';

        // SPAN tags that should be used when row color alternation is in effect
        $this->directive['normalClassTagAlt1']  = '';
        $this->directive['normalClassTagAlt2']  = '';

        // default input entity class tag, as an attribute to the input entities main tag
        $this->directive['entityAttributeClassTag'] = '';

        // allow choice of div or span wrapper on entity titles
        $this->directive['titleWrapTag']            = 'div';
        
        // reset button off by default
        $this->directive['resetButton']         = '';           // when blank, it wont show up. otherwise it should be
                                                                // the VALUE for the reset button

        // bad form message, displayed at top if
        // any filters failed
        $this->directive['badFormMessage']      = '';

        // submit button alignment
        $this->directive['submitAlign']         = 'left';

        // submit on right (or bottom)
        $this->directive['controlsOnRight']     = false;        // bottom by default, right otherwise

        // dump the smartform template to HTML source
        $this->directive['dumpTemplate']        = false;

        // if submit is on bottom, and reset is on bottom, submit is on the left
        // by default. to have submit on the right, and reset on the left, change this
        $this->directive['swapControlOrder']    = false;

        // free form seperator is used in SF_LAYOUT_FREEFORM
        $this->directive['freeFormSeparator']   = '<br />';
    

        // default layout of form entities
        $this->directive['defaultLayout']       = SF_LAYOUT_NORMAL;
    
        // use javascript or not
        $this->directive['useJS']               = true;         // when true, we'll output javascript code. if it's turned off
                                                                // we won't. note that some input entities and filters
                                                                // won't work without it.


        $this->directive['onSubEvent']      = '';           // Set a specific event for the form as a whole
                                                                
        // script to POST to. normally $PHP_SELF. if you change this, all validation functionality goes
        // out the window.
        $this->directive['postScript']          = $_SERVER['PHP_SELF'];
        
        // Set the encode type for a smartform
        $this->directive['enctype']             = '';
        
        // submit button image
        $this->directive['submitImage']         = '';

        // a flag used by SF tag to say that the end form tag has been written
        // so that we don't duplicate it
        $this->directive['formIsEnded']         = false;
        
        // header is blank by default
        $this->header                           = '';
        
        // footer is blank by default
        $this->footer                           = '';
            
        // form ID
        $this->formID                           = $formID;

        // the FORM NAME
        $this->directive['formName']            = $this->formID;

        // global frob list. if non empty, add each frobber to each entity
        $this->directive['globalFrobList']      = NULL;
        
        // setup FTL
        $myFTL = $this->formID.'_ftl';
        $ftlVal = $this->inVarH->getPOST($myFTL);
                        
        // is this a first time load?
        $this->ftl = !isset($ftlVal);
            
    }   

    function addDirective($key, $val) {
        parent::addDirective($key, $val);
        // CSS compatibility
        if (($key == 'legacyLayout') && ($val == false)) {
            // new style layout. make sure some settings are correct for this style
            $this->directive['titleWrapTag'] = NULL;
        }
    }
    
     /**
      * add a block of text in the input form
      * @param string $text the text to add. may contain HTML
      * @param string $align the alignment in the form table. LEFT, RIGHT or CENTER
      * @param constant $lay layout style (see layout defines)
      */
    function addText($text, $align="CENTER", $lay=SF_LAYOUT_UNDEFINED) {

        $this->rt++;
        $rtName = 'rawText_'.$this->rt;
        
        // setup default layout
        if ($lay == SF_LAYOUT_UNDEFINED)
            $lay = $this->directive['defaultLayout'];
        
        // if it's not FREEFORM, make it single
        if ($lay != SF_LAYOUT_FREEFORM)
            $lay = SF_LAYOUT_SINGLE;
        
        $rt = $this->add($rtName,'','rawText',false,'',NULL,$lay,$align,$align);
        $rt->addDirective('staticText', $text);

    }

     /**
      * setup a header to be displayed at the top of the form table
      * note: not used when the SmartForm uses a user-defined template
      * @param string $hText the text/html to be displayed
      */
    function setHeader($hText) {
        $this->header = $hText;
    }
    

     /**
      * setup a footer to be displayed at the bottom of the form table
      * note: not used when the SmartForm uses a user-defined template
      * @param string $fText the text/html to be displayed
      */
    function setFooter($fText) {
        $this->footer = $fText;
    }


    /**
     * override debugLog to show our form ID
     *
     */
    function debugLog($msg, $verbosity=0) {
        parent::debugLog($this->formID.': '.$msg, $verbosity);
    }
    
    /**
     * use a SiteManager template to format the output of the SmartForm
     * @param string $templateFile the template file to load. will search template file path.
     * @return SM_layoutTemplate the newly loaded template
     */
    function loadTemplate($templateFile) {

        global $SM_siteManager;

        // attempt to load the template file
        $this->template = $SM_siteManager->loadTemplate($templateFile);
        $this->template->addDirective('SF', array('parentForm' => $this));
        return $this->template;

    }

    /** 
     * return the current SM_layoutTemplate to be used for SmartForm output
     * @return SM_layoutTemplate current layout template     
     */
    function getTemplate() {
        return $this->template;
    }

    /**
     * tell the smartform it should use the given SM_layoutTemplate template
     * for layout
     * @param SM_layoutTemplate $templateObj the template the form should use for output
     */
    function useTemplate($templateObj) {
        $this->template = $templateObj;
        $this->template->addDirective('SF', array('parentForm' => $this));
    }

     /**
      * setup an HTML template to use for the form output
      * if this is not set, a template will be generated on the fly
      * @deprecated templates now use the main {@link SM_layoutTemplate} engine. this is provided for
      *             backward compatibility reasons.
      * @param string $fName the file to use as template (full path)
      */
    function setTemplate($fName) {

        global $SM_siteManager;

        // now find and load form. failing to find this WILL be fatal
        $fName = $SM_siteManager->findSMfile($fName, 'smartForms', 'stp', true);

        // open file, check input path too      
        $fp = fopen($fName, 'r', 1);
        
        if (!$fp) {
            $this->debugLog("setTemplate(): file not found $fName");
            return false;
        }
        
        // read in template
        while (!feof($fp)) {
            $this->template .= fgets($fp, 4096);            
        }
        
        // close template
        fclose($fp);
        
    }
    
   /**
    * add a new entity to the form
    *
    * @param string $varName the variable name in the form
    * @param string $title the text description to use as the title of the input entity
    * @param string $type the input type
    * @param bool $req required input or not
    * @param mixed $val initial value
    * @param array $args arguments to pass along to configure the input entity
    * @param const $lay layout of this entity.
    * @param string $tA title alignment
    * @param string $iA input entity alignment
    * @return SM_inputEntity input entity created
    */
    function add($varName, $title, $type, $req=false, $val='', $args=NULL, $lay=SF_LAYOUT_UNDEFINED, $tA='LEFT', $iA='LEFT') {
                                   
        // don't allow the same variable twice
        if (isset($this->entityList[$varName])) {
            $this->debugLog("add: variable name $varName was already defined. ignoreing.");
            return NULL;
        }

        // if this is a read only form, change type to 'staticText' entity
        if ($this->directive['readOnly'])
            $type = 'presetText';

        // verify and load this type
        if (!SM_sfLoadEntity($type)) {
            $this->debugLog("add: bad/unknown type passed (input entity module not found): $type");
            return NULL;
        }

        // setup default layout
        if ($lay == SF_LAYOUT_UNDEFINED)
            $lay = $this->directive['defaultLayout'];
                
        // good entity, add it in
        $this->entityList[$varName] = new SM_formEntity($varName, $title, $type, $req, $this, $lay, $val, $tA, $iA, $this->directive['useJS']);

        // if they passed args, do them here
        if (is_array($args))
            $this->entityList[$varName]->inputEntity->configure($args);

        // global frobbers?
        if (!empty($this->directive['globalFrobList'])) {
            foreach ($this->directive['globalFrobList'] as $frobber) {
                $this->entityList[$varName]->inputEntity->addFrobber($frobber);
            }
        }
        
        // success, return reference to formEntity
        return $this->entityList[$varName]->inputEntity;

    }

    /**
     * add a hidden variable to the form
     * @param string $name the name of the variable to add
     * @param string $value value of variable
     */
    function addHidden($name, $value) {
        $this->directive['hiddens'][$name] = $value;
    }

     /**
      * remove an entity from the form that has previously been added
      * @param string $varName the variable to remove
      */
    function remove($varName) {

        if (isset($this->entityList[$varName])) {
            unset($this->entityList[$varName]);
            return true;
        }
        else {
            $this->debugLog("remove: variable not found for remove: $varName");
            return false;
        }
    
    }

    /**
     * add a variable to the 'internal' var list. this is a variable that should be returned in getVarList()
     * that wasn't actually an entry in the entityList hash. this happens (for example) when input entities 
     * have more than one variable name, or when you want to include a hidden in the getVarList() hash
     * @param string $varName the variable name to add
     */
    function addInternalVar($varName) {
        if (!in_array($varName, $this->internalVarList)) {
            $this->internalVarList[] = $varName;
        }
    }

     /**
      * add a filter to an entity. the form will only pass the {@link dataVerified} check
      * when all filters in the form pass successfully.
      * @param string $varName the variable to add the filter to
      * @param string $fType the filter type to add
      * @param string $bMsg the message to show if the filter doesn't pass
      * @param array $args optional arguments to set to configure the filter
      * @return SM_entityFilter newly created filter, or NULL on error
      */
    function addFilter($varName, $fType, $bMsg='', $args=NULL) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("addFilter: variable not found for filter add: $varName");
            return NULL;
        }
        else {
            if (($newFilter = $this->entityList[$varName]->inputEntity->addFilter($fType, $bMsg)) == NULL) {
                $this->debugLog("addFilter: error adding filter type $fType to variable $varName");
                return NULL;
            }
            // if we have arguments, set them now
            if (is_array($args))
                $newFilter->configure($args);

            return $newFilter;
        }       
    
    }


     /**
      * remove a filter from an entity that has previously been added
      * @param string $varName the variable name to change
      * @param string $fType the filter to remove
      */
    function removeFilter($varName, $fType) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("removeFilter: variable not found for filter remove: $varName");
            return false;
        }
        else {
            if (!$this->entityList[$varName]->inputEntity->removeFilter($fType)) {
                $this->debugLog("removeFilter: filter type $fType not found for variable $varName");
                return false;
            }
            else {
                // if this is a required filter, remove from form entity
                if ($fType == 'required') {
                    $this->entityList[$varName]->required = false;
                }
                return true;
            }
        }

    }

     /**
      * set arguments (configuration directives) for a particular input entity
      * @deprecated use {@link getEntity} and then {@link SM_object::configure}
      * @param string $varName variable to configure
      * @param array $args key/value directive list
      * @return bool true on success, false otherwise
      */
    function setArgs($varName, $args) {
        
        if (!is_array($args)) {
            $this->debugLog("setFilterArgs: args variable was not an array");
            return false;
        }
            
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setArgs: variable not found for filter add: $varName");
            return false;
        }
        else {
            $this->entityList[$varName]->inputEntity->configure($args);
        }

    }

     /**
      * set a filters arguments (directives)
      * @param string $varName the variable to edit
      * @param string $fType the filter to edit 
      * @param array $args an array of key/value arguments
      * @return bool true on success, false on error
      */
    function setFilterArgs($varName, $fType, $args) {
    
        if (!is_array($args)) {
            $this->debugLog("setFilterArgs: args variable was not an array");
            return false;
        }
        
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setFilterArgs: variable not found for argument set: $varName");
            return false;
        }

        if (!isset($this->entityList[$varName]->inputEntity->filterList[$fType])) {
            $this->debugLog("setFilterArgs: filter type [$fType] not found for variable [$varName]");
            return false;
        }

        // configure
        $this->entityList[$varName]->inputEntity->filterList[$fType]->configure($args);        
        return true;
    
    }
     /**
      * Add some text before or after the entity input.
      * @param string $varName the variable to set
      * @param string $prefixWrap text before the entity
      * @param string $postfixWrap text after the enity input
      * @return bool true on success, false on error
      */
    function setEntityTextWrap($varName, $prefixWrap='',$postfixWrap='') {
    
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setEntityTextWrap: variable not found for argument set: $varName");
            return false;
        }
        
        $this->entityList[$varName]->addDirective('entityPrefixWrap',$prefixWrap);
        $this->entityList[$varName]->addDirective('entityPostfixWrap',$postfixWrap);
        return true;
    }

    /**
     * set a new title for a form entity
     * @param string $varName the variable to set
     * @param string $newTitle new title for this entity
     */
    function setEntityTitle($varName, $newTitle) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setEntityTitle: variable not found: $varName");
            return false;
        }
        
        $this->entityList[$varName]->setTitle($newTitle);

    }

     /**
      * set the layout style for an input entity      
      * see the layout defines
      *
      * @param string $varName the variable to set
      * @param constant $newLayout the new layout to use. 
      * @return bool true on success, false on error
      */
    function setLayout($varName, $newLayout) {
    
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setLayout: variable not found for argument set: $varName");
            return false;
        }
        
        $this->entityList[$varName]->layout = $newLayout;
        return true;
    
    }

    
    /**
     * turn an entity that's already been added into the form, into read only (presetText)
     * @param string $varName the variable to set
     * @param mixed $defaultValue the text the variable should display
     * @return bool true on success, false on error
     */
    function setReadOnly($varName, $defaultValue=NULL) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setLayout: variable not found for argument set: $varName");
            return false;
        }

        $this->entityList[$varName]->setReadOnly($defaultValue);
        return true;
        
    }

     /**
      * set the "showBgColor" flag to make this form element
      * @param string $varName the variable to set
      * @param bool $val value to use (bool)
      * @return bool true on success, false on error
      */
    function setShowBgColor($varName, $val) {
        
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setShowBgColor: variable not found for argument set: $varName");
            return false;
        }
        
        $this->entityList[$varName]->setShowBgColor($val);
        return true;
    }
    
     /**
      * set the alignment of the input for an input entity
      * @param string $varName the variable to set
      * @param string $newAlign the new alignment to use. LEFT, RIGHT, or CENTER
      * @return bool true on success, false on error      
      */
    function setInputAlign($varName, $newAlign) {
    
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setInputAlign: variable not found for argument set: $varName");
            return false;
        }
        
        $this->entityList[$varName]->inputAlign = $newAlign;
        return true;
    
    }
    
    
     /**
      * set the alignment of the title for an input entity    
      * @param string $varName the variable to set
      * @param string $newAlign the new alignment to use. LEFT, RIGHT, or CENTER
      * @return bool true on success, false on error      
      */
    function setTitleAlign($varName, $newAlign) {
    
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("settitleAlign: variable not found for argument set: $varName");
            return false;
        }
        
        $this->entityList[$varName]->titleAlign = $newAlign;
        return true;
    
    }

     /**
      * change a set default value for an input entity    
      * @param string $varName the variable to set
      * @param mixed $default the new value to display
      * @param bool $forceNotFTL when true, it will set default value even if we're not FTL
      * @return bool true on success, false on error      
      */
    function setDefaultValue($varName, $default, $forceNotFTL=false) {
   
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setDefaultValue: variable not found for argument set: $varName");
            return false;
        }

        // we only set defaults on FTL, unless force is on
        if ($this->ftl || $forceNotFTL) {
            // we set force to true on the inputEntity setDefaultValue, to avoid another ftl check
            $this->entityList[$varName]->inputEntity->setDefaultvalue($default,true);
            return true;
        }
        else {
            return false;
        }
    
    }

    /**
     * set default values for all matching entities in varHash
     * @param array $varHash key/value pairs of entityName/default values
     * @param bool $forceNotFTL when true, it will set default value even if we're not FTL
     */
    function setDefaults($varHash, $forceNotFTL=false) {
        foreach ($varHash as $k => $v) {
            if (isset($this->entityList[$k])) {
                $this->setDefaultValue($k, $v, $forceNotFTL);
            }
        }
    }

    /**
     * specify that a particular input entity is part of a certain group. groups are
     * used to collect entities together and give them a common background color.
     * @param string $varName the variable name to set
     * @param string $groupName the group name to set it to
     * @return bool true on success, false on error     
     */
    function setGroup($varName, $groupName) {
                
        if (!isset($this->entityList[$varName])) {
            $this->debugLog($this, "setGroup: variable not found for group set: $varName");
            return false;
        }
        
        $this->groupList[$varName] = $groupName;
        return true;
    
    }

    /**
     * specify that a particular input entity is part of a certain group
     * @param string $groupName the group name to set it to
     * @param array $varList an array of variable names to put in this group
     * @return bool true on success, false on error     
     */
    function setGroupList($groupName, $varList) {

        if (!is_array($varList)) {      
            $this->debugLog("setGroupList: parameter \$varList is not array of variable names");
            return false;
        }
        
        foreach ($varList as $varName) {
                
            if (!isset($this->entityList[$varName])) {
                $this->debugLog("setGroupList: variable not found for group set: $varName");
                return false;
            }
            
            $this->groupList[$varName] = $groupName;
            
        }

        return true;
            
    }
    
     /**
      * commit an entity to a variable. workaround for not being able to return
      * a reference to our object in add() function
      * @deprecated should not be used, since a reference is returned from {@link add}, or
      *             you can retrieve a reference to an entity through {@link getEntity}
      * @param string $varName the variable to edit
      * @param SM_inputEntity $ent a reference to the entity to commit to the form
      * @return bool true on success, false on error
      */
    function commit($varName, $ent) {
        
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("commit: variable not found for entity commit: $varName");
            return false;
        }
        if (get_class($this->entityList[$varName]->inputEntity) != get_class($ent)) {
            $this->debugLog("commit: entity passed to commit (".get_class($ent).") is not of the same type as $varName (".get_class($this->entityList[$varName]->inputEntity).")! changing types implicitly.");
            unset($this->entityList[$varName]->inputEntity);
        }
        $this->entityList[$varName]->inputEntity = $ent;
        return true;
        
    }


    /** 
     * wrapper function for runForm(). identical in every way
     * only for consistency.
     */
    function run() { $this->runForm(); }

     /**
      * called to apply filters and "execute" the form
      */
    function runForm() {

        if ($this->formHasRun)
            return;
        
        // blank form?      
        if (empty($this->entityList)) {
            $this->debugLog('form was blank!');
            return;
        }
            
        foreach ($this->entityList as $ent) {
        
            // apply filters/frobbers if this isn't a ftl (ie, dont do server side if this is a first time load)
            if (!$this->ftl) {
                $ent->applyFilters();
                $ent->inputEntity->applyFrobbers();
            }
                                   
        }

        // Mark that this form has run
        $this->formHasRun = TRUE;

    }

    
    /**
     * after entities has run entityThink (after buildTemplate), and before form outputs, gather
     * entity javascript
     */
    function buildEntityJS() {

        // check all entities for javascript
        foreach ($this->entityList as $ent) {
                        
            // get javascript code if we've got that turned on
            if ($this->directive['useJS'] == true) {
                $jso = $ent->getJS(); 
                if ($jso != '') {
                    $entJSlist[] = $ent->varName;
                }
                $this->jsOutput .= $jso;
            }
                        
        }        
            
        // finally, if we have entities with javascript, we need to run a function to run
        // them all onSubmit
        if (($this->directive['useJS'] == true)&&(!empty($entJSlist))&&(is_array($entJSlist))) {
            $this->jsOutput .= "\n\nfunction check_{$this->formID}_data() {\n\n";
            
                foreach ($entJSlist as $entName) {
                    $this->jsOutput .= "\tif (!check_".$entName."()) return false;\n";
                }
            
            $this->jsOutput .= "\treturn true;\n";
            $this->jsOutput .= "}\n\n";
        }
        
    }

     /**
      * this function should be called AFTER setting up the entire form. it will return TRUE
      * if all the input entities have passed all of their filters, letting the calling script
      * know that all the data they need has been entered and verified as correct according
      * to the filters applied
      * 
      * @return bool true if all data is "verified" and ready for submission, false otherwise
      */
    function dataVerified() {

        // if this is a first time load, simply return false
        // this is also important if there are multiple forms on the same page, as it will
        // avoid us trying to dataVerify on another forms data, since the FTL hidden var name
        // is based on our smartform id, which is based on the module we are in.
        if ($this->ftl)
            return false;   
    
        //check to see if the form has run. if not, run it
        if(!$this->formHasRun)
            $this->runForm();
        
        // loop through each entity and ask if it's valid or not
        foreach ($this->entityList as $ent) {

            // skip text entities
            if ($ent instanceof rawTextEntity)
                continue;
        
            if (!$ent->inputEntity->isValid) {
                $this->debugLog("entity [{$ent->varName}] failed filter check");
                return false;
            }
        
        }
        
        return true;
    
    }

    /**
     * run the template, doing substitutions for form entities
     * @deprecated in >= 2.3.x, this is no longer used, since templates are now 
     *             standard SM_layoutTemplate objects
     */
    function runTemplate() {

        // BACKWARD COMPATIBILITY
        // FIXME: this is deprecated, will be removed at some point... 
        if (is_string($this->template)) {

            // loop through each entity, do subs
            foreach ($this->entityList as $ent) {
                
                // get info from entity objects for display
                $vName  = $ent->varName;
                (preg_match("/^(.+)\_\w+$/",$vName,$m)) ? $vpName = $m[1] : $vpName = $vName;
                $title  = $ent->getTitle();
                $input  = $ent->output();
                
                // try to substitute in the template
                $tSubVar  = '{'."$vName.title".'}';
                $tpSubVar = '{'."$vpName.title".'}';          
                $iSubVar  = '{'."$vName.input".'}';
                $ipSubVar = '{'."$vpName.input".'}';
                
                $this->template = str_replace($tSubVar, $title, $this->template);
                $this->template = str_replace($tpSubVar, $title, $this->template);
                $this->template = str_replace($iSubVar, $input, $this->template);
                $this->template = str_replace($ipSubVar, $input, $this->template);
                
            }
            
            // sub in submit and reset buttons
            $submitPattern = "/\{submit.(.+)\}/U";
            if (preg_match($submitPattern,$this->template,$m)) {
                SM_sfLoadEntity('submit');
                $submitButton = new SM_formEntity('_submitBut'.'_'.$this->formID, '', 'submit', false, $this);
                $submitButton->inputEntity->addDirective('value',$m[1]);
                if (!empty($this->directive['submitImage'])) {            
                    $submitButton->inputEntity->addDirective('image',true);
                    $submitButton->inputEntity->addDirective('src',$this->directive['submitImage']);
                }
                $submitVal = $submitButton->output();
                $this->template = preg_replace($submitPattern, $submitVal, $this->template);
            }
    
            $resetPattern = "/\{reset.(.+)\}/U";
            if (preg_match($resetPattern,$this->template,$m)) {
                SM_sfLoadEntity('reset');
                $resetButton = new SM_formEntity('_resetBut'.'_'.$this->formID, '', 'reset', false, $this);
                $resetButton->inputEntity->addDirective('value',$m[1]);
                if (!empty($this->directive['resetImage'])) {            
                    $resetButton->inputEntity->addDirective('image',true);
                    $resetButton->inputEntity->addDirective('src',$this->directive['resetImage']);
                }
                $resetVal = $resetButton->output();
                $this->template = preg_replace($resetPattern, $resetVal, $this->template);
            }

            return $this->template;

        }
        elseif (is_object($this->template)) {

            // SM_layoutTemplate method
            return $this->template->run();

        }

        // fall through, unknown template
        $this->debugLog("unknown template type (non-string or SM_layoutTemplate");
        return '';

    }

    /**
     * build a valid SM_layoutTemplate template, to output the form
     * @param string $submit text of the default submit button. if not specified, no default
     *                       submit button is added.
     */
     function buildTemplate($submit='') {
     
        // if $submit is set, convert html entities to allow >, etc
        if ($submit != '') {
            $submit = htmlspecialchars($submit);
        }
        
        $template = "<table width=\"{$this->directive['tableWidth']}\" border=\"{$this->directive['tableBorder']}\" cellspacing=\"{$this->directive['tableCellSpacing']}\" cellpadding=\"{$this->directive['tableCellPadding']}\" ";
        if ($this->directive['tableBgColor'] != '')
            $template .= " bgcolor=\"{$this->directive['tableBgColor']}\" ";
        $template .= ">";
        
        // if this is NOT a ftl, it's a bad form. if badFormMessage is set,
        // use that as a header
        if (!$this->ftl && isset($this->directive['badFormMessage'])) {
            $this->header .= "<span class=\"".$this->directive['badClassTag']."\">".$this->directive['badFormMessage']."</span>";
        }

        // add header if we've got one
        if ($this->header != '')
            $template .= "<tr><td colspan=\"2\"><span class=\"".$this->directive['normalClassTag']."\">$this->header</span></td></tr>\n";
        
        // preloop variable set
        $colorRow   = false;
        $lastVar    = '';; // the last variable we just looked at        

        // loop through each entity
        $firstEnt = true;
        $numFormEntities = sizeof($this->entityList);
        foreach ($this->entityList as $varName => $ent) {
                        
            // get info from entity objects for display
            $title  = '<sm type="sf" data="title" var="'.$ent->varName.'">';
            $input  = '<sm type="sf" data="entity" var="'.$ent->varName.'">';
            $layout = $ent->getLayout();
            $tA     = $ent->getTitleAlign();
            $iA     = $ent->getInputAlign();
            $bgO    = $ent->getShowBgColor();       // check for bg color setting
            
            // group support
            (isset($this->groupList[$ent->varName]))    ? $currentGroup     = $this->groupList[$ent->varName] : $currentGroup = '';         
            (isset($this->groupList[$lastVar]))    ? $lastGroup        = $this->groupList[$lastVar] : $lastGroup = '';            

            // if this variable is in the same group as the last variable, then keep the
            // same background color
            if ((($lastGroup != '')||($currentGroup != ''))&&($lastGroup == $currentGroup)) {
                                             
                // yes we're in the same group
                // which means we need to do the opposite of what colorRow currently says
                $colorRow = (!$colorRow);
            
            }
            
            // handle alternating row colors
            if (($this->directive['rowColorAlt1'] != '')&&($bgO)) {
                
                // in this case we've been told to show a background color,
                // AND the element has said we can.
                if ($colorRow) {
                    $rC = "bgcolor=\"{$this->directive['rowColorAlt1']}\"";
                    $colorRow = false;
                }
                else {
                    $rC = "bgcolor=\"{$this->directive['rowColorAlt2']}\"";
                    $colorRow = true;
                }
                
            }
            else {
                // no background color shown
                $rC = '';
                $colorRow = false;
            }
            
            // setup the row alt number
            ($colorRow) ? $this->entityList[$varName]->rowAltNum = 2 : $this->entityList[$varName]->rowAltNum = 1;

            // we need to check for a background color override.
            // this is where the element itself either tells us explicity that
            // there should or should not be a background color         
            if (!$bgO)
                $rC = '';
                        
            // handle layout based on $layout
            // 0 = Title on left, entity on right
            // 1 = Entity on left, title on right
            // 2 = Title on top<br>entity on bottom
            
            switch ($layout) {
            case SF_LAYOUT_FREEFORM:
                if ($title == '')
                    $sep = '';
                else
                    $sep = $this->directive['freeFormSeparator'];
                $template .= "$title$sep$input";
                break;
            case SF_LAYOUT_FLIPPED:
                $template .= "<tr $rC><td align=\"$iA\">$input</td><td align=\"$tA\">$title</td>";
                if ((!$this->directive['controlsOnRight'])||(!$firstEnt))
                    $template .= "</tr>\n";
                break;
            case SF_LAYOUT_SINGLE:
                $template .= "<tr $rC><td colspan=\"2\" align=\"$iA\">$title";
                if (trim($title) != '')
                    $template .= "<br />";
                $template .= "$input</td>";   
                if ((!$this->directive['controlsOnRight'])||(!$firstEnt))
                    $template .= "</tr>\n";
                break;                      
            case SF_LAYOUT_SINGLENOBR:
                $template .= "<tr $rC><td colspan=\"2\" align=\"$iA\">$title$input";
                if ((!$this->directive['controlsOnRight'])||(!$firstEnt))
                    $template .= "</tr>\n";
                break;                      
            case SF_LAYOUT_NORMAL:
            default:
                // this makes it show background color
                if ($title == '')
                    $title = '<br />';
                $template .= "<tr $rC><td align=\"$tA\">$title</td><td align=\"$iA\">$input</td>";    
                if ((!$this->directive['controlsOnRight'])||(!$firstEnt))
                    $template .= "</tr>\n";
                break;
            }
            
            // setup group info for next iteration
            $lastVar = $ent->varName;
        
            // handle controls on right         
            if (($this->directive['controlsOnRight'])&&($firstEnt)) {
            
                if ($this->directive['resetButton'] != '') {
                    $template .= "<td rowspan=\"$numFormEntities\" align=\"center\">";    

                    if ($submit != '')                    
                        $template .= '<sm type="sf" data="submit" title="'.$submit.'"><br />';

                    $template .= '<sm type="sf" data="reset" title="'.$this->directive['resetButton'].'">';
                    $template .= "</td></tr>";
                }
                else {
                    if ($submit != '')                    
                        $template .= "<td rowspan=\"$numFormEntities\">".'<sm type="sf" data="submit" title="'."$submit\"></td></tr>";
                }
                
                $firstEnt = false;

            }
            
        }

        ///////////////////////////////////////////////////////////////////
        // bottom of table, after form entities
        
        // required note
        if ($this->directive['requiredTag'] && $this->directive['showRequiredHelp'])
            $template .= "<tr><td colspan=\"2\"><span class=\"{$this->directive['requiredClassTag']}\">{$this->directive['requiredStar']} {$this->directive['requiredText']}</span></td></tr>\n";
        

        // add footer if we've got one
        if ($this->footer != '')
            $template .= "<tr><td colspan=\"2\"><span class=\"".$this->directive['normalClassTag']."\">$this->footer</span></td></tr>\n";


        // submit button
        
        if (!$this->directive['controlsOnRight']) {
        
            if ($this->directive['resetButton'] != '') {

                $template .= "<tr><td align=\"center\"";
                 
                if ($submit == '')
                    $template .= " colspan=\"2\" ";

                $template .= ">"; 

                if (!$this->directive['swapControlOrder']) {                
                    if ($submit != '')
                        $template .= '<sm type="sf" data="submit" title="'."$submit\">";
                }
                else {
                    $template .= '<sm type="sf" data="reset" title="'.$this->directive['resetButton'].'">';
                }
                
                $template .= "</td><td align=\"center\">";                

                if (!$this->directive['swapControlOrder']) {                
                    $template .= '<sm type="sf" data="reset" title="'.$this->directive['resetButton'].'">';
                }
                else {
                    if ($submit != '')
                        $template .= '<sm type="sf" data="submit" title="'."$submit\">";
                }

                $template .= "</td></tr>";

            }
            else {
                if ($submit != '')                
                    $template .= "<tr><td colspan=\"2\" align=\"{$this->directive['submitAlign']}\">".'<sm type="sf" data="submit" title="'."$submit\"></td></tr>";
            }

        }

        $template .= "</table>\n";
     
        // create a new template, set it's template data to the template we just made
        $this->template = new SM_layoutTemplate();
        $this->template->addDirective('SF', array('parentForm' => $this));
        $this->template->setTemplateData($template);

     }
    
     /**
      * return output of this form. should be called after {@link run}
      * @param string $submit value of the submit button (if not using custom template). leave blank to not add a submit button automatically
      * @param array $hiddens a hash of hidden variables (key/value pairs) to pass with the script
      * @return string the final output of the form, including all HTML, suitable for output to client
      */
    function output($submit="", $hiddens=NULL) {

        // add the ftl last
        $this->addHidden($this->formID.'_ftl','1');

        // if we don't have any entities, quit now
        if ((!isset($this->entityList)) || (!is_array($this->entityList))) {
            $this->debugLog("Warning: no input entities were added, but output was called!");
            return '';
        }

        // if we don't already have a template, make one
        if (empty($this->template))
            $this->buildTemplate($submit);        
        
        // run template, which runs all entityThinks
        $templateOutput = $this->runTemplate();        

        // gather entity javascript
        $this->buildEntityJS();
        
        // use javascript if allowed
        if (($this->directive['useJS'] == true)&&($this->jsOutput != ''))
            $onSub =  "onSubmit=\"return check_{$this->formID}_data();\"";
        else
            $onSub = $this->directive['onSubEvent'];
        
        // specify enctype if required
        if (!empty($this->directive['enctype']))
            $enctype =  'enctype="'.$this->directive['enctype'].'"';
        else
            $enctype = '';

        // specify enctype if required
        if (!empty($this->directive['formClassTag']))
            $fClass =  'class="'.$this->directive['formClassTag'].'"';
        else
            $fClass = '';

        if ($this->directive['legacyLayout']) {
            $this->sfOutput  = "\n\n<form $onSub $enctype $fClass name=\"{$this->directive['formName']}\" action=\"{$this->directive['postScript']}\" method=\"post\"";
        }
        else {
            $this->sfOutput  = "\n\n<form $onSub $enctype $fClass id=\"{$this->directive['formName']}\" action=\"{$this->directive['postScript']}\" method=\"post\"";
        }
   
        if (!empty($this->directive['formParams']))
            $this->sfOutput .= ' '.$this->directive['formParams'].' ';
        $this->sfOutput .= ">\n";
                
        if (is_array($hiddens)|| (isset($this->directive['hiddens']) && is_array($this->directive['hiddens'] ) )) {
        
            // if they're both arrays, merge them
            if (is_array($hiddens) && (isset($this->directive['hiddens']) && is_array($this->directive['hiddens'])))
                $hiddens = array_merge($hiddens, $this->directive['hiddens']);

            // if only directive hiddens is set, make that $hiddens
            if ( (isset($this->directive['hiddens']) && is_array($this->directive['hiddens'])) && !(is_array($hiddens)))
                $hiddens = $this->directive['hiddens'];
            
            foreach($hiddens as $hKey => $hVal) {
                $this->sfOutput .= "<input type=\"hidden\" name=\"{$hKey}\" value=\"$hVal\" />\n";
            }
            
        }
            
        // ** SESSIONS **
        // session variables from sessions.inc, if available
        if (isset($this->sessionH)) {        
            if (is_array($hiddens)) {
                if (is_array($this->directive['varExcludeList'])) {
                    $eList = array_merge($this->directive['varExcludeList'], array_keys($hiddens));  
                }
                else {
                    $eList = array_keys($hiddens);
                }
            }
            else {
                $eList = $this->directive['varExcludeList'];
            }
            $this->sfOutput .= $this->sessionH->formID($eList);
        }
                    
        // if we've been told to dump the template, do so here
        if ((!empty($this->directive['dumpTemplate'])) && ($this->directive['dumpTemplate'])) {
            if (is_string($this->template))
                $this->sfOutput .= "<!--\n\n TEMPLATE DUMP\n\n$this->template\n\n -->\n";
            else {
                $tOutput = join("\n",$this->template->htmlTemplate);
                $this->sfOutput .= "<!--\n\n TEMPLATE DUMP\n\n{$tOutput}\n\n -->\n";
            }
        }
                
        // run template, which runs all entityThinks
        $this->sfOutput .= $templateOutput;
        
        // end form
        if (!$this->directive['formIsEnded'])     
            $this->sfOutput .= "\n</form>\n";
        
        // return final output
        return $this->sfOutput;
    
    }

    
     /**
      * Get the value of a variable that's been added to the form, from the current POST of the script
      * @param string $varName the variable to retrieve
      */
    function getVar($varName) {
        // if we have the entity listed, use its getValue which applies frobs.
        // otherwise just get the POST value, which maintains backwards compatibility
        // and allows for internalVarList
        $v = NULL;
        if (isset($this->entityList[$varName])) {
            $v = $this->entityList[$varName]->inputEntity->getValue();
        }
        if (empty($v)) {
            $v = $this->inVarH->getPOST($varName);
        }
        return $v;
    }
    
    /**
     * get a reference to a {@link SM_formEntity} (NOT the input entity) by
     * variable ID
     * @see getEntity
     * @param string $varName form variable to retrieve
     * @return SM_formEntity reference to SM_formEntity object, or NULL if not found
     */
    function getFormEntity($varName) {

        // sanity
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("getFormEntity: variable not found: $varName");
            return NULL;
        }

        return $this->entityList[$varName];

    }

    /** 
     * determine if an entity has been added to the form, by entity name
     * @param string $varName the var (entity) name
     * @return bool true if entity is part of this form, false otherwise
     */
    function entityExists($varName) {
        return isset($this->entityList[$varName]);
    }

    /**
     * get a reference to a smartform input entity that has been added to the form
     * by variable id
     * @param string $varName form variable to retrieve
     * @return SM_inputTypeEntity reference to SM_inputTypeEntity object requested, or NULL if not found
     */
    function getEntity($varName) {
        
        // sanity
        if (!isset($this->entityList[$varName]->inputEntity)) {
            $this->debugLog("getEntity: variable not found: $varName");
            return NULL;
        }

        return $this->entityList[$varName]->inputEntity;

    }

    /**
     * see what type of entity a certain varname represents
     * by variable id
     * @param string $varName form variable to lookup
     * @return string a type of input entity, or NULL if not found
     */
    function getEntityType($varName) {
        
        // sanity
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("getEntityType: variable not found: $varName");
            return NULL;
        }

        return $this->entityList[$varName]->getType();

    }

    
    /**
     * return the amount of input entities that have been added to the form
     * @return int number of entities
     */
    function getNumVars() {
        return sizeof($this->entityList);
    }

    /**
     * return an array of all input variable names in use
     * @return array list of variables which can be used with getVar()
     */
     function getVarList() {
        
        // return main list 
        $varList = array_keys($this->entityList);

        // return 'internal' list, which would be entities that contain
        // more than one input variable
        if (is_array($this->internalVarList)) {        
            foreach ($this->internalVarList as $vName) {
                $varList[] = $vName;
            }
        }
        
        return $varList;
     
     }
    
     /**
      * Return a hash of all input variable names in use, keyed by field name
      * and the value set to getVar(fieldName)
      * @param array $getList a list of fields to exclusively include - all others excluded
      * @return array hash of fieldName => form value
      */
     function getVarHash($getList=NULL) {

         $varList = $this->getVarList();
         foreach ($varList as $fieldName) {
             if (($getList != NULL) && (!in_array($fieldName, $getList))) {
                 continue;
             }
             $varHash[$fieldName] = $this->getVar($fieldName);
         }

         return $varHash;
        
     }

    /**
     * dump all variables for this form (module). 
     * useful for debugging after data gets verified
     */
    function dumpFormVars() {
        
        $varList = $this->getVarList();
        $output = '';
        foreach ($varList as $formVar) {
            if (isset($_POST[$formVar])) {
                $output .= $formVar." => ".$this->getVar($formVar).'|'.$_POST[$formVar]."<br />";
            }
            else {
                $output .= $formVar.' => [NOT PASSED IN POST]<br />';
            }
        }
        
        return $output;
    
    }
    
    /**
     * dump smartform information
     * @return string debug information
     */
    function dumpInfo() {

        $op = "SmartForm Information:<br /><ul>\n";

        $op .= $this->getClassLog();

        $op .= "Entity List:<br />";
        $varList = $this->getVarList();
        foreach ($varList as $var) {
            $op .= "variable [$var], type ".get_class($this->entityList[$var]->inputEntity)."<br />\n";
        }

        $op .= $this->dumpDirectives();

        $op .= "</ul>";

        return $op;

    }

    /**
     * XML HANDLE FUNCTIONS 
     */

    /**
     * parse xml form into smartform objects
     *
     */
    function _parseSF_XML() {

        // Loop through each entity
        // Make certain there are entities
        if (!empty($this->xEntity)) {
            foreach ($this->xEntity as $entityName => $entityValue) {
                
                // store entity name
                $eLoadList[] = $entityName;

                /*
                
                Use the default layout directive when set                
                
                */

                if (!(isset($this->xEntity[$entityName]['layout']))) {
                    $entityValue['layout'] = $this->directive['defaultLayout'];
                }

                if (!isset($entityValue['inputAlign']))
                    $entityValue['inputAlign'] = '';
                if (!isset($entityValue['titleAlign']))
                    $entityValue['titleAlign'] = '';
                if (!isset($entityValue['required']))
                    $entityValue['required'] = false;
                if (!isset($entityValue['value']))
                    $entityValue['value'] = '';
                

                // Add the element
                $entity = $this->add($entityName,
                                      $entityValue['title'],
                                      $entityValue['type'],
                                      $entityValue['required'],
                                      $entityValue['value'],
                                      NULL,
                                      $entityValue['layout'],
                                      $entityValue['titleAlign'],
                                      $entityValue['inputAlign']
                                      );
    
                if ($entity == NULL) {
                    $this->debugLog("unable to add loaded entity to SmartForm: $entityName (defined twice?)");
                    continue;
                }
                
                // Add Filters to the form element
                if (isset($entityValue['filter'])) {
                    foreach ($entityValue['filter'] as $filterID => $filterIDValue) {
                        foreach($filterIDValue as $filterName => $filterValue){
                            // if there is no array parameter use a simple addDirective.
                            if (empty($filterID)) {
                                // filter value used to store the bad message for a filter with no id.
                                $this->addFilter($entityName,$filterName,$filterValue);
                            } else {
                                // if a filter array (name) is set
                                // then read in the arguments, create an array and set the filter arguments
                                // for each item set
    
                                // create the filter if it doesnt exist
                                if (!isset($this->entityList[$entityName]->inputEntity->filterList[$filterID])) {
                                    // filter message
                                    $filterMessage = $entityValue['filter'][$filterID]['message'];
                                    $this->addFilter($entityName,$filterID,$filterMessage);
                                } 
    
                                $this->setFilterArgs($entityName,$filterID,array($filterName=>$filterValue));
    
                            }
                        }
                    }
                } // end filter
                
                // Add Directives to the form element
                if (isset($entityValue['directive'])) {
                    foreach ($entityValue['directive'] as $directiveID => $directiveIDValue) {
                        foreach($directiveIDValue as $directiveName => $directiveValue) {
                            // if there is no array parameter use a simple addDirective.
                            if (empty($directiveID)) {
                                $entity->addDirective($directiveName,$directiveValue);
                            } else {
                                // if a KEY was set, the value for this directive is an array

                                // check for the special key 'optionList', and use addOption on
                                // this entity
                                if (($directiveID == 'optionList')&&(method_exists($entity,'addOption'))) {
                                    $entity->addOption($directiveValue,$directiveName);
                                }
                                else {                                
                                    // set the array directly
                                    $entity->directive[$directiveID][$directiveName]=$directiveValue;
                                }
                            }
                        }
                    } 
                }  // end directives
            } // end check of existing entities
        } // end entity

        // clear xEntity
        $this->xEntity = NULL;

        return $eLoadList;

    }

    /**
     * parse an XML form retrieved from an external source, such as a database
     * @param string $data the XML data to parse
     * @return array the list of field names loaded from the XML form
     */
    function loadFormData($data) {

        // if we dont have a reader yet, make it
        if ($this->cFormReader == NULL) {
            $this->cFormReader = SM_loadConfigReader('SMARTFORM', $this);
        }

        $this->cFormReader->XMLsetContent($data);
        $this->cFormReader->parseXML();

        return $this->_parseSF_XML();

    }

    /**
     * force a bad entity, as if a filter had failed, with the specified message
     * @param string $varName the variable to set
     * @param string $badMsg the message to show inline
     */
    function triggerBadEntity($varName, $badMsg) {
        // sanity
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("triggerBadEntity: variable not found: $varName");
            return false;
        }

        $this->entityList[$varName]->inputEntity->isValid = false;
        $this->entityList[$varName]->inputEntity->fixMessage = $badMsg;
        
        return true;
    }
    
    /**
     * load a SmartForm definition through an XML file
     * @param string $configFile the xml file
     * @return array the list of field names loaded from the XML form
     */
    function loadForm($configFile) {

        global $SM_siteManager;

        // now find and load form. failing to find this WILL be fatal
        $configFile = $SM_siteManager->findSMfile($configFile, 'smartForms', 'xsm');

        if ($GLOBALS['SM_develState'])        
            $this->debugLog("loadForm(): $configFile");

        // fall through: get config
        // if we dont have a reader yet, make it
        if ($this->cFormReader == NULL) {
            $this->cFormReader = SM_loadConfigReader('SMARTFORM', $this);
        }
        
        // check cache?
        $willCache = false;
        if ($this->siteConfig->getGlobalVar('cache','cacheSmartForm')) {

            $willCache = true;
            if (!$this->loadSerial($configFile, 'SMARTFORM', $this->xEntity)) {
                // if loadserial fails, parse the file
                $this->cFormReader->readConfigXML($configFile);
            }

        }
        else {        

            // fall through: parse
            $this->cFormReader->readConfigXML($configFile);
            
        }

        // should we cache the config file?
        if ($willCache) {
            $this->saveSerial($configFile, 'SMARTFORM', $this->xEntity);
        }

        return $this->_parseSF_XML();

    }

    /**
     * determine if the given button element was pressed.
     * @param $varName the name of the button element that has already been added to the form
     * @return bool 
     */
    function buttonPressed($varName) {
        
        // sanity
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("buttonPressed: variable not found: $varName");
            return false;
        }

        return ($this->getVar($varName) == $this->entityList[$varName]->inputEntity->directive['value']);
        
    }

}


?>
