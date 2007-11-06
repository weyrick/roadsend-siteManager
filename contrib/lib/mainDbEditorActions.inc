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
* Author(s): gollum
*
*
* Change Log
* 
*       10/24/2001 - created
*
*       11/20/2001 - Modified to use the new includeLib function to load
*                    libary file for SM_dbEditorAction
*
*       12/18/2001 - Modified to handle the actual lookup of the data for
*                    for the smart form elements for both the edit and
*                    delete actions. (gollum)
*
*       01/12/2004 - Clean up problems with refrences. Removed old, commented
*                    out code. Changed SM_dbErrorCheck to $this->dbErrorCheck, etc. (gollum)
*
*/

global $SM_siteManager;
$SM_siteManager->includeLib('dbEditorAction');

class addEditorAction extends SM_dbEditorAction {

    /**
     * Method to handle what gets done if the 
     * data on the form is Verified
     */
    function dataVerified($myForm) {
        $dbEditor = $this->getDbEditor();
        $database = $dbEditor->dbHL[$dbEditor->directive['dataBaseID']];
        $SQL = "INSERT INTO {$dbEditor->directive['tableName']} SET ";

        // loop through the variables in the form and update the database
        $fList = $myForm->getVarList();

        // build the update query               
        foreach ($fList as $f) {

            // set the value for file uploads to the file name
            if ($f == $dbEditor->directive['fileUpload']) {

                // Skip on blank - so they don't have to reload the image
                if (empty($this->fileName))
                    continue;

                $val = $fileName;

            // pull the normal value
            } else {
                $val = $myForm->getVar($f);
            }


            // see how we need to write out the variable in the query based on it's type
            if ($val=='') {
                // nothing to see here, move along.
                continue;
            } else {
                if (get_magic_quotes_gpc())
                    $data = "'".$val."'";
                else
                    $data = "'".addslashes($val)."'";
            }

            // check for field hook by directive
            // NOTE this only handles auto creation when the field was actually
            // added to the form. this often isn't the case because these fields are
            // often hidden. that's taken care of after this loop (weyrick)
            if (($autoDateFields = $this->getDirective('autoDateFields')) != NULL) {
                // handle dateCreated type fields by setting their value to NOW()
                if (in_array($f, $autoDateFields)) {
                    // override data
                    $data = 'NOW()';
                }            
            }
            if (($autoMD5Fields = $this->getDirective('autoMD5Fields')) != NULL) {
                // handle auto generation of MD5 fields
                if (in_array($f, $autoMD5Fields)) {
                    // override data
                    $data = "'".md5(uniqid(rand(),true))."'";
                }
            }

            // append to SQL query  
            $SQL .= "$f=$data, ";  

        }

        // here we handle auto creation of fields that weren't in the table, but were in
        // auto creation directives
        if (($autoDateFields = $this->getDirective('autoDateFields')) != NULL) {
            foreach ($autoDateFields as $adF) {
                // handle dateCreated type fields by setting their value to NOW()
                if (!in_array($adF, $fList)) {
                    // directive wasn't in main field list, add it here
                    $SQL .= "$adF=NOW(), ";
                }            
            }
        }
        if (($autoMD5Fields = $this->getDirective('autoMD5Fields')) != NULL) {
            foreach ($autoMD5Fields as $mdF) {
                // handle dateCreated type fields by setting their value to NOW()
                if (!in_array($mdF, $fList)) {
                    // directive wasn't in main field list, add it here
                    $SQL .= "$mdF='".md5(uniqid(rand(),true))."', ";
                }            
            }
        }

        // chop off that final comma
        $SQL = substr($SQL, 0, -2);

        // execute the query
        $rh = $database->query($SQL);

        $this->dbErrorCheck($rh);

        // if it was an add, return the id of the new record
        return mysql_insert_id();

    }

}

class editEditorAction extends SM_dbEditorAction {
    
    /**
     * Method to handle what gets done if the 
     * data on the form is Verified
     */
    function dataVerified($myForm) {
        $dbEditor = $this->getDbEditor();
        $database = $dbEditor->dbHL[$dbEditor->directive['dataBaseID']];

        $SQL = "UPDATE {$dbEditor->directive['tableName']} SET ";
            
        // loop through the variables in the form and update the database
        $fList = $myForm->getVarHash();

        // build the update query               
        foreach ($fList as $f => $fVal) {

            // set the value for file uploads to the file name
            if ($f == $dbEditor->directive['fileUpload']) {

                // Skip on blank - so they don't have to reload the image
                if (empty($this->fileName))
                    continue;

                $val = $fileName;

            // pull the normal value
            } else {
                $val = $fVal;
            }

            // handle enums/checkboxes
            if (is_array($val)) {
                $val = join(',',$val);
            }

            $data = $database->quote($val);

            // append to SQL query  
            $SQL .= "$f=$data, ";  
        }

        // chop off that final comma
        $SQL = substr($SQL, 0, -2);

        // add where clause if editing
        $SQL .= " WHERE {$dbEditor->directive['dataField']}='" . $dbEditor->rNum."'";

        // execute the query
        $rh = $database->query($SQL);
        $this->dbErrorCheck($rh);

        // return the rNum
        return $dbEditor->rNum;

    }

