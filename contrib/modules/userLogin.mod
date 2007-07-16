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

/**
 * a login module for logging users into the SiteManager Member System
 *
 */
class SM_userLogin extends SM_module {
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        // logout switch
        $this->addInVar('l',0,'int');

        // turn this on to show logout notice if logged in
        $this->addDirective('showLoginStatus', false);
        // turn this on to issue a reloadPage() after successful login
        $this->addDirective('reloadOnLogin', false);

    }

    function canModuleLoad() {

        // don't load if member system isn't turned on
        return $this->sessionH->directive['useMemberSystem'];

    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
    
        if ($this->sessionH->isGuest()) {
            // create and configure the login form
            $myForm = $this->newSmartForm();

            $un = $myForm->add('userName','Username','text',true);
            $un->configure(array('size'=>'30','maxLength'=>'30'));

            $pw = $myForm->add('passWord','Password','text',true);
            $pw->configure(array('size'=>'30','maxLength'=>'30','passWord'=>true));

            // run the form
            $myForm->runForm();

            // was data verified?        
            if ($myForm->dataVerified()) {

                // attempt to login
                if ($this->sessionH->attemptLogin($myForm->getVar('userName'),$myForm->getVar('passWord'))) {
                    
                    // they logged in succesfully
                    if ($this->getDirective('reloadOnLogin')) {
                        $this->sessionH->reloadPage();
                    }
                    else {
                        $this->say("Successful Login!");
                    }

                }
                else {

                    // unsuccessful login
                    $this->say("Sorry, login was incorrect.");
                    $this->say($myForm->output("Login"));

                }

            }
            else {

                // no! output the form
                if (!$this->sessionH->isMember()) {
                    $this->say($myForm->output("Login"));
                }

            }

        }
        elseif ($this->getDirective('showLoginStatus')) {

            // if they want to logout, do so here
            if ($this->getVar('l') == 1) {
                $this->sessionH->attemptLogout();
                $this->sessionH->reloadPage();
            }

            // alrady member
            $member = $this->sessionH->getMemberData();
            $this->say("You are logged in as: ".$member['userName']);
            $this->say(" [".$this->hLink($this->sessionH->PHP_SELF.'?l=1','logout','blueViewSource').']');

        }

    }

    
}


?>
