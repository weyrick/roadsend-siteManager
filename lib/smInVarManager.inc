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
 * a class for regulating incoming script variables 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_inVar 
 */
class SM_inVarManager extends SM_object {

    /** 
    * session (persistent) variables, valid for this session 
    * @var mixed
    */
    var $sessionVarList = array();

    /** 
    * list of variable names we will NOT return data for 
    * @var mixed
    */
    var $badVarList = array();
    
    /** 
    * values overriden by setInVar (tcobb: added to support setInVar)
    * @var array
    */
    var $overriddenVarList = array();

    /** 
    * a list of inVars to propagate for each module loaded by the system 
    * @var array
    */
    var $inVarPList = array();
    
    /** 
     * add a variable to the bad variable list
     * @param string $varName the variable name to add
     */
    function addBadVar($varName) {
        $this->badVarList[] = $varName;
    }


    /**
     * return a variable passed to this script as a cookie, optionally checking type
     * @param string $varName the variable name to retrieve
     * @param string $type a valid PHP variable type
     * @return mixed contents of the cookie requested, or NULL if it was in the badVar list, or not found
     */
    function getCookieVar($varName, $type = '') {

        // no bad vars
        if (in_array($varName, $this->badVarList))
            return NULL;

        // get value
        if (isset($_COOKIE[$varName])) {
            $varVal = $_COOKIE[$varName];
        }
        else {
            $varVal = NULL;
        }

        // check type
        if ($type != '')
            $this->_checkType($varName, $varVal, $type);

        // return value
        return $varVal;

    }


    /**
     * private method to add value of session variable
     * @param string $varName add a variable to the session (persistent) var list
     * @param mixed $varVal the value of the session variable
     */
    function addSessionVar($varName, $varVal) {

        // if there's already a value set for this session variable, don't overwrite
        if (isset($this->sessionVarList[$varName]) && $this->sessionVarList[$varName] != '')
            return;

        // otherwise, add/set the variable
        $this->sessionVarList[$varName] = $varVal;

    }

    /**
     * set value of session variable, only by session system
     * @param string $varName variable to set
     * @param mixed $varVal new value
     */
    function setSessionVar($varName, $varVal) {
                       
        $this->sessionVarList[$varName] = $varVal;

    }

    /**
     * override an inVar value. use sparingly for best practice.
     * @param string $varName variable to override/set
     * @param mixed $varVal new value
     */
    function setInVar($varName, $varVal) {

        $this->overriddenVarList[$varName] = $varVal;

    }

    /**
     * return the entire session var list hash
     * @return array a copy of the current session var hash
     */
    function getSessionVarList() {
        return $this->sessionVarList;
    }

    /**
     * retrieve the current value of a persistent session variable
     * @param string $varName the session variable name
     * @return mixed the value of the session variable, or NULL if not found
     */
    function getSessionVar($varName) {

        // return current value of session variable
        if (isset($this->sessionVarList[$varName])) {
            return $this->sessionVarList[$varName];
        }
        else {        
            return NULL;
        }

    }

    /**
     * retrieve the value of a variable passed as a POST variable to this script, optionally
     * checking variable type. it will not return a variable on the badVar list
     * @param string $varName POST variable name
     * @param string $type [optional: blank] valid PHP variable type
     * @param bool $dieOnBadType [optional: false] whether to issue of fatal error, or a warning if bad type
     * @return mixed value of the variable, or NULL if not found
     */
    function getPOST($varName, $type='', $dieOnBadType=true) {

        // no bad vars
        if (in_array($varName, $this->badVarList))
            return NULL;

        // get value
        if (isset($_POST[$varName]))        
            $varVal = $_POST[$varName];
        else
            $varVal = NULL;

        // check type
        if ($type != '')
            $this->_checkType($varName, $varVal, $type, $dieOnBadType);

        // return value
        return $varVal;

    }

