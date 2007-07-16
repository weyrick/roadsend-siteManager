<?php

/*********************************************************************
*  Roadsend SiteManager
*  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
**********************************************************************
*
* This source file is subject to version 1.0 of the Roadsend Public
* License, that is bundled with this package in the file 
* LICENSE, and is available through the world wide web at 
* http://www.roadsend.com/license/rpl1.txt
*
**********************************************************************
* Author(s): weyrick
*
*/

/*

SM Object
    Common SM Variables
        siteConfig x
        dbH x
        dbHL x
        sessionH x
        inVarH x
        errorHandler x
    Localization
        Loading XML language file
        Using getText
        Testing browser provided locale
    Debug
        debugLog x
        fatalErrorPage ?
        dbErrorCheck x
    Directives
        Getting x
        Setting x
            w/Overwrite On/Off x
        Loading from XML x
*    Serialization
        Saving
        Loading    
?    Debug
        dumpInfo
        dumpDirectives
        Class Log
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_smObjects extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'SM Object Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the SM Object.';

     /**
      * run by base class after base runs moduleConfig()
      */
    function T_moduleConfig() {

        // module config stuff here


    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {

        // TEST
        $actual = gettype($this->siteConfig).':'.get_class($this->siteConfig);
        $expect = 'object:SM_siteConfig';
        $this->addTest('handler ref: siteConfig', 
                       'verify $siteConfig variable is initialized',
                       $expect,
                       $actual);
    
        // TEST
        $actual = gettype($this->dbH).':'.get_class($this->dbH);
        $expect = 'object:PDO';
        $this->addTest('handler ref: dbH', 
                       'verify $dbH member variable is initialized',
                       $expect,
                       $actual);
    
        // TEST
        $actual = gettype($this->dbHL);
        $expect = 'array';
        $this->addTest('handler ref: dbHL', 
                       'verify $dbHL member variable is initialized',
                       $expect,
                       $actual);
    
        // TEST
        $actual = gettype($this->sessionH).':'.get_class($this->sessionH);
        $expect = 'object:SM_session';
        $this->addTest('handler ref: sessionH', 
                       'verify $sessionH member variable is initialized',
                       $expect,
                       $actual);
    
        // TEST
        $actual = gettype($this->inVarH).':'.get_class($this->inVarH);
        $expect = 'object:SM_inVarManager';
        $this->addTest('handler ref: inVarH', 
                       'verify $inVarH member variable is initialized',
                       $expect,
                       $actual);
    

        // TEST
        $actual = gettype($this->errorHandler).':'.get_class($this->errorHandler);
        $expect = 'object:SM_errorHandler';
        $this->addTest('handler ref: errorHandler', 
                       'verify $errorHandler member variable is initialized',
                       $expect,
                       $actual);

        // TEST
        global $SM_debugOutput;
        $this->debugLog("test debuglog entry",5);
        $testEntry = array_pop($SM_debugOutput);
        $actual = $testEntry;
        $expect = array('msg' => 'test_smObjects:: test debuglog entry', 'verbosity' => 5);
        $this->addTest('debugLog() method', 
                       'make sure debugLog() method writes to global debugLog',
                       $expect,
                       $actual);

        // TEST
        $this->addDirective('testKey','testVal');
        $this->addDirective('testKey','TEST VALUE');
        $actual = $this->getDirective('testKey');
        $expect = 'TEST VALUE';
        $this->addTest('addDirective() #1', 
                       'test addDirective method, overwrite to true',
                       $expect,
                       $actual);

        // TEST
        $this->addDirective('testKey','testVal', true);
        $this->addDirective('testKey','TEST VALUE', false);
        $actual = $this->getDirective('testKey');
        $expect = array('testVal','TEST VALUE');
        $this->addTest('addDirective() #2', 
                       'test addDirective method, overwrite to false',
                       $expect,
                       $actual);

        // TEST                       
        $this->addDirective('single','sVal');
        $this->addDirective('single',array('aVal1','aVal2'), false);
        $actual = $this->getDirective('single');
        $expect = array('sVal','aVal1','aVal2');
        $this->addTest('addDirective() #3', 
                       'test addDirective method, overwrite to false w/mixed array/scalar',
                       $expect,
                       $actual);

        // TEST                       
        $this->addDirective('array',array('aVal1','aVal2'));
        $this->addDirective('array','sVal',false);
        $actual = $this->getDirective('array');
        $expect = array('aVal1','aVal2','sVal');
        $this->addTest('addDirective() #4', 
                       'test addDirective method, overwrite to false w/mixed array/scalar',
                       $expect,
                       $actual);

        // TEST                       
        $this->addDirective('array2',array('aVal1','aVal2'));
        $this->addDirective('array2',array('nVal1','nVal2'),false);
        $actual = $this->getDirective('array2');
        $expect = array('aVal1','aVal2','nVal1','nVal2');
        $this->addTest('addDirective() #5', 
                       'test addDirective method, overwrite to false w/ two arrays',
                       $expect,
                       $actual);
                       

        // TEST
        $this->addDirective('testGet','GET VALUE');
        $actual = $this->getDirective('testGet');
        $expect = 'GET VALUE';
        $this->addTest('getDirective()', 
                       'test getDirective method',
                       $expect,
                       $actual);

        // TEST
        $this->loadConfig('smObjectTest');
        $actual = $this->getDirective('loadedDirective');
        $expect = 'Loaded Value';
        $this->addTest('loadConfig()', 
                       'test loading of directives through XML configuration file',
                       $expect,
                       $actual);


        // TEST
        $this->loadLanguage('testSite-lang');
        $eText = $this->getText('text1','en_us'); // test specification of english
        $dText = $this->getText('text1','da_dk'); // test specification of danish
        $fText = $this->getText('text1','fr_fr'); // test specification of french
        $actual = $eText.'<br />'.$dText.'<br />'.$fText;
        $expect = 'This is the text<br />Dette er en tekst<br />C\'est le texte';
        $this->addTest('loadLanguage()', 
                       'test loadLanguage(), getText() internationalization functions',
                       $expect,
                       $actual);



    }
    
}


?>
