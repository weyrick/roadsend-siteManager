<?php

/*********************************************************************
 *  Roadsend SiteManager
 *  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
 *********************************************************************
 *
 * This source file is subject to version 1.0 of the Roadsend Public
 * License, that is bundled with this package in the file 
 * LICENSE, and is available through the world wide web at 
 * http://www.roadsend.com/license/rpl1.txt
 * see dbRecordSelector.mod
 *
 *********************************************************************
 * Author(s): gollum
 *
 *
 * Change Log:
 *      10/30/2001 - Created. Base class for database specfic 'hooks'
 *                   for the dbGui module. 
 *
 */
 
class dbGuiBaseHook extends SM_object {

    var $_className = 'dbGuiBaseHook';

    function dbGuiBaseHook () {        
        //call the SM_object's constructor.
        SM_object::_smoConfigure();
    }

    /**
     * This function is called in the 'addToSmartForm' method of the
     * columnDefinition class in dbGui. It is called just before
     * myform->commit() and passes in a refrence to the current
     * columnDefinition class. 
     */
    function addToSmartFormHook($columnDefinition, $ie) {
    }

    /**
     * returns the class name of this class.
     *
     * If no calss name is set, it returns the value for
     * get_class($this), otherwise it returns $this->className
     */
    function getClassName() {
        if ($this->_className == '')
            return get_class($this);
        else
            return $this->_className;
    }

    /**
     * Function to set the class name. It is here because
     * the get_class() function returns the class name in all
     * lower case and case sensitive issues can arise. 
     *
     * This class checks to make sure that the name that is
     * trying to be set at least matches the get_class() version
     */
    function setClassName($name) {
        if(strtolower(get_class($this)) != strtolower($name))
            SM_fatalErrorPage("Class names did not match");

        $this->_className = $name;
    }

    /**
     * This function is called in the 'setDataType' method of the
     * columnDefinition class in dbGui. It is called just before
     * the end of the method and passes in a refrence to the
     * current columnDefinition class. 
     */
    function setDataTypeHook($columnDefinition, $flags) {
    }
}
  
?>