    /**
     * retrieve the value of a variable passed as a GET variable to this script, optionally
     * checking variable type. it will not return a variable on the badVar list
     * @param string $varName POST variable name
     * @param string $type [optional: blank] valid PHP variable type
     * @param bool $dieOnBadType [optional: false] whether to issue of fatal error, or a warning if bad type
     * @return mixed value of the variable, or NULL if not found
     */
    function getGET($varName, $type='', $dieOnBadType=true) {

        // no bad vars
        if (in_array($varName, $this->badVarList))
            return NULL;

        // get value
        if (isset($_GET[$varName]))        
            $varVal = $_GET[$varName];
        else
            $varVal = NULL;

        // check type
        if ($type != '')
            $this->_checkType($varName, $varVal, $type, $dieOnBadType);

        // return value
        return $varVal;

    }

    /**
     * get a variable passed to the script, either through Sessions (persistent) or GET, POST, or COOKIE
     * optionally checking type. it will not return a variable on the badVar list
     * variable precendence is:
     * 1) session var
     * 2) GET/POST/COOKIE (_REQUEST variable)
     * @param string $varName the variable name to retrieve
     * @param string $type [optional: blank] valid PHP variable type
     * @param bool $dieOnBadType [optional: false] whether to issue of fatal error, or a warning if bad type
     * @return mixed value of the variable, or NULL if not found     
     */
    function getVar($varName, $type='', $dieOnBadType=true) {

        // no bad vars
        if (in_array($varName, $this->badVarList))
            return NULL;

        // value defaults to NULL
        $varVal = NULL;

        // check sessions first
        if (isset($this->sessionVarList[$varName])) {
            $varVal = $this->sessionVarList[$varName];
        }
        else {
            // check overriden var list next
            if (isset($this->overriddenVarList[$varName])) {
                $varVal = $this->overriddenVarList[$varName];
            }
            else {
                // GET/POST/COOKIE
                if (isset($_REQUEST[$varName]))
                    $varVal = $_REQUEST[$varName];
            }
        }

        // check type
        if ($type != '')
            $this->_checkType($varName, $varVal, $type, $dieOnBadType);

        // return value
        return $varVal;

    }

    /**
     * determine if the variable passed is currently in the list of variables
     * to propagate
     * @param string $varName the variable to check
     * @return bool whether this variable will propagate or not, at time of check
     */
    function isInVarP($varName) {
        return in_array($varName, $this->inVarPList);
    }

    /**
     * add an inVar for propagation
     * @param string $varName the variable to add
     */
    function addInVarP($varName) {
        if (!in_array($varName, $this->inVarPList))
            $this->inVarPList[]=$varName;
    }


    /**
     * private function used to verify the type of an inVar
     *
     * @param string $var the inVar variable to check. must have been declared by addInVar()
     * @param mixed $val the value to check it against
     * @param string $type valid PHP variable type
     * @param bool $dieOnBadType whether to issue of fatal error, or a warning if bad type
     * @return bool true on success, false on failure. will not return if invalid type and dieOnBadInVarType is true
     */
    function _checkType($var, $val, $type, $dieOnBadType) {

        // if it's a blank string, ignore
        if ($val == '')
            return true;

        // if no type specified, ignore
        if (is_string($type) && ($type == ''))
            return true;

        $vOk = false;

        // check basic types
        if (is_string($type)) {
            switch ($type) {
            case 'number':
            case 'int':
            case 'integer':
                $vOk = is_numeric($val);
                break;
            case 'array':
                $vOk = is_array($val);
                break;
            case 'object':
                $vOk = is_object($val);
                break;
            case 'double':
                $vOk = is_double($val);
                break;
            case 'boolean':
            case 'bool':
                $vOk = is_bool($val);
                break;
            case 'md5':
                $vOk = preg_match('/^[0-9a-fA-F]{32}$/',$val);
                break;
            case 'hex':
                $vOk = preg_match('/^[0-9a-fA-F]+$/',$val);
                break;
            case 'string':
                $vOk = is_string($val);
                break;
            default:
                // these should never match, if they do it's invalid                
                $this->fatalErrorPage("checkType() found unknown type: ".$type);
                break;
            }
        }
        // check enumerated list of possible valid values
        elseif (is_array($type)) {
            foreach ($type as $t) {
                $vOk = ($val == $t);
                if ($vOk)
                    break;
            }
        }

        if (!$vOk) {
            $msg = "A required variable appears to be of an invalid type ($var / $val [wanted $type])";
            if ($dieOnBadType) 
                $this->fatalErrorPage($msg);
            else
                $this->debugLog($msg);
        }

        // return status
        return $vOk;

    }

}


?>
