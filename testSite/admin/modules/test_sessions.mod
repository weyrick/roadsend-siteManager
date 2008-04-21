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

Sessions
    Persistents
        Getting
        Settings
    Session Containers
        Database
        File
        None
    Keeping Sessions Active
        Hlink
        Ulink
        PUlink
        formID
        reloadPage
        transferToPage
    Member Subsystem Access
    Session Clearing
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_sessions extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'Session Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the Session System.';

     /**
      * run by base class after base runs moduleConfig()
      */
    function T_moduleConfig() {

        // module config stuff here
        $this->addInVar('cs', 0, 'int');    // clear session

    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {

        // clear session
        if ($this->getVar('cs') == 1) {
                $this->sessionH->clearSession();
                $this->saybr('Session was cleared.');
        }
        else {
            $this->saybr("To clear the current session, ".$this->hLink($this->sessionH->PHP_SELF.'?cs=1','click here').".");
        }
    
        $this->say("<Br />");


        //////////////


        // set persistent
        // get persistent
        $val = $this->sessionH->getSessionVar('testPersistent');
        if ($val != 'bar' && $this->sessionH->hasCookies) {
            $this->sessionH->setSessionVar('testPersistent','bar');
            $this->sessionH->reloadPage();
        }

        $actual = $val;
        $expect = 'bar';
        $this->addTest('Persistent Variables', 
                       'test getting/setting persistent variables',
                       $expect,
                       $actual);


        // hlink
        $actual = $this->sessionH->hLink('test.php?u=1','Test Link','myClass','extra="extra"',array('exclude'));
        //$expect = '<a href="test.php?u=1&sID='.$this->sessionH->getSessionID().'" class="myClass" extra="extra">Test Link</a>';
        $expect = '<a href="test.php?u=1" class="myClass" extra="extra">Test Link</a>';
        $this->addTest('hLink()', 
                       'test hLink method for creating links that maintain session',
                       $expect,
                       $actual);


        // reloadPage


        // transferToPage


        //////////////


        // session info
        $sInfo = $this->sessionH->dumpInfo();
        $this->saybr($sInfo);

    }
    
}


?>
