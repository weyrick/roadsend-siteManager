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

GCS
    Global Config
        Getting Global Variable x
        Getting Global Section x
        Displaying Global Variables x
    Local Config
        Getting Local Variable x
        Getting Local Section
    Getting Merge Variables x
    Getting ID List
    Set Var
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_gcs extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'GCS Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the Global Configuration System.';

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
    

        global $adminDir;

        // TEST
        $actual = $this->siteConfig->getVar('dirs','testDir');
        $expect = $adminDir."test/";
        $this->addTest('getVar() #1', 
                       'Retrieve varible from GCS with siteConfig->getVar() method.',
                       $expect,
                       $actual);

        // TEST
        $actual = $this->siteConfig->getVar('dirs','testDir2');
        $expect = $this->siteConfig->getVar('dirs','testDir').'test2/';
        $this->addTest('getVar() #2 (backref 1)', 
                       'getVar using variable referencing from same section',
                       $expect,
                       $actual);
        
        // TEST
        $actual = $this->siteConfig->getVar('dirs','testDir2');
        $expect = $this->siteConfig->getVar('test','testDir');
        $this->addTest('getVar() #3 (backref 2)', 
                       'getVar using variable referencing from different section',
                       $expect,
                       $actual);

        
        // TEST
        $expect = 'PASSED';
        $actual = $this->siteConfig->getVar('test','configTest1');
        $this->addTest('getVar() #4', 
                       'getVar from a SECTION with single ID, one value',
                       $expect,
                       $actual);

        // TEST
        $expect = array('TVAL1','TVAL2');
        $actual = $this->siteConfig->getVar('test','configTest2');
        $this->addTest('getVar() #5', 
                       'getVar from a SECTION with single ID, multiple values',
                       $expect,
                       $actual);

        // TEST
        $expect = 'PASSED';
        $actual = $this->siteConfig->getVar('test2','sample',false,'primary');
        $this->addTest('getVar() #6', 
                       'getVar from a SECTION with mutliple IDs, one value, specifying SECTION ID',
                       $expect,
                       $actual);

        // TEST
        $expect = array('SAMPLE-PRIMARY','SAMPLE-SECONDARY');
        $actual = $this->siteConfig->getVar('test2','sample2');
        $this->addTest('getVar() #7', 
                       'getVar from a SECTION with mutliple IDs, one value, using all IDs',
                       $expect,
                       $actual);


        // TEST
        $expect = 'FU';
        $actual = $this->siteConfig->getVar('vTest','testGlobal');                       
        $this->addTest('getVar() w/global replacement #1',
                       'Get a variable that uses variable replacement',
                       $expect,
                       $actual);

        // TEST
        $expect = $_SERVER['SERVER_NAME'];
        $actual = $this->siteConfig->getVar('vTest','testServer');
        $this->addTest('getVar() w/global replacement #2',
                       'Get a variable that uses variable replacement',
                       $expect,
                       $actual);                       
                       
                       
        // TEST
        $expect = array('testDir' => '/var/www/testSite/admin/test/test2/',
                        'testDir2' => '/var/www/testSite/admin/test/test2/ - testDir2',
                        'testDir3' => '/var/www/testSite/admin/test/test2/ - testDir3',
                        'configTest1' => 'PASSED',
                        'configTest2' => array('TVAL1','TVAL2')
                        );
        $actual = $this->siteConfig->getSection('test');
        $this->addTest('getSection() #1', 
                       'getSection from a SECTION with single ID',
                       $expect,
                       $actual);

        // TEST
        $expect = array('sample' => 'PASSED',
                        'sample2' => 'SAMPLE-PRIMARY',
                        'sample3' => array('SAMPLE3-PRIMARY-1','SAMPLE3-PRIMARY-2'),
                        );
        $actual = $this->siteConfig->getSection('test2','primary');
        $this->addTest('getSection() #2', 
                       'getSection from a SECTION with multiple IDs, specifying SECTION ID',
                       $expect,
                       $actual);

        // TEST
        $expect = array('sample' => 'PASSED',
                        'sample2' => array('SAMPLE-PRIMARY','SAMPLE-SECONDARY'),
                        'sample3' => array('SAMPLE3-PRIMARY-1','SAMPLE3-PRIMARY-2','SAMPLE3-SECONDARY-1','SAMPLE3-SECONDARY-2'),
                        );
        $actual = $this->siteConfig->getSection('test2');
        $this->addTest('getSection() #3', 
                       'getSection from a SECTION with multiple IDs, using all IDs',
                       $expect,
                       $actual);
    
        // TEST
        $expect = $adminDir.'config/';
        $actual = $this->siteConfig->getLocalVar('dirs','config');
        $this->addTest('getLocalVar()',
                       'getLocalVar on single value that exists in both globalConfig.xsm and localConfig.xsm',
                       $expect,
                       $actual);

        // TEST
        $expect = array('allowExtendedTags'=>true, 'testSection1'=>'PASS', 'testSection2'=>'PASS');
        $actual = $this->siteConfig->getLocalSection('templates');
        $this->addTest('getLocalSection()',
                       'getLocalSection on single section that exists in both globalConfig.xsm and localConfig.xsm',
                       $expect,
                       $actual);


        // TEST
        $expect = $this->siteConfig->getGlobalVar('dirs','smRoot').'config/';
        $actual = $this->siteConfig->getGlobalVar('dirs','config');
        $this->addTest('getGlobalVar()',
                       'getGlobalVar on single value that exists in both globalConfig.xsm and localConfig.xsm',
                       $expect,
                       $actual);

        // TEST
        $expect = array('allowExtendedTags'=>false, 'allowQuickTags'=>true, 'quickTagIdentifier'=>'@@');
        $actual = $this->siteConfig->getGlobalSection('templates');
        $this->addTest('getGlobalSection()',
                       'getGlobalSection on single section that exists in both globalConfig.xsm and localConfig.xsm',
                       $expect,
                       $actual);

        // TEST
        $expect = true;
        $actual = ($this->siteConfig->dumpInfo() != '');
        $this->addTest('dumpInfo()',
                       'Determine if dumpInfo() is returning results',
                       $expect,
                       $actual);

        // TEST
        $expect = array('0'=>'default','2'=>'secondary');
        $actual = $this->siteConfig->getIDlist('db');
        $this->addTest('getIDlist()',
                       'Return ID list of sections',
                       $expect,
                       $actual);

                       
    }
    
}


?>
