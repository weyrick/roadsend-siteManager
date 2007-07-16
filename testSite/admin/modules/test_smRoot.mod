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

SM Root
    Loading Resources
    Including Resources
    Finding Resources
    Starting Database Subsystem
    Starting Session Subsystem
    Complete Page
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_smRoot extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'SM Root Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the SM Root.';

     /**
      * run by base class after base runs moduleConfig()
      */
    function T_moduleConfig() {

        // module config stuff here

        $this->addOutVar('testPVar');

    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
    
        global $SM_siteManager;

        // load template
        $tpt = $SM_siteManager->loadTemplate('test');
        $tpt->addText('(sample text added to area)','areaOne');
        $actual = $tpt->run();
        $expect = '<b>Sample Template</b><br>
(sample text added to area)
<br>
Template Test Complete
';
        $this->addTest('loadTemplate()', 
                       'Load, parse and run template from file through SM_siteManager root object',
                       $expect,
                       $actual);

        // include module
        $actual = $SM_siteManager->includeModule("rawHTML");
        $expect = true;
        $this->addTest('includeModule()', 
                       'Include a module definition without instantiation',
                       $expect,
                       $actual);


        // false include module
        $actual = $SM_siteManager->includeModule("nonexistant-module", false);
        $expect = false;
        $this->addTest('includeModule()', 
                       'Include a fake module with fatalFNF false',
                       $expect,
                       $actual);

        // load module
        $mod = $SM_siteManager->loadModule('rawHTML');
        $mod->addDirective('cleanOutput', true);
        $mod->addDirective('output','sample module output');
        $actual = $mod->run();
        $expect = 'sample module output';
        $this->addTest('loadModule()', 
                       'Load and run a module from SM_siteManager root object',
                       $expect,
                       $actual);

        // include code plate
        $actual = $SM_siteManager->includeCodePlate("testCodePlate");
        $expect = true;
        $this->addTest('includeCodePlate()', 
                       'Include a codePlates definition without instantiation',
                       $expect,
                       $actual);

        // false include code plate
        $actual = $SM_siteManager->includeCodePlate("testCodePlate-nonexistant", false);
        $expect = false;
        $this->addTest('includeCodePlate()', 
                       'Include a fake codePlate with fatalFNF false',
                       $expect,
                       $actual);


        // load code plate
        $cpt = $SM_siteManager->loadCodePlate("testCodePlate");
        $cpt->addText('<b>Codeplate Test</b><br>', 'areaOne');
        $actual = $cpt->run();
        $expect = '<b>Sample Template</b><br>
<b>Codeplate Test</b><br><span id="SM5">
<table width="100%" border="0" cellspacing="0" cellpadding="0"><tbody>
<tr>
<td><B>Hello World</B></td></tr></tbody>
</table>
</span>

<br>
Template Test Complete
';
        $this->addTest('loadCodePlate()', 
                       'Load a CodePlate, add some text from SM_siteManager root object',
                       $expect,
                       $actual);



        // include lib
        $SM_siteManager->includeLib("testLibrary");
        $actual = testLibrary();
        $expect = 'LIBRARY LOADED';
        $this->addTest('includeLibrary()', 
                       'Include a library file',
                       $expect,
                       $actual);

        // isInVarP
        $actual = $SM_siteManager->isInVarP('testPVar');
        $expect = true;
        $this->addTest('isInVarP()', 
                       'Test for propper presence of a persistent variable added in this module',
                       $expect,
                       $actual);


        // find SM file
        $actual = $SM_siteManager->findSMfile('testFindFile','myDir','foo',false);
        $expect = $this->siteConfig->getVar('dirs','myDir').'/testFindFile.foo';
        $this->addTest('findSMfile()', 
                       'Test findSMfile method, for searching for files in the GCS dirs',
                       $expect,
                       $actual);

        // define DB connection
        $dbSettings['DSN'] = $this->siteConfig->getVar('db','DSN',false,'default');
        var_dump($dbSettings['DSN']);
        $actual = $SM_siteManager->defineDBconnection('newDB',$dbSettings,false);
        $expect = true;
        $this->addTest('defineDBconnection()', 
                       'Test defining a new database connection on the fly',
                       $expect,
                       $actual);
    
    }
    
}


?>
