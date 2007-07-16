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

    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_contrib extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'Contrib Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of Contributed Code.';

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
    
        // NG MENU

        // get the menu module
        $menu = $this->loadModule('ngMenu');
        
        // add root item, return created child
        $child = $menu->addItem('Root Item');
        
        // add child (linked) to the child
        $child->addLinkItem('Child Item','test.php');
        
        // add a root item with an ID
        $menu->addItem('New Root Item','myID');
        
        // get that item back so we can add to it
        $gItem = $menu->getItem('myID');
        
        // add a child to it
        $newChild = $gItem->addItem('New Child');
        
        // add directives to the child
        $newChild->addDirective('preFix','<b>');
        $newChild->addDirective('postFix','</b>');
        
        // show the menu in this module
        $this->saybr('<b>SAMPLE MENU (using ngMenu.mod)</b>');
        $this->say($menu->run());

    }
    
}


?>
