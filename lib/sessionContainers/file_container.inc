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
 * a class for saving/loading session data from/to files
 * 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_session 
 */
class SM_sessionContainer_file extends SM_sessionContainer {


    /**
     * save session out to a file
     * @param array &$persistentVars not used in this function, since we access the sessionVarList from inVar manager     
     */
    function saveSession(&$persistentVars) {

        // if we're clearing the session, don't save
        if ($this->clearSessionF) {
            return;
        }
        
        // get current sessionID
        $sessionID = $this->sessionH->getSessionID();

        // find path
        $fileName = $this->directive['savePath'].$sessionID;

        // get current session variables and their values
        $sVarHash = $this->inVarH->getSessionVarList();
        
        // unset sessionIDname
        unset($sVarHash[$this->sessionH->directive['sessionIDName']]);

        // serialize
        $pVars = serialize($sVarHash);

        // open...
        if (!$fp = fopen($fileName, 'wb')) {
            $this->debugLog("unable to open session save file $fileName - save not saved (check perms)");
            return;
        }


        // save, done
        fwrite($fp, $pVars, strlen($pVars));
        fclose($fp);

        $this->debugLog("saved session to $fileName");

    }


    /**
     * load session from file
     */
    function loadSession() {

        // get session ID
        $sessionID = $this->sessionH->getSessionID();

        // build file name
        $fileName = $this->directive['savePath'].$sessionID;

        if (!$fp = fopen($fileName, 'rb')) {
            // no session to load
            return false;
        }

        // gather stored persistents
        $pVarsS = fread($fp, filesize($fileName));

        // need to unserialize
        $pVars = unserialize($pVarsS);

        // if it's not an array, assume it's a bad session file
        if (!is_array($pVars)) {
            $this->debugLog("invalid session file, ignoring");
            return false;
        }

        // loop through array and recreate session
        foreach ($pVars as $varName => $dataVal) {
        
            // update inVar manager
            $this->inVarH->setSessionVar($varName, $dataVal);
        
            // backwards compatibility
            if ($this->siteConfig->getVar('compatibility','2.2.x')) {
                // 2.2.0 functionality is to make the variable global
                global $$varName;
                $$varName = $dataVal;
            }
        
        }

        // done
        $this->debugLog("loaded session from $fileName");
        return true;

    }

    /**
     * clear (remove) a session
     */
    function clearSession($sessionID) {
        
        // set flag
        $this->clearSessionF = true;
        
        // remove session file
        $fileName = $this->directive['savePath'].$sessionID;
        return unlink($fileName);
        
    }    
    
}


?>
