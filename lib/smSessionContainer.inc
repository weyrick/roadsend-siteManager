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
 * a base class for saving/loading session data
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_session
 */
class SM_sessionContainer extends SM_object {

    /** 
     * the original state of peristents as read in from a container 
     * @var array
     */
    var $origPVal = array();

    /**
     * flag to determine if the session is being cleared, in which case
     * it won't be saved
     * @var bool
     */
    var $clearSessionF = false;
    
    /**
    * session container startup
    */
     
    function SM_sessionContainer($sessionH, $cType) {

        // SM_object config
        $this->_smoConfigure();

        // parent session
        $this->sessionH = $sessionH;

        // read config
        $this->directive = $this->siteConfig->getSection('sessionContainer', $cType);

        // run config
        $this->containerConfig();
        
    }


    /**
    * configure the container
    */
     
    function containerConfig() {
        // virtual
    }

    /**
    * save session variables to container
    */
     
    function saveSession(&$pVars) {
        // virtual
        return true;
    }


    /**
    * load session variables from container
    */
     
    function loadSession() {
        // virtual
        return false;
    }

    /**
     * clear (remove) a session
     */
    function clearSession($sessionID) {
        // virtual
        return true;
    }

    /**
     * garbage collect. clear all sessions older than $maxLifeTime seconds 
     *
     */
    function garbageCollect($maxLifeTime) {
	// virtual
	return true;
    }
    
}


?>