    /**
     * Method to handle anything that needs to 
     * be done before the form is actually run.
     */
    function preFormRun($myForm) {
        
        $dbEditor = $this->getDbEditor();

        $this->debugLog('rNum is '.$this->directive['rNumVar'].' and its value is '.$dbEditor->rNum);

        // if we've got a record we're viewing, get the data from the databse
        if ($dbEditor->rNum != 0) {
            
            $SQL = "SELECT * FROM {$dbEditor->directive['tableName']} WHERE {$dbEditor->directive['dataField']}='" . $dbEditor->rNum."'";
            $rh = $dbEditor->dbHL[$dbEditor->directive['dataBaseID']]->query($SQL);
            $this->dbErrorCheck($rh);            
            $rr = $rh->fetch();
            $dbEditor->rowData = $rr;
        }

        // ge a list of vars from the form and attempt to set the default values
        $list = $myForm->getVarList();
        foreach($list as $name) {
            // make sure it's not on the noDbValue list
            if ( (!isset($this->dbEditor->directive['noDbValue'])) || ((is_array($this->dbEditor->directive['noDbValue'])) && 
                 (!in_array($name, $this->dbEditor->directive['noDbValue']))) ) {
                    $myForm->setDefaultvalue($name, $rr[$name]);
                 }
        }

    }

}

class removeEditorAction extends SM_dbEditorAction {

    /**
     * Method to handle what gets done if the 
     * data on the form is Verified
     */
    function dataVerified($myForm) {
        $dbEditor = $this->getDbEditor();
        $database = $dbEditor->dbHL[$dbEditor->directive['dataBaseID']];

        // make sure they confirmed
        if ($myForm->getVar('_removeConfirm') != 'yes') {

            // redisplay form
            $myForm->setHeader("YOU MUST CHECK THE BOX TO CONFIRM REMOVAL");
            $sMsg = SM_prettyName($dbEditor->sType);
            $dbEditor->say($myForm->output($sMsg,array($this->directive['actionVar']=>$dbEditor->sType,$this->directive['rNumVar']=>$dbEditor->rNum)));
            return;
        }
        
        // support for external remove function
        if (method_exists($dbEditor->directive['removeObject'],$dbEditor->directive['removeFunction'])) {
            $object = $dbEditor->directive['removeObject'];
            $fName = $dbEditor->directive['removeFunction'];
            $object->$fName($dbEditor->rNum);
        }

        // verified, remove it
        $database->query("DELETE FROM {$dbEditor->directive['tableName']} WHERE {$dbEditor->directive['dataField']}='" . $dbEditor->rNum."'");

        //return 0 because everything went ok.
        return 0;
    }

    /**
     * Method to handle anything that needs to 
     * be done before the form is actually run.
     */
    function preFormRun($myForm) {

        $dbEditor= $this->getDbEditor();

        // if we've got a record we're viewing, get the data from the databse
        if ($dbEditor->rNum != 0) {
            $SQL = "SELECT * FROM {$dbEditor->directive['tableName']} WHERE {$dbEditor->directive['dataField']}='" . $dbEditor->rNum."'";
            $rh = $dbEditor->dbHL[$dbEditor->directive['dataBaseID']]->query($SQL);
            $this->dbErrorCheck($rh, $this);
            
            $rr = $rh->fetch();
        }

        // ge a list of vars from the form and attempt to set the default values
        $list = $myForm->getVarList();
        foreach($list as $name) {
            $myForm->setDefaultvalue($name, $rr[$name]);
        }

        // add an extra check so that we can confirm the delete.
        if($dbEditor->sType == $this->getTypeName()) {
            $myForm->addDirective('requiredTag','');
            $rC = $myForm->add('_removeConfirm','','checkBox',true);
            $rC->addOption('Are you SURE you wish to remove this item?','yes');
            $myForm->setLayout('_removeConfirm',SF_LAYOUT_SINGLE);
        }
    }


}


?>
