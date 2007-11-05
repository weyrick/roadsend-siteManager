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
*/

/**
 *
 */
class SM_dbEditorAction extends SM_object {

    /** Keep track of the 'type' represented by this object **/
    var $typeName = '';

    /** keep track of the editor that has us **/
    var $dbEditor = NULL;
    

    function SM_dbEditorAction ($name) {
        
        //set the name for this object
        $this->setTypeName($name);
        
        //call the SM_object's constructor.
        SM_object::_smoConfigure();

        // the variable name we use to indicate a record ID
        $this->addDirective('rNumVar','rNum');
        // the variable name we use to indicate action type
        $this->addDirective('actionVar','sType');

    }

    /**
     * Method to handle anything that needs to 
     * be done before the form is actually run.
     *
     *  *** Should be overridden by sub-classes
     *
     */
    function preFormRun($myForm) {
    }

    /**
     * Method to handle what gets done if the 
     * data on the form is Verified.
     * It returns -1 on an error and 0 or any other
     * number on sucess. This number will be set as the
     * rNum on the page reload and wil determin what sType
     * pass along. 
     *
     *  *** Should be overridden by sub-classes
     *
     */
    function dataVerified($myForm) {
        return 0;
    }

    /**
     * This function returns the 'editor' 
     * object that has this object
     *
     */
    function getDbEditor() {
        return $this->dbEditor;
    }

    /**
     * This function returns the 'type' name of this class
     *
     */
    function getTypeName() {
        return $this->typeName;
    }

    /**
     * Sets the 'dbEditor' that has this object
     */
    function setDbEditor($newDbEditor) {
        $this->dbEditor = $newDbEditor;
    }

    /**
     * Sets the 'type' name
     */
    function setTypeName($newTypeName) {
        $this->typeName = $newTypeName;
    }

}
?>


