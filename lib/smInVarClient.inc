<?php

/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
 *******************************************************************
 *
 * This source file is subject to version 1.0 of the Roadsend Public
 * License, that is bundled with this package in the file 
 * LICENSE, and is available through the world wide web at 
 * http://www.roadsend.com/license/rpl1.txt
 *
 *******************************************************************
 * Author(s): Shannon Weyrick (weyrick@roadsend.com)
 *
 */


/**
 * an inVar "client", used by modules and codeplates for keeping
 * track of inVars requested by that resource
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_inVar
 */
class SM_inVarClient extends SM_object {

    /**
    * a list of inVar's this client is maintaining 
    * @var array
    */
    var $inVarList = array();

    /** 
    * a reference to the object who owns this client 
    * @var SM_inVarClient
    */
    var $parent = NULL;

    /**
     * create a new inVar client
     * @param SM_inVarClient:mixed $parent parent who is creating this resource
     */
    function SM_inVarClient($parent) {
        $this->_smoConfigure();
        $this->directive['dieOnBadInVarType'] = true;
        $this->parent = $parent;
    }

    /**
     * verify that all required inVars were passed
     * @return bool true if all requireds were passed, false otherwise
     */
    function verifyInVars(&$failedVar) {

        // check for all requires
        foreach ($this->inVarList as $varName => $inVar) {
            // if the variable is flagged as required, and it wasn't passed,
            // return false
            if (($inVar->varReq) && ($this->inVarH->getVar($varName,'',false) === NULL)) {
                $failedVar = $varName;
                return false;
            }
        }

        return true;

    }

    /**
     * return an array of inVars this client manages
     * @return array list of inVars
     */
    function getInVarList() {
        return array_keys($this->inVarList);
    }

    /** 
     * declare that this client will be looking for an inVar 
     * @param string $varName the variable name
     * @param mixed $defaultVal default value the variable will get, if it was not passed to the script
     * @param string $expectType variable type this inVar should be constricted to (ie, 'int', 'array', etc), 
     *                             or array of values it may be
     * @param bool $propagate whether this inVar should propagate or not
     * @param bool $req whether this inVar is considered 'required' or not. if it's required but not
     *                    passed to the script, the script will exit with an SM_fatalErrorPage
     */
    function addInVar($varName, $defaultVal=NULL, $expectType='', $propagate=false, $req=false) {

        // default some expectType's
        if (isset($defaultVal)) {
            switch (gettype($defaultVal)) {
            case 'integer':
                $expectType='integer';
                break;
            case 'array':
                $expectType='array';
                break;
            case 'object':
                $expectType='object';
                break;
            }        
        }

        // inVar types
        if (is_string($expectType)) {
        switch ($expectType) {
            case 'bool':
            case 'boolean':
            case 'string':
            case 'double':            
            case 'number':
            case 'int':
            case 'integer':
            case 'array':
            case 'object':
            case 'md5':
            case 'hex':
            case '':
                break;
            default:
                $this->fatalErrorPage("addInVar(): expectType was unknown: $expectType",$this);
            }
        }
        elseif (is_array($expectType)) {
            // this will be an enumerated list of values this variable may be
            // to be considered valid
        }
        else {
            $this->fatalErrorPage("addInVar(): bad expectType",$this);
        }

        // propagation?
        if ($propagate === NULL) {        
            $propagate = $this->directive['propagateInVars'];
        }

        if (is_string($propagate)) {
            if ($propagate == 'true') $propagate = true;     
            if ($propagate == 'false') $propagate = false;
        }

        // create new inVar item
        $this->inVarList[$varName] = new SM_inVarItem();
        $this->inVarList[$varName]->varName     = $varName;
        $this->inVarList[$varName]->defaultVal  = $defaultVal;
        $this->inVarList[$varName]->varType     = $expectType;
        $this->inVarList[$varName]->varProp     = $propagate;
        $this->inVarList[$varName]->varReq      = $req;

        // check this inVar against the current list of propagated inVars.
        // throw a warning if it's already there
        // no longer need the global, use inVarH
        if ($this->inVarH->isInVarP($varName)) {
            $this->debugLog("warning: inVar [$varName] in resource [".get_class($this->parent)."] is already used as an inVar in another resource (possible conflict?)");
        }
        else {
            // not already used, so add this to the list of propagated vars via inVarH if $propagate is true
            if ($propagate)
                $this->inVarH->addInVarP($varName);
        }
    }

