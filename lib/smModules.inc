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
 *
 *  the module base class. modules are objects that are placed into templates to give a site
 *  site functionality
 *
 *  all siteManager modules should extend this class (or a descendent of it)
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 */
class SM_module extends SM_object { 

    /** 
    * the moduleID
    * @var string
    */
    var $moduleID = '';

    /** 
    * the final output of the module 
    * @var string
    */
    var $moduleOutput = '';  

    /**
     * inVarClient object. use addInVar() and getVar() to access.
     * @var SM_inVarClient
     */
    var $inVarClient = NULL;

    /** 
    * a list of smart forms this module contains. array of references to smartform objects 
    * @var array
    */
    var $formList = array();
            
    /** 
    * a list of methods to call BEFORE moduleThink() 
    * @var array
    */
    var $preThinkList = array();

    /** 
    * a list of methods to call AFTER moduleTnink() 
    * @var array
    */
    var $postThinkList = array();

    /** 
    * a list of methods to call AFTER moduleConfig() 
    * @var array
    */
    var $postConfigList = array();

    /**
     * serialize the module ids
     */
    static private $moduleCount = 0;

    /**
     * different output wrappers
     */
    const WRAP_NONE = 0;
    const WRAP_SPAN = 1;
    const WRAP_DIV  = 2;
    const WRAP_P    = 3;

    /**
     * construction. does some initializing. will run moduleConfig() method if it
     * exists in this class
     */
    function SM_module() {
            
        // smobject setup
        $this->_smoConfigure();

        // configure module
        $this->moduleID     = "SM".self::$moduleCount++;

        // setup default directives
        $this->directive['outputWrapper']       = self::WRAP_DIV;
        $this->directive['outputClass']         = 'mod '.get_class($this);
    
        $this->directive['dieOnBadInVarType']   = true;
    
        $this->directive['propagateInVars']     = $this->siteConfig->getVar('flags','propagateInVarDefault');
        
        // run config for module, if it has one
        if (method_exists($this, "moduleConfig"))
            $this->moduleConfig();    
        
        // run postconfig methods
        if ((isset($this->postConfigList))&&(is_array($this->postConfigList))) {
            foreach ($this->postConfigList as $method) {
                call_user_func(array($this,$method));
            }
        }

    }
             
    
     /**
     * override addDirective from SM_object to take care of some
     * extra processing
     */
     
    function addDirective($key, $val='', $overWrite=true) {

        // check key, some which we need to send to
        // the inVar client
        switch ($key) {
        case 'dieOnBadInVarType':
        case 'propagateInVars':
            $this->_requireInVarClient();
            $this->inVarClient->addDirective($key,$val);
            break;
        }

        // call parent
        parent::addDirective($key, $val, $overWrite);

    }
         
     /**
      * this function will cause the module to "run" itself.
      * that is, execute methodThink() method, and return
      * the results of the module output as a string.
      *
      * @return string the module output
      */
    function run() {

        // see if we should load or not
        // this allows the user to run some code that will decide
        // if this module should display anything or not
        if (!$this->canModuleLoad()) {        
            $this->moduleOutput = '';
            return '';
        }

        if ($this->directive['outputWrapper'] != self::WRAP_NONE) {
            switch ($this->directive['outputWrapper']) {
                default:
                case self::WRAP_DIV:
                    $w = 'div';
                    break;
                case self::WRAP_P:
                    $w = 'p';
                    break;
                case self::WRAP_SPAN:
                    $w = 'span';
                    break;
            }
            if ($this->directive['outputClass'] != '')
                $class = 'class="'.$this->directive['outputClass'].'"';
            $this->moduleOutput = "<{$w} id=\"{$this->moduleID}\" {$class}>\n";
        }
        
        // verify inVars
        $this->_requireInVarClient();   
        if (!$this->inVarClient->verifyInVars($failedVar)) {
            // inVar check was unsuccessful
            $this->eventBadInVar($failedVar);
        }

        // run prethink methods
        if ((isset($this->preThinkList))&&(is_array($this->preThinkList))) {
            foreach ($this->preThinkList as $method) {
                call_user_func(array($this,$method));
            }
        }

        // this will get the ouput into our output variable.
        $this->moduleThink();

        // run postthink methods
        if ((isset($this->postThinkList))&&(is_array($this->postThinkList))) {
            foreach ($this->postThinkList as $method) {
                call_user_func(array($this,$method));
            }
        }
        
                
        // end wrapper
        if ($this->directive['outputWrapper'] != self::WRAP_NONE)
            $this->moduleOutput .= "</{$w}>\n";

        // that's all, folks.
        return $this->moduleOutput;
    }   


