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

Member Systems
    Default
        Login
        Logout
        Timeouts
        Persistent Member
        Maintain Tables
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_members extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'Member Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the Member System.';

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
    

        $mod1 = $this->loadModule('userLogin');
        $mod1->addDirective('showLoginStatus',true);
        $this->say($mod1->run());

        $this->say("<br><br>");
    
        $dbSettings = array('tableName' => 'members',
                            'viewField' => 'userName');

        $mod2 = $this->loadModule('dbRecordSelector');
        $mod2->configure($dbSettings);

        $mod3 = $this->loadModule('dbGui');
        $mod3->configure($dbSettings);

        $memberSystem = $this->sessionH->getMemberSystem();
        $this->say($memberSystem->dumpInfo());

        $this->say($mod2->run());
        $this->say($mod3->run());

    }
    
}


?>
