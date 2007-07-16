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

Components
    Registering
    Loading        
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_components extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'Component Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the Component System.';

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
        SM_registerComponent('testComponent', $adminDir.'lib/testComponent.inc');
        $actual = SM_isComponentRegistered('testComponent');
        $expect = true;
        $this->addTest('SM_registerComponent(), SM_isComponentRegistered()', 
                       'make sure a component can be registered',
                       $expect,
                       $actual);

        // TEST
        SM_loadComponent('testComponent');
        if (SM_isComponentLoaded('testComponent')) {
            $actual = componentLoaded();
        }
        else {
            $actual = '(component was not loaded)';
        }
        $expect = 'COMPONENT LOADED';
        $this->addTest('SM_isComponentLoaded(), SM_loadComponent()', 
                       'make sure a registered component is identifiable and loadable',
                       $expect,
                       $actual);
    
    }
    
}


?>