    /**
     * this method is called when the inVars passed to the script didn't meet the criteria
     * specified in this module - either by type or requiredness. by default this method
     * output an fatalErrorPage, but may be overridden to do anything you'd like
     *
     * @param string $varName the variable that failed the check, if known
     */
    function eventBadInVar($varName='', $wantedType='') {
        
        global $SM_develState;
        $message = 'This script was accessed incorrectly.';
        if($SM_develState && ! empty($varName)) {
            $message .= " Offending inVar: $varName";
            if ($wantedType != '')
                $message .= ' wanted type '.$wantedType.'.';
        }
        $this->fatalErrorPage($message);
    }


    /**
     * called by template object when templateReset() is called. it should reset
     * class variables, free memory, etc and prepare for another run
     */
    function _resetModule() {

        $this->moduleOutput = NULL;

        if (method_exists($this, "resetModule"))
            $this->resetModule();

    }


    /**
     * load a module for use in this module. searches module paths
     * NOTE: you should be using useModule() if possible
     * @param string $moduleFile the module file to load (.mod file)
     * @param string $moduleName if the class name is different from the module
     *                             file name, specify it here
     * @return SM_module the loaded module, or NULL if fatalFNF is false and it wasn't found
     */
    function loadModule($moduleFile, $moduleName='', $fatalFNF=true) {

        global $SM_siteManager;

        $newModule = $SM_siteManager->loadModule($moduleFile, $moduleName, $fatalFNF);

        return $newModule;

    }

   /**
    * load a module for use in this module. searches module paths
    * NOTE: you should be using useTemplate() if possible
    *
    * @param string $templateFile the template file to load (.tpt file)
    * @param bool $fatalFNF when true, if the file wasn't found, will exit with SM_fatalErrorPage
    * @return SM_layoutTemplate the loaded template, or NULL if fatalFNF=false and it wasn't found
    */
   function loadTemplate($templateFile, $fatalFNF=true) {

       global $SM_siteManager;

       $newTemplate = $SM_siteManager->loadTemplate($templateFile, $fatalFNF);

       return $newTemplate;

   }


   /**
    * load a codePlate for use in this module. seraches codeplate paths
    *
    * NOTE: you should be using useCodePlate() if possible    
    * @param string $codePlateFile the template to load
    * @return SM_codePlate the loaded codePlate, or NULL if fatalFNF=false and it wasn't found
    */
   function loadCodePlate($codePlateFile, $fatalFNF=true) {

       global $SM_siteManager;

       $newCodePlate = $SM_siteManager->loadCodePlate($codePlateFile, '', $fatalFNF);

       return $newCodePlate;

   }


    /**
     * this method is run after moduleConfig and before moduleThink. you can override this method
     * in your module to have it decide whether it should show itself or not. this might come up
     * for example, when you need to check a database a see if any values exist, and if not don't
     * display the module. since it is run after moduleConfig, you can use all declared inVars
     * and directives to make the descision.
     *
     * if this method is not overriden, it returns true which will run moduleThink as normal
     *
     * @return bool true if loading should continue, false if we should NOT run moduleThink() and stop here
     */
    function canModuleLoad() {
        return true;
    }
    

