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
* Author(s): xxx
*
*/

/**
 * a skeleton module. extends base SM_module class.
 *
 */
class MODNAME extends SM_module {
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        // * Directives
        // use these to allow configuration of your modules
        //$this->addDirective('tableBgColor','#FFFFCC');

        // you can also load your directives from an XML file
        //$this->loadConfig('myDirectives');

        // * InVars
        // use these to allow your module to access variables passed to the page
        //$this->addInVar('testVariable','defaultValue');

        // you can specify that an inVar should be a certain variable type
        //$this->addInVar('testVar2',0,'integer');

        // if you're going to use a module or a template in moduleThink(), declare
        // it as a resource. see moduleThink() for examples of retrieving these
        // resources
        // $mod1 = $this->useModule('modID','moduleName');
        // $tmp1 = $this->useTemplate('tmpID','templateName');

        // * Styles
        // use these to allow your module to use Cascading Style Sheets for output
        //$this->addStyle('blueText');        

        // * Database Access
        // if this module requires database functionality, 
        // check for a proper connection
        //if (!isset($this->dbH)) {
        //    SM_fatalErrorPage("this module requires a database connection, which wasn't present!");
        //}

    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
    
        // you can run any valid PHP code here
        // use $this->say or $this->saybr to output text

        // if you're using sessions, use hLink to make a link somewhere
        // this will keep your session variables in tact
        //$this->say($this->hLink('newPage.php','Link To New Page'));

        // you can use your declared resources here
        // $mod1 = $this->getResource('modID');
        // $this->say($mod1->run());
        // $tmp1 = $this->getResource('tmpID');
        // $tmp1->addText('areaOne','Sample Text');
        // $this->say($tmp1->run());

        // Simple output of text
        $this->saybr("Hello, World. (".get_class($this).")");

    }
    
}


?>
