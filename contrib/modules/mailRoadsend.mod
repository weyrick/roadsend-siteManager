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
* Author(s): Jon Michel (pym@roadsend.com)
*
*/


/**
 * A very simple mail module class
 */
class SM_mailRoadsend extends SM_module {
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {
        
      $this->addDirective('tableBgColor','#FFFCC');
      $this->addDirective('requiredTag',false);
    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
                
        $myForm = $this->newSmartForm();
        $myForm->addDirective("requiredStar","*");
        $myForm->addDirective('requiredTag',$this->directive['requiredTag']);
        
        // add some text -- in the form so it will not show when data is verified
        $myForm->addText("<br>Hello New Site Manager User!<br><br>");
        
        
        // add a new text entity
        $myForm->add('userName','Your Name','text',false,'',array('size'=>'20','maxLength'=>'25'),0);
        
        // add a text entity and test it as a valid email
        $myForm->add('email','Email','text',false,'',undef,0);
        $myForm->addFilter('email','email','Not a valid email address',array('format'=>'user@site.com'));
        
        // add a textarea box      
        $myForm->add('message',"Message",'textArea',true,"I Love Roadsend's Site Manager because ... ",array('rows'=>'5','cols'=>'60'),0);
        
        $myForm->runForm();
        if ($myForm->dataVerified()) {
        
            $userName = $myForm->getVar('userName');
            $message = stripslashes($myForm->getVar('message'));
            $email = $myForm->getVar('email');
            
            mail("pym@roadsend.com","Hello from ".$userName,$message,"From: $email\r\nReply-to: $email\r\n");
            $this->say("thanks for the comments!!<Br><br>");

        }
        else {
            $this->say($myForm->output());
        }
        
        
    }
    
}


?>