    /**
     * this function creates a link to the desired URL, and adds the session information automatically
     * NOTE: this is a wrapper for the $this->sessionH->uLink function
     * @param string $link the URL location to link to     
     * @param array $exList a list of variables to specifically NOT include in the URL
     */
    function uLink($link, $exList=NULL) {       
        return $this->sessionH->uLink($link, $exList);
    }

    
    /**
     * this function creates a link to the desired URL, and adds the session information automatically
     * NOTE: this is a wrapper for the $this->sessionH->hLink function
     * @param string $link the URL location to link to
     * @param string $text the text to link (ie, the text between <A></A>
     * @param string $class the CLASS (ie, style sheet element) to use for this text
     * @param string $extra extra data to stick into the <A> tag. may be CLASS, TARGET, etc. 
     * @param string $display if set to true, it will echo immediately. otherwise, only returns output     
     * @param array $exList a list of variables to specifically NOT include in the URL
     * @param string $anchor optional anchor to add to URL (ie, #anchor at the end of URL)
     */
    function hLink($link, $text, $class='', $extra='', $exList=NULL, $anchor=NULL) {       
        return $this->sessionH->hLink($link, $text, $class, $extra, $exList, $anchor);
    }


    /**
     * this function creates a link to the desired URL, and adds the session information automatically
     * NOTE: this is a wrapper for the saying the output of $this->sessionH->hLink() function
     * @param string $link the URL location to link to
     * @param string $text the text to link (ie, the text between <A></A>
     * @param string $class the CLASS (ie, style sheet element) to use for this text
     * @param string $extra extra data to stick into the <A> tag. may be CLASS, TARGET, etc. 
     * @param string $display if set to true, it will echo immediately. otherwise, only returns output
     */
    function sayHLink($link, $text, $class='', $extra='', $exList=NULL) {       
        $this->say($this->sessionH->hLink($link, $text, $class, $extra, $exList));
    }

    /**
     * Create a new SmartForm for use in this module
     * @return SM_smartForm reference to a new SmartForm object
     */
    function newSmartForm() {
               
        // make sure smartForm component loaded
        SM_loadComponent('SM_smartForm');

        // create the new form
        $myForm = new SM_smartForm($this->moduleID.'_SF'.sizeof($this->formList));
        
        // add it to our list
        $this->formList[] = $myForm;

        // return the new smartform to them
        return $myForm;
            
    }
    
    /**
     * private method to make sure inVar client component is loaded and
     * has been instantiated for use by this module
     */
    function _requireInVarClient() {

        if (!isset($this->inVarClient)) {

            // load component
            SM_loadComponent('SM_inVarClient');

            // create the inVar client
            $this->inVarClient = new SM_inVarClient($this);
            $this->inVarClient->addDirective('propagateInVars',$this->directive['propagateInVars']);

        }

    }

    /**
     * declare that we are looking for certain variables to be passed (inVar)
     * (either POST, GET, or from sessions) for use in this module
     * optionally give a default value to use if it wasn't passed
     * optionally describe which variable type the value should have (for security)
     * optionally decide whether this inVar should be propagated or not
     *
     * @param string $varName the variable name we're looking for
     * @param mixed $defaultVal the default value to use, if it wasn't passed
     * @param string $expectType the expected type of the inVar. must be a valid PHP type
     * @param bool $propagate whether to include this inVar in links and FORMS when set     
     * @param bool $req whether this inVar is considered 'required' or not. if it's required but not
     *                    passed to the script, the script will exit with an SM_fatalErrorPage
     */
    function addInVar($varName, $defaultVal=NULL, $expectType='', $propagate=NULL, $req=false) {
        $this->_requireInVarClient();
        return $this->inVarClient->addInVar($varName, $defaultVal, $expectType, $propagate, $req);
    }


