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
$SM_siteManager->includeModule('yuiModule');

/**
 * a base class for implementing YUI panels
 *
 */
class yuiPanel extends yuiModule {

    var $panelHeader = 'XX Panel Header XX';
    var $panelFooter = '';

    var $panelBody = '';
    
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        parent::moduleConfig();
        $this->postThinkList[] = '_writePanel';

        // required js files for panel
        $this->jsRequire('yahoo/yahoo.js');
        $this->jsRequire('event/event.js');
        $this->jsRequire('dom/dom.js');
        $this->jsRequire('dragdrop/dragdrop.js');
        $this->jsRequire('container/container.js');

    }

    function panelSay($txt) {
        $this->panelBody .= $txt;
    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function _writePanel() {

        $this->say("<div class='hd'>{$this->panelHeader}</div>");
        $this->say("<div class='bd'>{$this->panelBody}</div>");
        $this->say("<div class='ft'>{$this->panelFooter}</div>");

    }
    
}


?>
