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

/**
 * a base class for implementing modules that work with YUI:
 * http://developer.yahoo.com/yui/
 *
 */
class yuiModule extends SM_module {
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        $this->addDirective('outputInTable', false);
        $this->addDirective('outputInDiv', true);

        $this->postThinkList[] = '_doJsRequire';

        $yuiDirectives = $this->siteConfig->getSection('YUI');
        $this->configure($yuiDirectives);

    }

    /**
     * require a certain YUI javascript file
     * this makes sure it won't be listed twice
     * should be in the form: "yahoo/yahoo.js" or "dom/dom.js"
     */
    function jsRequire($fName) {

        global $YUI_JSLIST;

        if (!isset($YUI_JSLIST[$fName]))
            $YUI_JSLIST[$fName] = false;

    }

    /**
     * output the list of required javascript includes
     */
    function _doJsRequire() {
    
        global $YUI_JSLIST;

        $yuiBase = $this->directive['yuiBase'];
        foreach ($YUI_JSLIST as $jsFile => $f) {
            if (!$f) {
                $this->sessionH->jsOutput .= '<script type="text/javascript" src="'.$yuiBase.'build/'.$jsFile.'"></script>'."\n";
                $YUI_JSLIST[$jsFile] = true;
            }
        }
        
    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {

        //

    }
    
}


?>