    /**
     * declare that we will be propagating a variable. this is synonamous with creating
     * a propagated inVar, and is here for code readability reasons
     * 
     * @param string $varName the variable name we will propagate
     * @param mixed $defaultVal the default value to use, if it wasn't passed
     * @param string $expectType the expected type of the inVar. must be a valid PHP type
     */
    function addOutVar($varName, $defaultVal=NULL, $expectType='') {
        $this->_requireInVarClient();
        return $this->inVarClient->addInVar($varName, $defaultVal, $expectType, true);
    }

    /**
     * set the value of a variable defined through inVar
     * use this to override variables declared as inVar's
     * @see SM_inVarClient::setInVar    
     * @param string $varName the variable to override
     * @param mixed $varValue 
     * @return bool true if successful, false if not successful
     */
    function setInVar($varName, $varValue) {
        $this->_requireInVarClient();
        return ($this->inVarClient->setInVar($varName,$varValue));
    }


    /**
     * get the value of a variable defined through inVar
     * optionally pass a second parameter that the value should be set to
     * if the variable you're requesting is empty
     * use this to retrieve variables declared as inVar's
     * @see SM_inVarClient::getVar    
     * @param string $varName the variable to retrieve
     * @param mixed $default if $varName is empty(), this default value will be returned instead
     * @return mixed current value of the inVar requested (or $default or NULL if not set)
     */
    function getVar($varName, $default=NULL) {

        $this->_requireInVarClient();

        // get value from inVar client, check type
        $varVal = $this->inVarClient->getVar($varName);

        // return
        if (isset($varVal))
            return $varVal;
        else
            return $default;

    }


    /**
     * retrieve a list of inVars that should be propagated 
     *
     * @return array a list of inVars that should be propagated or NULL if none
     */
    function getInVarP() {
        $this->_requireInVarClient();
        return $this->inVarClient->getInVarP();
    }

   
    /**
     * retrieve a list of inVars this module uses
     * @return array list of inVars
     */
    function getInVarList() {
        $this->_requireInVarClient();
        return $this->inVarClient->getInVarList();
    }

    /**
     * retrieve a single hash of all current inVar values
     * @return array associative array, keyed by declared inVars, populated by the current getVar() value
     */
    function getInVarValues() {
        $ivList = $this->getInVarList();
        $valData = array();
        foreach ($ivList as $inVar) {
            $valData[$inVar] = $this->getVar($inVar);
        }
        return $valData;
    }

    /**
     * dump the value of all variables we've setup as inVars
     * for debugging purposes
     */
   function dumpInVars() {
       $this->_requireInVarClient();
       return $this->inVarClient->dumpInVars();
   }

    
     /**
      * a simple wrapper function that adds $text to this modules final output
      * @param string $text text to be added to final output
      */
    function say($text) {
        $this->moduleOutput .= $text;
    }

     /**
      * like say, but adds a newline
      * @param string $text text to be added to final output
      * @param string $sName internal style name to use for this output. will wrap in SPAN tag
      */
    function saynl($text, $sName='') {
        $this->say($text."\n", $sName);
    }

     /**
      * same as say(), but adds a <BR> to the end of the output
      * @param string $text text to be added to final output. leave blank for just a <BR>
      * @param string $sName internal style name to use for this output. will wrap in SPAN tag
      */
    function saybr($text='',$sName='') {
        $this->say($text."<br />\n",$sName);
    }


   /**
   * dump information on this module, for debugging purposes
   */
    
    function dumpInfo() {

        $output = "Module Name: ".get_class($this)."<hr /><br />\n";
        $output .= $this->dumpInVars();
        $output .= $this->dumpDirectives();
        $output .= "module has ".sizeof($this->formList)." smartForm objects<br /><br />\n";

        // dump smartForm information, if any
        if (sizeof($this->formList)) {            
            foreach ($this->formList as $smartForm) {
                $output .= $smartForm->dumpInfo();
            }
        }

        $output .= "<br />";

        return $output;

    }

    
}


?>
