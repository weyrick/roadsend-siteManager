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

/**
 * extend testBase for testing sitemanager functionality
 */
class test_yuiModules extends SM_module {
     
    /** title output at top of test module */
    var $testTitle = 'YUI Module Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the Module System, using the Yahoo UI';

     /**
      * run by base class after base runs moduleConfig()
      */
    function moduleConfig() {

        $this->addDirective('cleanOutput',true);

    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {

        // this could be generated
        $this->sayJS('
         <script>
            YAHOO.namespace("example.container");

            function init() {
                // Instantiate a Panel from markup
                YAHOO.example.container.panel1 = new YAHOO.widget.Panel("panel1", { width:"300px", visible:false, constraintoviewport:true } );
                YAHOO.example.container.panel1.render();
                
                YAHOO.example.container.panel2 = new YAHOO.widget.Panel("panel2", { width:"300px", visible:false, constraintoviewport:true } );
                YAHOO.example.container.panel2.render();

                YAHOO.util.Event.addListener("show1", "click", YAHOO.example.container.panel1.show, YAHOO.example.container.panel1, true);
                YAHOO.util.Event.addListener("hide1", "click", YAHOO.example.container.panel1.hide, YAHOO.example.container.panel1, true);
                
                YAHOO.util.Event.addListener("show2", "click", YAHOO.example.container.panel2.show, YAHOO.example.container.panel2, true);
                YAHOO.util.Event.addListener("hide2", "click", YAHOO.example.container.panel2.hide, YAHOO.example.container.panel2, true);

            }

            YAHOO.util.Event.addListener(window, "load", init);
         </script>
        ');

        // method of access. also could be generated
        $this->say('
        <div>
            <button id="show1">Show panel 1</button>
            <button id="hide1">Hide panel 1</button>
            <button id="show2">Show panel 2</button>
            <button id="hide2">Hide panel 2</button>
        </div>
        ');

        $panel = $this->loadModule('sm_yuiPanel');
        $panel->moduleID = 'panel1';
        $panel->panelSay('This goes in the first module<br>');
        $this->say($panel->run());
        
        $panel2 = $this->loadModule('sm_yuiPanel');
        $panel2->moduleID = 'panel2';
        $panel2->panelSay('This goes in the second module - whee!<br>');
        $this->say($panel2->run());

    }
    
}


?>