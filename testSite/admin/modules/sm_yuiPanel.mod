<?php

/*********************************************************************
*  Roadsend SiteManager
*  Copyright (c) 2001-2007 Roadsend, Inc.(http://www.roadsend.com)
**********************************************************************
*
* This source file is subject to version 1.0 of the Roadsend Public
* License, that is bundled with this package in the file 
* LICENSE, and is available through the world wide web at 
* http://www.roadsend.com/license/rpl1.txt
*
**********************************************************************
* Author(s): Shannon Weyrick
*
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('yuiPanel');

/**
 * a base class for implementing YUI panels
 *
 */
class sm_yuiPanel extends yuiPanel {

    var $panelHeader = 'SiteManager Test Panel';
    var $panelFooter = '-- welcome to the future --';
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        parent::moduleConfig();
        
        // this can (should?) be set by caller
        $this->moduleID = 'panel1';

    }

    function moduleThink() {

        $this->panelSay('This is the output of the panel body');
    
    }
    
}


?>