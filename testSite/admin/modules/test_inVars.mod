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

InVars
    Security
        Specify Type
        Specify Valid List
        Required
    Propagating        
    Manager
        getVar x
        getPOST x
        getGET x
        getSessionVar
        getCookieVar
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_inVars extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'InVar Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the InVar System.';

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
        $actual = $this->inVarH->getVar('t1');
        $expect = 'test';
        $this->addTest('getVar() #1', 
                       'make sure getVar() recieves a GET variable',
                       $expect,
                       $actual);
    
        // TEST
        $actual = $this->inVarH->getVar('t2','int');
        $expect = '201';
        $this->addTest('getVar() #1', 
                       'make sure getVar() recieves a GET variable, with type checking',
                       $expect,
                       $actual);

        // TEST
        $actual = $this->inVarH->getGET('t1');
        $expect = 'test';
        $this->addTest('getGET() #1', 
                       'make sure getGET() recieves a GET variable',
                       $expect,
                       $actual);
        
        // TEST
        $actual = $this->inVarH->getGET('t2','int');
        $expect = '201';
        $this->addTest('getGET() #1', 
                       'make sure getGET() recieves a GET variable, with type checking',
                       $expect,
                       $actual);

        // TEST
        $_POST['p1'] = 'testPOST';
        $actual = $this->inVarH->getPOST('p1');
        $expect = 'testPOST';
        $this->addTest('getPOST() #1', 
                       'make sure getPOST() recieves a POST variable (simulated)',
                       $expect,
                       $actual);

        // TEST
        $md5val = md5('testval');
        $_POST['p2'] = $md5val;
        $actual = $this->inVarH->getPOST('p2','md5');
        $expect = $md5val;
        $this->addTest('getPOST() #2', 
                       'make sure getPOST() recieves a POST variable (simulated), with type checking',
                       $expect,
                       $actual);


    }
    
}


?>
