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
* Author(s): gollum, weyrick
*
*
* Change Log
*
*       10/24/2001 - created. Based of of baseDbEditor.mod  (gollum)
*
*       11/20/2001 - Modified to use the new 'includeLib' function to 
*                    load the actual action classes. (gollum)
*
*       12/12/2001 - Modified to use the useSmartForm and getResource
*                    functions insted of keeping a class var for the 
*                    smartform. (gollum)
*
*       05/15/2002 - move actions to hash, keyed by action typeName (weyrick)
*                  - allow adding of directives to actions (weyrick)
*
*       01/12/2004 - Cleaned up problems with refrences. Removed unused
*                   commented out code. Changed SM_dbErrorCheck to
*                   $this->dbErrorCheck, etc. (gollum)
*
*/

//Include the file that actually defins the actions we want.
global $SM_siteManager;
$SM_siteManager->includeLib('mainDbEditorActions.inc');

/**
 * a base module for creating editiable database-based smartForms
 *
 */
class SM_abstractDbEditor extends SM_module {
     
    /** (array: objects) keep track of our action objects **/
    var $actions = array();

    /** keep track of the name of the uploaded file (if any) **/
    var $fileName = NULL;

    /** local var based on inVar **/
    var $rNum = NULL;

    /** local var based on inVar **/
    var $sType = NULL;
                                                                       
    /**
     * configure the module. run before moduleThink
     */
    function moduleConfig() {

        // directives       
        $this->addDirective('tableName','');                    // table name to operate on
        $this->addDirective('dataField','idxNum');              // field to operate with (primary key)
        $this->addDirective('smartFormDirectives','');          // Directives to be passed to the smartForm
        $this->addDirective('dataBaseID','default');            // set which database handle to use
        $this->addDirective('showSubmit',true);                 //should we show the submit button?
                
        // handle file uploads
        $this->addDirective('fileUpload','');           // set up file upload - name of field as directive       
        $this->addDirective('path',''); 
        $this->addDirective('maxUploadSize',20000);
        
        // default action / record var names
        $this->addDirective('rNumVar','rNum');
        $this->addDirective('actionVar','sType');
        $this->addDirective('sucessFormHeader',"THE DATABASE HAS BEEN UPDATED");
        // incoming record indicator. shared with recordSelectorModule
        $this->addInVar('dbu_'.$this->moduleID);        // 'database has been updated' flag

        $this->useSmartForm("myForm");

        // must include function to define a form
        $this->postConfigList[] = 'setInVars';
        $this->preThinkList[] = 'defineForm';

    }

    /**
     * set invars for rNum and sType, once we give them a chance to override their names
     */
    function setInVars() {

        // add var names
        $this->addInVar($this->directive['rNumVar'],0,$this->getDirective('rNumType'));
        $this->addInVar($this->directive['actionVar'],'add','string');

        // add our "default" types
        $this->addDefaultTypes();

        //
        $rNum = $this->getVar($this->directive['rNumVar']);
        if(!empty($rNum)) {
            $this->rNum = $rNum;
        }

        $sType = $this->getVar($this->directive['actionVar']);
        if(!empty($sType)) {
            $this->sType = $sType;
        }

    }