    /**
     * get the value of a variable declared as an inVar
     * @param string $varName the variable to retrieve
     * @return mixed value of the variable, or NULL if not found
     */
    function getVar($varName) {

        // sanity
        if (!isset($this->inVarList[$varName]))
            return NULL;

        // check inVar manager (no type check here)
        $val = $this->inVarH->getVar($varName);

        // check type manually
        $typeOK = $this->inVarH->_checkType($varName, $val, $this->inVarList[$varName]->varType, false);

        // if the type was bad, run custom function or SM_fatalErrorPage if we are supposed to die
        if ((!$typeOK) && ($this->directive['dieOnBadInVarType'])) {
            // parent method
            if (method_exists($this->parent,'eventBadInVar'))
                $val = $this->parent->eventBadInVar($varName, $this->inVarList[$varName]->varType);
            // default message
            else
                $this->fatalErrorPage('The script was accessed incorrectly.');
        }

        if (!isset($val))
            return $this->inVarList[$varName]->defaultVal;
        else
            return $val;

    }

    // tcobb: added to support settting InVar values 
    /**
     * set the value of a variable declared as an inVar
     * overrides the valued passed in via GET/POST/COOKIE
     * @param string $varName the variable to retrieve
     * @param mixed $varValue the value of the variable to set
     * @return bool TRUE if found, FALSE if not found
     */
    function setInVar($varName, $varValue) {

        // sanity
        if (!isset($this->inVarList[$varName]))
            return FALSE;

        // check type manually
        $typeOK = $this->inVarH->_checkType($varName, $varValue, $this->inVarList[$varName]->varType, false);

        // if the type was bad, run custom function or SM_fatalErrorPage if we are supposed to die
        if ((!$typeOK) && ($this->directive['dieOnBadInVarType'])) {
            // parent method
            if (method_exists($this->parent,'eventBadInVar'))
                $this->parent->eventBadInVar($varName);
            // default message
            else
                $this->fatalErrorPage('The script was accessed incorrectly.');
        }

        $this->inVarH->setInVar($varName,$varValue);

        return TRUE;
    }

    /**
     * return the number of inVars in the list
     * @return int number of declared inVars
     */
    function getNumInVars() {
        return count($this->inVarList);
    }

    /**
     * get a list of inVars that were declared as propagating inVars (outVars)
     * when added to the list
     * @return array: string a list of inVar variable names that should propagate, or NULL on error
     */
    function getInVarP() {

        if (!count($this->inVarList)) {
            return NULL;
        }

        $ivList = array();
        foreach ($this->inVarList as $ivItem) {
            if ($ivItem->varProp)
                $ivList[] = $ivItem->varName;
        }
        return $ivList;
    }

    /**
     * get the variable type that a declared inVar should use
     * @param string $varName the variable to lookup
     * @return string a valid php variable type, or blank if not found
     */
    function getType($varName) {
        if (isset($this->inVarList[$varName]))
            return $this->inVarList[$varName]->varType;
        else
            return '';
    }

    /**
     * discover whether a declared inVar should propagate or not
     * @param string $varName the variable to lookup
     * @return bool whether this variable should propagate or not, or NULL if not found
     */
    function getProp($varName) {
        if (isset($this->inVarList[$varName]))
            return $this->inVarList[$varName]->varProp;
        else
            return NULL;        
    }


    /**
     * dump the value of all variables we've setup as inVars
     * for debugging purposes
     */
   function dumpInVars() {
   
        if (!count($this->inVarList))
            return;
        
        global $_dOp;

        $_dOp = 'registered inVar information (inVar => value): <br /><ul>';


        foreach ($this->inVarList as $iV => $data) {
            if ($this->getProp($iV)) {
                $_dOp .= "<b>$iV *WILL* propagate (read-write)</b><br />\n";
            }
            else {
                $_dOp .= "$iV will not propagate (read-only)<br />\n";
            }
        }

        $_dOp .= '<br />';

        array_walk($this->inVarList, '_ddPrint');       

        $_dOp .= '<br />';


        $_dOp .= '</ul><br />';

        return $_dOp;
     
   }

}

/**
 * a single inVar item, used by the inVarClient 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_inVar
 */
class SM_inVarItem {

    /** 
    * the inVar varName 
    * @var string
    */
    var $varName = '';

    /** 
    * the inVar variable type, or array of possible valid values 
    * @var mixed 
    */
    var $varType = '';

    /** 
    * the default value 
    * @var mixed
    */
    var $defaultVal = NULL;

    /** 
    * the inVar propagation details 
    * @var bool
    */
    var $varProp = false;

    /** 
    * wether this inVar is required for succesfull operation by the parent 
    * @var bool
    */
    var $varReq = false;

}

?>
