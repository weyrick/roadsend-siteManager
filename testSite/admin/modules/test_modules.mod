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

Modules
    InVars
        Propagating
        Getting
        Setting
    OutVars
    ThinkLists
    ConfigList
    Output Directives
    Loading SiteManager Resources
        Templates
        Modules
        CodePlates
    CanModuleLoad
    Wrapper Methods
        HLink
        ULink
    SmartForms
    Debug
        dumpInfo    
    Extending
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');


/**
 * extend testBase for testing sitemanager functionality
 */
class test_modules extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'Module Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the Module System.';

     /**
      * run by base class after base runs moduleConfig()
      */
    function T_moduleConfig() {

        // module config stuff here

        // set invars
        $this->addInVar('modVar1',NULL,'string',true);

        // think lists
        $this->preThinkList[] = 'modulePreThink';
        $this->postThinkList[] = 'modulePostThink';
        $this->postConfigList[] = 'modulePostConfig';

    }


    function eventBadInVar($var='', $wantedType='') {

        $actual = $var;
        $expect = 'modVar2';
        $this->addTest('getVar() / eventBadInVar()', 
                       'test eventBadInVar, called when an inVar failed its type check upon access',
                       $expect,
                       $actual);

    }


    function canModuleLoad() {

        return true;

    }

    function modulePostConfig() {

        $this->addInVar('modVar2',NULL,'md5',true);

    }

    function modulePreThink() {

        $this->say('TOP OF MODULE');

    }
    
    function modulePostThink() {

        $this->say('BOTTOM OF MODULE');

    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
    

        // get var
        $actual = $this->getVar('modVar1');
        $expect = 'pokey';
        $this->addTest('getVar()', 
                       'test getVar() retrieval of inVar for module',
                       $expect,
                       $actual);

        $modVar2 = $this->getVar('modVar2');  // will run eventBadInVar


        // load module
        $mod = $this->loadModule('rawHTML');
        $mod->addDirective('output','fufu');
        $mod->addDirective('outputWrapper',SM_module::WRAP_NONE);
        $actual = $mod->run();
        $expect = 'fufu';
        $this->addTest('loadModule', 
                       'load module for use in this module',
                       $expect,
                       $actual);

        // load template
        $tpt = $this->loadTemplate('test');
        $tpt->addText('moo','areaOne');
        $actual = $tpt->run();
        $expect = '<b>Sample Template</b><br>
moo
<br>
Template Test Complete

';
        $this->addTest('loadTemplate', 
                       'load template for use in this module',
                       $expect,
                       $actual);


        // hlink
        $actual = $this->hLink('test.php?u=1','Test Link','myClass','extra="extra"',array('exclude'));
//        $expect = '<a href="test.php?u=1&sID='.$this->sessionH->getSessionID().'&modVar1=pokey&modVar2=5" class="myClass" extra="extra">Test Link</a>';
        $expect = '<a href="test.php?u=1&modVar1=pokey&modVar2=5" class="myClass" extra="extra">Test Link</a>';
        $this->addTest('hLink()', 
                       'test hLink method for creating links that maintain session',
                       $expect,
                       $actual);
    
        // debug
        $this->debugLog("This is a sample debug log message, visible when SM_debug is turned on");

        // smartform
        $form = $this->newSmartForm();
        $form->add('var','Name','text');
        $this->say($form->output('go'));


    }
    
}


?>