    /**
     * this function contains the core functionality entry point for the module.
     */
    function moduleThink() {

        if (empty($this->sType)) {$this->sType='add';}

        $myForm = $this->getResource('myForm');

        // loop thru all the 'Types' and 
        // call the preFormRun method
        foreach ($this->actions as $actionType => $actionObj) {
            if ($actionType == $this->sType) {
                $this->actions[$actionType]->preFormRun($myForm);
            }
        }

        // if dbu flag is set, notify the user of a database update
        if ($this->getVar('dbu_'.$this->moduleID) == '1') {       
            $myForm->setHeader($this->getDirective('sucessFormHeader'));
        }

        // add sType/rNum hiddens
        $myForm->addHidden($this->directive['actionVar'], $this->getVar($this->directive['actionVar']));
        $myForm->addHidden($this->directive['rNumVar'], $this->getVar($this->directive['rNumVar']));

        // run form     
        $myForm->runForm();
        
        // check for verification
        if ($myForm->dataVerified()) {

            $this->handleFileUpload();

            //loop thru each action and find out which 
            //one we need to call dataVerified on.
            foreach ($this->actions as $actionType => $actionObj) {
                if ($actionType == $this->sType) {
                    $status = $this->actions[$actionType]->dataVerified($myForm);
                }
            }

            // Run the post
            $this->postDataVerified();

            $dbu = 'dbu_'.$this->moduleID;

            if($status == -1) {
                // There was an error
                $this->debugLog("Data Verified returned an error");
                $this->fatalErrorPage("This page Generated an error, pelase contact the system administrator");
            } else if($status == 0) {
                // Everything was ok, but no rNum.
                SM_reloadPage(array($dbu=>1));
            } else {
                // Use the "status" var as the rNum.
                SM_reloadPage(array($this->directive['rNumVar']=>$status,$this->directive['actionVar']=>'edit',$dbu=>1));
            }

            exit;

        }
        else {
            $sMsg = SM_prettyName($this->sType);
            if($this->directive['showSubmit'])
                $this->say($myForm->output($sMsg));
            else
                $this->say($myForm->output());
            return;
        }

    }

    /**
     * a method that runs after the actions dataVerified() has run
     * virtual by default, may be overridden
     */
    function postDataVerified() {
        // virtual
    }


    /**
     * This function is called by moduleConfig and
     * builds the default add/edit/remove types. 
     */
    function addDefaultTypes() {
        $this->addType(new addEditorAction("add"));
        $this->addType(new editEditorAction("edit"));
        $this->addType(new removeEditorAction("remove"));
    }
    
    /**
     * add directives to a type
     */
    function addTypeDirective($type, $key, $val) {
        if (!isset($this->actions[$type])) {
            $this->fatalErrorPage("addTypeDirective: type not found: $type (ignored)");
        }
        $this->actions[$type]->addDirective($key, $val);
    }

    /**
     * Add an additional type to this object. The type 
     * should extend the 'SM_dbEditorAction' class and can't
     * have the same name as a type already added.
     *
     * @param $newType - the new object to add to the list.
     */
    function addType($newType) {
    
        if(is_subclass_of($newType, 'SM_dbEditorAction')) {
            //check to see if this type already exsists
            if (in_array($newType->getTypeName(), array_keys($this->actions))) {
                $this->fatalErrorPage("addType: type name already existed");
            }

            // tell it how to get to rNum and sType
            $newType->addDirective('rNumVar',$this->directive['rNumVar']);
            $newType->addDirective('actionVar',$this->directive['actionVar']);

            $newType->setDbEditor($this);
            $tName = $newType->getTypeName();
            $this->actions[$tName] = $newType;

        } else {
            $this->debugLog('Type was not a subclass of SM_dbEditorAction');
        }

    }

    /**
     * checks to see if there is a file and 
     * handels it accorfingly as well as sets some 
     * vars to keep track for later use.
     *
     */
    function handleFileUpload() {
        // Upload A File
        if (!empty($this->directive['fileUpload'])) {
            global $HTTP_POST_FILES;

            $sfTitle = $this->directive['fileUpload'];

            $prefix = $this->modulePrefix;
            $this->fileName = $HTTP_POST_FILES[$sfTitle.'_'.$prefix]['name'];
            $tmpName  = $HTTP_POST_FILES[$sfTitle.'_'.$prefix]['tmp_name'];
            $fileSize = $HTTP_POST_FILES[$sfTitle.'_'.$prefix]['size'];

            if ($fileSize <= $this->directive['maxUploadSize']) {
                move_uploaded_file($tmpName,$this->directive['path'].$this->fileName);
            } else {
                $this->fatalErrorPage("The file you tried to upload was too big. $fileSize exceeded ". $this->directive['maxUploadSize'] );
            }

        }
    }
    
}


?>
