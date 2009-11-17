<?php

/*********************************************************************
*  Roadsend SiteManager
*  Copyright (c) 2001-2009 Roadsend, Inc.(http://www.roadsend.com)
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

SmartForms
    Output Directives
        Table Directives
        Required Info        
    Input Entities
        Filtered
        JavaScript
    Filters
        JavaScript
    XML Load
    Layout Templates
    Styles
    Entity Grouping
        Setting
    Headers & Footers
    Manipulating
        Entities
            Adding
            Removing
            Set Default Value
        Filters
            Adding
            Removing        
    Hiddens
    Retrieving Information
        Number of entities
        Entity variable name list
        Entity object
        Entity type by name
    Debug
        dumpFormVars
        dumpInfo
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_smartForms extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'SmartForms Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of SmartForms.';

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
    
        // create the form
        $myForm = $this->newSmartForm('testForm');

        $myForm->addDirective('badFormMessage','<center><b><br>There is a problem with the form input. Please correct your input and try again.</b><br><br></center>');

        if ($this->getVar('loadXML') != 1) {        
        
        $myForm->addDirective('cleanHiddens',true);

        
        // submit image
        /*
        if ($this->getVar('useTemplate') !=1 ) {        
            $myForm->addDirective('submitImage','pointR.gif');
            //$myForm->addDirective('resetImage','pointR.gif');
        }
        */
        


        // dump template
        $myForm->addDirective('dumpTemplate',true);

        // misc directives
        $myForm->addDirective('resetButton','Reset');
        $myForm->addDirective('tableBorder','1');
                
        // default entity styles
        //$myForm->addDirective('entityClassTag','testStyle');

        // extra submit button right on top
        $myForm->add('submit1','','submit');
        $myForm->setArgs('submit1',array('value'=>'Submit Form'));
        
        $myForm->add('submit2','','submit');
        //$myForm->setArgs('submit2',array('image'=>true,'src'=>'pointR.gif'));

        // add a regular text entity. this uses most of the (optional) parameters of the add() method
        $ent = $myForm->add('userName','User Name','text',true,'start name',array('size'=>'50','maxLength'=>'100'));        
        $ent->addFrobber('trim');
        $ent->addFrobber('toLatin1');
        $ent->addDirective('entityClassTag','foo');
        
        // some simpler examples
        $myForm->add('firstName','First Name','text',false);
        $myForm->add('lastName','Last Name','text',false);
        
        if ($this->getVar('useTemplate') != 1) {        

        // adding userName, firstName and lastName to the same group will mean they
        // will be considered one "row" as far as the alternating row colors are
        // concerned (ie, they will have the same background color)
        $myForm->defineFieldset(array('userName','firstName','lastName'),'nameInfo');
        
        // add email address. returns a reference to the input entity
        $eA = $myForm->add('emailAddress','Email Address','text',false);
        // add a filter to that entity i just added
        $eA->addFilter('email','Bad Email Address');        
        
        // set class
        $eA->addDirective('entityAttributeClassTag','sfNormal1');

        // text area example
        $ent = $myForm->add('desc','Description','textArea',true);
        $ent->addFrobber('toLatin1');
        
        // phone number, with filter
        $myForm->add('phone','Phone Number','text',false);
        $myForm->addFilter('phone','phone','Not a valid phone number',array('format'=>'(XXX) XXX-XXXX'));

        // password is just a textEntity with passWord directive set to true
        $myForm->add('passWord','Password','text',false,'',array('passWord'=>true));

        // zipcode with filter
        $zipCode = $myForm->add('zipCode','Zip Code','text',true,'',array('size'=>'5','maxLength'=>'5'));
        $zipCode->addFilter('number','The zipcode appears to be invalid');
        $zipCode->setFilterArgs('number',array('length'=>'5'));

        // price with filters
        $price = $myForm->add('price','Price','text');
        $price->addFilter('number','Bad price');
        $price->setFilterArgs('number',array('decimal'=>true));
        
        // date entity examples
        $myForm->add('birthDate','Birth Date','date',true,'',array('yearSelect'=>false));
        $myForm->setVarFieldset('birthDate','dateInfo');
        $myForm->add('otherDate','Other Date','date',false,'',array('yearSelect'=>true));
        $myForm->setVarFieldset('otherDate','dateInfo');
        // test entityNewValue
        $myForm->setDefaultValue('otherDate','2009-5-12');
        
        // select box
        $thisSelect = $myForm->add('sex','Sex','select',true,'',array('multiple'=>true));
        $thisSelect->addOption("Male","Male");
        $thisSelect->addOption("Female","Female");
        $thisSelect->addOption("Something Inbetween","NULL");
        

        // select box 2
        $thisSelect = $myForm->add('selectTest','Select Test','select',true);
        $thisSelect->addOption("$1.00 Test Option 1","testVal");
        $thisSelect->addOption("$2.00 Test Option 2",'testVal2');
        $thisSelect->addOption("Test Option 3","testVal");
        
        // check box
        $thisCheckBox = $myForm->add('ccType','Credit Card Type','checkBox',false,'',array('optionBreak'=>true));
        $thisCheckBox->addOption("Visa","visa");
        $thisCheckBox->addOption("Discover","discover");
        $thisCheckBox->addOption("American Express","amex");
        $thisCheckBox->addOption("MasterCard","mc");
        $thisCheckBox->addDirective('optionBreak',false);
        
        // use the card type variable from above in this credit card filter
        $ccNum = $myForm->add('ccNum','Credit Card Number','text',false);
        $ccNum->addFilter('creditCard','Invalid credit card number');
        $ccNum->setFilterArgs('creditCard',array('cardVar'=>'ccType'));
        
        // group the card type and number together
        $myForm->defineFieldset(array('ccNum','ccType'),'creditCard');
        
        // radio box
        $thisRadio = $myForm->add('radio','Radio Buttons','radio',true);
        $thisRadio->addOption("Choice 1","1");
        $thisRadio->addOption("Choice 2","2");
        
        // combo box
        $cBox = $myForm->add('fType','Type','comboBox');
        $cBox->addOption("Type 1",'t1');      
        $cBox->addOption("Type 2",'t2');
        $cBox->addOption("Type 3",'t3');      
        
        // state box
        $myForm->add('state','State','stateList');

        // country box
        $myForm->add('country','Country','countryList');
        
        // radio grid
        $rg = $myForm->add('rGrid','','surveyGrid');
        $rg->addDirective('header','Please answer the following questions');
        //$rg->addDirective('inputType','checkBox');
        $rg->addResponse(array('Excellent'=>5,'Very Good'=>4,'Good'=>3,'Average'=>2,'Poor'=>1));
        $rg->addQuestion('How was your meal?','meal',true);
        $rg->addQuestion('How was your waitor?','waitor',true);
        $rg->addQuestion('Rate the beverage','beverage',true);        

        // entity grid
        $eg = $myForm->add('eGrid','','entityGrid');

        $eg->addColumn('tVar', 'Text Column', 'text');
        $eg->addColumn('cVar', 'Check Column', 'checkBox');
        $eg->addColumn('sVar', 'Submit Column', 'submit', array('value'=>'Go!!'));

        for ($i=0; $i<5; $i++) {
            $eg->addRow($i);
            $eg->addRowEntity($i, 'tVar', 'Default', true);
            $check = $eg->addRowEntity($i, 'cVar', '', false);
            $check->addOption('Test Option');
            $eg->addRowEntity($i, 'sVar', '', false);
        }

        // pool select
        $p = $myForm->add('pool','Pool Select','poolSelect');
        $p->createEntities();
        $poolList = array('zuba'=>99,'one'=>1,'two'=>2,'three'=>3);
        $selectList = array('red'=>10,'blue'=>11,'black'=>12);
        $p->setPoolList($poolList);
        $p->setSelectedList($selectList);

        //$myForm->add('reset','','reset');
        //$myForm->setArgs('reset',array('image'=>true,'src'=>'pointR.gif'));

        } // use template

        } // load xml

        //
        // apply the form 
        //
        $myForm->runForm();
                
        //
        // verify data, if good, do sql or email, or whatever you'd like with your data
        //
        if ($myForm->dataVerified()) {
        
            $this->say("data was verified:<br><br>");
        
            $this->say($myForm->dumpFormVars());

            $this->say("<br>variables from form:<br><br>");
            $this->saybr(join("<br>",$myForm->getVarList()));
        
            if (isset($eg)) {
                // do entity grid output
                $rowList = $eg->getRowIDList();
                $varList = $eg->getVarList();
                foreach ($rowList as $rowID) {
                    foreach ($varList as $var) {
                        $this->saybr("row $rowID, var $var is: ".$eg->getVar($rowID, $var));
                    }
                }

                // check submit button
                $sButton = $eg->getSubmitButton();
                if ($sButton != NULL) {                
                    $this->saybr("Submit button pressed in row {$sButton['rowID']} in column {$sButton['colID']}, and had a value of {$sButton['value']}");
                }
            }

            $this->saybr('pool select list:');
            $this->saybr('added left:');
            $this->saybr($p->getAddedLeft());
            $this->saybr('removed left:');
            $this->saybr($p->getRemovedLeft());
            $this->saybr('added right:');
            $this->saybr($p->getAddedRight());
            $this->saybr('removed right:');
            $this->saybr($p->getRemovedRight());
            $this->saybr('new left:');
            $this->saybr($p->getNewLeft());
            $this->saybr('new right:');
            $this->saybr($p->getNewRight());
            
        }
        else {

            //
            // fall through, data wasn't verified, show form
            //
            
            // output the form 
            $this->say($myForm->output('Go',array('testHidden2'=>'testval')));
        }

    
    }
    
}


?>
