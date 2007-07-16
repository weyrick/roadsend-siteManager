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
*
*
+ 11/24/2002 Added compatibility with pgsql database, Eric Andersen
*
*/

/**
 * Use this module as a base or example for adding and editing users 
 * in the member system. It knows automatically whether a user is
 * currently logged in or not, and creates a form for either editing
 * or adding accordingly.
 *
 */
class memberEdit extends SM_module {
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        // directives
        $this->addDirective('maxUserNameLen',30);
        $this->addDirective('minUserNameLen',3);

        $this->addDirective('maxPassWordLen',30);
        $this->addDirective('minPassWordLen',6);

        // check for a proper connection
        if (!isset($this->dbH)) {
            SM_fatalErrorPage("this module requires a database connection, which wasn't present!");
        }

    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
    
        // get some global config'd vars
        global $SM_siteName;

        // use these later
        $memberTable   = $this->siteConfig->getVar('memberSystem','memberTable');
        $userNameField = $this->siteConfig->getVar('memberSystem','userNameField');
        $passWordField = $this->siteConfig->getVar('memberSystem','passWordField');

        // edit or add?
        if ($this->sessionH->isGuest())
            $myMember = array();
        else
            $myMember = $this->sessionH->getMemberData();

        if ($this->sessionH->isMember())
            $this->saybr("<center><b>Modify Member Profile<br></center>");
        else
            $this->saybr("<center><b>New Member Profile<br></center>");

        // create form                  
        $myForm = $this->newSmartForm();       

        $myForm->addDirective("resetButton","Reset Form");
        $myForm->addDirective('tableCellPadding','3');

        //  Text input boxes                 
        if ($this->sessionH->isMember()) {
            $myForm->add('userName','User Name','presetText',false,$myMember['userName'],array('style'=>'<B><I>'));
        } else {
            $myForm->add('userName','Choose a Username','text',true,'',array('size'=>'20','maxLength'=>$this->directive['maxUserNameLen']));
            $myForm->addFilter('userName','preg','Invalid username',array('pattern'=>"/^\w{".$this->directive['minUserNameLen'].",".$this->directive['maxUserNameLen']."}\$/"));
            $myForm->addFilter('userName','dbUnique','Sorry, this username is already in use',array('tableName'=>$memberTable,'checkField'=>$userNameField));
        }
        
        $myForm->add('passWord','Password','text',true,$myMember['passWord'],array('size'=>'20','maxLength'=>'30','passWord'=>true));
        $myForm->addFilter('passWord','preg','Invalid password',array('pattern'=>"/^\w{".$this->directive['minPassWordLen'].",".$this->directive['maxPassWordLen']."}\$/"));
        $myForm->addFilter('passWord','varCompare',"Your password cannot be the same as your username",array('cType'=>'notEqual','compVar'=>'userName'));
        
        $myForm->add('passWordConfirm','Re-enter Password','text',true,$myMember['passWord'],array('size'=>'20','maxLength'=>'30','passWord'=>true));
        $myForm->addFilter('passWordConfirm','varCompare',"The passwords you entered did not match",array('compVar'=>'passWord'));
      
        $myForm->add('emailAddress','Current Email Address','text',true,$myMember['emailAddress'],array('size'=>'20','maxLength'=>'200'));
        $myForm->addFilter('emailAddress','email','Invalid Email Address');     
        $f = $myForm->addFilter('emailAddress','dbUnique','Sorry, this email address is already in use by another member profile',array('tableName'=>$memberTable, 'checkField'=>'emailAddress'));
        
        if ($this->sessionH->isMember()) {
            // if already a member, dont check unique against their member entry
            //  $f->addDirective('whereClause' => 'idxNum != '.$myMember['idxNum']);
        }
        
        $myForm->add('firstName','First Name','text',true,$myMember['firstName'],array('size'=>'20','maxLength'=>'60'));
        $myForm->add('lastName','Last Name','text',true,$myMember['lastName'],array('size'=>'20','maxLength'=>'60'));

        // run the form
        $myForm->runForm();
        
        // *** DATA VERIFIED ***
        if ($myForm->dataVerified()) {                       
    
                $dbType = $this->siteConfig->getVar('db','dbType');

                if ($dbType == 'mysql') {
                    // MODIFY MEMBER
                    if ($this->sessionH->isMember()) {
                        $SQL = "UPDATE $memberTable SET 
                                        $passWordField='".$myForm->getVar('passWord')."', 
                                        emailAddress='".$myForm->getVar('emailAddress')."',
                                        firstName='".$myForm->getVar('firstName')."', 
                                        lastName='".$myForm->getVar('lastName')."'
                                        WHERE idxNum = " .$myMember['idxNum'];
                    } else {

                        // generate a unique global member ID
                        $globalID = md5(uniqid(rand(),true));

                        // INSERT MEMBER 
                        $SQL = "INSERT INTO $memberTable SET 
                                    uID = '$globalID',
                                    $userNameField='".$myForm->getVar('userName')."', 
                                    $passWordField='".$myForm->getVar('passWord')."', 
                                    emailAddress='".$myForm->getVar('emailAddress')."',
                                    firstName='".$myForm->getVar('firstName')."', 
                                    lastName='".$myForm->getVar('lastName')."',
                                    dateCreated=NOW()";

                    }
                }
                elseif ($dbType == 'pgsql') {

                    if ($this->sessionH->isMember()) {
    
                         $SQL = "UPDATE \"$memberTable\" SET 
                         \"$passWordField\"='".$myForm->getVar('passWord')."', 
                         \"emailAddress\"='".$myForm->getVar('emailAddress')."',
                         \"firstName\"='".$myForm->getVar('firstName')."', 
                         \"lastName\"='".$myForm->getVar('lastName')."'
                         WHERE \"idxNum\" = " .$myMember['idxNum'].";";

                    } else {

                        // generate a unique global member ID
                        $globalID = md5(uniqid(rand()));

                        $SQL = "INSERT INTO \"$memberTable\" 
                        (\"uID\",\"$userNameField\",\"$passWordField\",\"emailAddress\",\"firstName\",\"lastName\",\"dateCreated\")
                        VALUES ( 
                        '$globalID',
                        '".$myForm->getVar('userName')."', 
                        '".$myForm->getVar('passWord')."', 
                        '".$myForm->getVar('emailAddress')."',
                        '".$myForm->getVar('firstName')."', 
                        '".$myForm->getVar('lastName')."',
                        NOW());";

                    }

                }
                else {
                    $this->fatalErrorPage('Sorry, this database is not supported.');
                }

                $rh = $this->dbH->query($SQL);
                if (empty($rh)) {
                    SM_fatalErrorPage("unable to access database",$this);
                }

                // end page
                if ($this->sessionH->isMember()) {

                    // this was an update...
                    $this->say("Profile updated.");

                    // need to flush member info!
                    $this->sessionH->flushMemberInfo();

                }
                else {
                    // new member
                    $this->say("Profile created. Thank you for becoming a member of ".$SM_siteName."!");
                }

        } // end data verified
        else {
            // data not verified, show form
            $this->say($myForm->output("Submit"));
        }


    }
    
}


?>
