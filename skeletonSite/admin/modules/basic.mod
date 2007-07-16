<?php

/*********************************************************************
*  <project>
*  Copyright (c) 2001-2003 <company name>
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
class basic extends SM_module {
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        // * Directives
        // use these to allow configuration of your modules
        //$this->addDirective('tableBgColor','#FFFFCC');

        // this directive is set in the skeleton site in the 
        // directive script home/index.php
        $this->addDirective('testDirective');

        // you can also load your directives from an XML file
        //$this->loadConfig('myDirectives');

        // if you're going to use a module or a template in moduleThink(), declare
        // it as a resource. see moduleThink() for examples of retrieving these
        // resources
        // $mod1 = $this->useModule('modID','moduleName');
        // $tmp1 = $this->useTemplate('tmpID','templateName');

        // * InVars
        // use these to allow your module to access variables passed to the page
        //$this->addInVar('testVariable','defaultValue');

        // you can specify that an inVar should be a certain variable type
        //$this->addInVar('testVar2',0,'integer');

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

        // here is the code for a simple smartform
        $myForm = $this->newSmartForm();
        $textBox = $myForm->add('sample','Sample Form','text',true,'sample value');
        $textBox->addDirective('size','35');
        $textBox->addDirective('maxLength','100');
        $textBox->addFilter('number','Must be a number!');
        $myForm->run();
        if ($myForm->dataVerified()) {
            $this->saybr("Looks like you entered a number: ".$myForm->getVar('sample'));
        }
        else {
            $this->saybr($myForm->output('Go!'));
        }

        // Simple output of text
        $this->saybr("Hello, World. (".get_class($this).")");

        // Simple access of directives
        $this->saybr("Test Directive Value: {$this->directive['testDirective']}");

    }
    
}


?>
