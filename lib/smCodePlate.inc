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
 * a class to create pre-configured templates ("canned templates") that have 
 * roughly the same template and module layout, but are used in multiple
 * pages (directive scripts). this saves you having to create the same
 * layout system in multiple directive scripts.
 *
 * this class should be extended for use. override codePlateConfig() and
 * codePlateThink() for functionality. 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 */
class SM_codePlate extends SM_layoutTemplate {

    /**
     * inVarClient object. use addInVar() and getVar() to access.
     * @var object SM_inVarClient 
     */
    var $inVarClient = NULL;

    /**
     * constructor. setup a new codeplate. assign database and session handlers.
     * constructor will call SM_codePlate::codePlateConfig() which the extending
     * class must define. codePlateConfig() should call setMyTemplate()
     *
     */
    function SM_codePlate() {

        // setup handlers
        $this->_smoConfigure();

        // base template config
        $this->_templateConfig();

        // in var defaults
        $this->directive['propagateInVars']     = $this->siteConfig->getVar('flags','propagateInVarDefault');
        $this->directive['dieOnBadInVarType']   = true;

        // call codePlateConfig(), which should be a method
        // defined by the extending class
        // in this class a call to setMyTemplate() should be made
        $this->codePlateConfig();

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
     * retrieve a list of inVars that should be propagated 
     * @return array a list of inVars that should be propagated or NULL if none
     */
    function getInVarP() {
        $this->_requireInVarClient();
        return $this->inVarClient->getInVarP();
    }

    /**
     * get the value of a variable defined through inVar
     * optionally pass a second parameter that the value should be set to
     * if the variable you're requesting is empty
     * use this to retrieve variables declared as inVar's
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
        }

        // call parent
        parent::addDirective($key, $val, $overWrite);

    }

    /**
     * set the HTML template file this codePlate will be based on. it
     * will attempt to parse the file given. it will search template
     * paths for the file specified, automatically adding .tpt extension
     * if you want to load the text yourself (for example, from a database)
     * use setTemplateData()
     *
     * @param string $fName the file name of the HTML template
     */
    function setMyTemplate($fName) {
        
        $this->htmlTemplateFile = $fName;
        $this->parseTemplate();

    }

    /**
     * load a module for use in this codeplate. optionally add it immediately to
     * an area in my template. it will search module paths. note, returns by reference.
     * if $areaName isn't specified, the module must be added by hand later through
     * $this->addModule()
     *
     * @param string $moduleFile the module file to load (.mod file)
     * @param string $areaName add this module to an area of the template
     * @param string $moduleName if the class name is different from the module
     *                             file name, specify it here
     * @return SM_module the loaded module
     */
    function loadModule($moduleFile, $areaName='',$moduleName='') {

        global $SM_siteManager;

        $newModule = $SM_siteManager->loadModule($moduleFile,$moduleName);

        if ($areaName != '') {
            $this->addModule($newModule,$areaName);
        }

        return $newModule;

    }


    /**
     * load a template for use in this codeplate. optionally add immediately it to
     * an area in my template. it will search template paths. note, returns by reference
     * if $areaName isn't specified, the template can be added later by hand through $this->addTemplate()     
     *
     * @param string $templateFile the template file to load (.tpt file)
     * @param string $areaName add this template to an area of the codePlate's template
     * @return SM_layoutTemplate the loaded template
     */
    function loadTemplate($templateFile, $areaName='') {

        global $SM_siteManager;

        $newTemplate = $SM_siteManager->loadTemplate($templateFile);

        if ($areaName != '') {
            $this->addTemplate($newTemplate,$areaName);
        }

        return $newTemplate;

    }


    /**
     * load a (sub) codePlate for use in this codeplate. optionally add immediately it to
     * an area in my template. it will search codePlate paths. note, returns by reference
     * if $areaName isn't specified, the codePLate can be added later by hand through $this->addTemplate()
     *
     * @param string $codePlateFile the template to load
     * @param string $areaName add this codePlate to an area of my template
     * @return SM_codePlate the loaded codePlate
     */
    function loadCodePlate($codePlateFile, $areaName='') {

        global $SM_siteManager;

        $newCodePlate = $SM_siteManager->loadCodePlate($codePlateFile);

        if ($areaName != '') {
            $this->addTemplate($newCodePlate,$areaName);
        }

        return $newCodePlate;

    }

    /**
     * this method is called when the inVars passed to the script didn't meet the criteria
     * specified in this module - either by type or requiredness. by default this method
     * output an SM_fatalErrorPage, but may be overridden to do anything you'd like
     */
    function eventBadInVar() {
        $this->fatalErrorPage('This script was accessed incorrectly.');
    }

    /**
     * override SM_layoutTemplate::_run and allow codePlate to run it's
     * codePlateThink() method     
     */
    function run() {

        // prep tags
        $this->_prepTags();

        // verify inVars
        $this->_requireInVarClient();
        if ((isset($this->inVarClient))&&(!$this->inVarClient->verifyInVars($failedVar))) {
            // inVar check was unsuccessful
            $this->eventBadInVar($failedVar);
        }
        
        // run codePlate think code
        if (method_exists($this,'codePlateThink')) {
            $this->codePlateThink();
        }
        
        // run the tags
        $this->_runTags();
        
        // return output
        return $this->htmlOutput;

    }


}

?>
