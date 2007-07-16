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

Debug
    Error Handler
        Custom
        DebugLog
        FatalErrorPage
        DbErrorCheck
    Global Error Functions
        SM_debugLog
        SM_fatalErrorPage
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_debug extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'Debug Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the Debug System.';

     /**
      * run by base class after base runs moduleConfig()
      */
    function T_moduleConfig() {

        // module config stuff here
        $this->addInVar('fe', 0, 'int');

    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
    
        if ($this->getVar('fe') == 1) {
            $this->fatalErrorPage("Fatal Error Page Generation Test");
        }

        $dVal = $this->inVarH->getSessionVar('SM_debug');
        $this->say("Turn SM_debug: ");
        ($dVal == 0) ? $this->saybr($this->hLink($this->sessionH->PHP_SELF.'?SM_debug=1','ON')) : $this->saybr($this->hLink($this->sessionH->PHP_SELF.'?SM_debug=0','OFF'));

        $this->say('<br />');

        $this->sayHLink($this->sessionH->PHP_SELF.'?fe=1','Generate Fatal Error Page');

    }
    
}


?>
