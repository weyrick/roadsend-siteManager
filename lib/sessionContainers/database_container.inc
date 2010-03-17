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
 * a class for saving/loading session data to/from a database
 * using the PEAR db abstraction API
 * 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_session 
 */
class SM_sessionContainer_database extends SM_sessionContainer {

    /** 
    * ID of the database connection we're using 
    * @var string
    */
    var $dbID = '';

    // configure before we load/save
     
    function containerConfig() {

        // get session container settings
        if ($this->directive['dataBaseID'] == '')
            $this->directive['dataBaseID'] = 'default';

        $this->dbID = $this->directive['dataBaseID'];
        
        if (!isset($this->dbHL[$this->dbID])) {
            $this->fatalErrorPage("database id specified for session container: [$this->dbID] doesn't exist");
        }

        $this->directive['dbType']  = $this->siteConfig->getVar('db','dbType',false,$this->dbID);
        
        if (!isset($this->directive['sessionTable'])) {
            $this->fatalErrorPage("sessionTable was not defined");
        }

        if ((!isset($this->dbHL[$this->dbID]))&&($this->directive['useDatabase'])) {
            $this->fatalErrorPage("database connection ID '$this->dbID' does not exist. check dataBaseID in your config.");
        }

    }


    /**
     * save session to database
     * @param SM_sessionContainer_database &$persistentVars current list of persistent variables from sessions
     */
    function saveSession(&$persistentVars) {

        global $REMOTE_ADDR;

        // if we've cleared the session, don't save
        if ($this->clearSessionF) {
            return;
        }
        
        // define some configured variables we'll be using
        $sessionTable       = $this->directive['sessionTable'];

        // get database settings
        $dbType             = $this->directive['dbType'];

        // remote address
        $REMOTE_ADDR        = $_SERVER['REMOTE_ADDR'];

        // we should have all the information we need to write a session to database
        // write session variables

        // for each variable we want to keep persistent
        foreach ($persistentVars as $p) {
            
            $pVal = $this->inVarH->getSessionVar($p);
            if (isset($pVal) && ($p != $this->sessionH->directive['sessionIDName'])) {   

                // get value
                $dataVal = $pVal;

                // if the data we're about to write is the same as what's already
                // in the database, skip the query
                if ($this->origPVal[$p] === $dataVal)
                    continue;                
                else
                    // otherwise, cache the newly saved value so we avoid anothed rewrite if saveSession is called again
                    $this->origPVal[$p] = $dataVal;

                // serialize the data?
                if ($this->directive['serialize']) {                
                    $dataVal = serialize($dataVal);
                }

                // compute hash for this sessionID+dataKey combo
                // using this hash saves us a query by replacing
                // vars that are already in the database
                switch ($this->directive['hashFunc']) {
                case 'crc32':
                    $hashID = crc32($this->sessionH->sessionID.$p);
                    break;
                case 'md5':
                    $hashID = md5($this->sessionH->sessionID.$p);
                    break;
                default:
                    $this->fatalErrorPage("hashFunc must be 'crc32' or 'md5'");
                }

                switch ($dbType) {
                case 'pgsql':
                    // remove query
                    $dSQL = "
                             DELETE FROM 
                                \"$sessionTable\"
                             WHERE 
                                \"hashID\"='$hashID'
                            ";
                    // add query
                    $aSQL = "INSERT INTO 
                                $sessionTable
                            (
                                \"sessionID\",
                                \"hashID\",
                                \"remoteHost\",
                                \"createStamp\",
                                \"dataKey\",
                                \"dataVal\"
                            )
                            VALUES (
                                '{$this->sessionH->sessionID}',
                                '$hashID',
                                '$REMOTE_ADDR',                               
                                CURRENT_TIMESTAMP,
                                '$p',
                                '".addslashes($dataVal)."'
                            )
                           ";                           
                    break;
                case 'mysql':
                    // no remove query
                    $dSQL = '';
                    // add (ie, replace) query
                    $aSQL = "REPLACE INTO 
                                $sessionTable
                            SET
                                sessionID='{$this->sessionH->sessionID}',
                                hashID='$hashID',
                                remoteHost='$REMOTE_ADDR',
                                createStamp=NOW(),
                                dataKey='$p',
                                dataVal='".addslashes($dataVal)."'
                           ";
                    break;
                case 'oci8':
                    // remove query
                   $dSQL = "
                            DELETE FROM 
                               $sessionTable
                            WHERE 
                               hashID='$hashID'
                           ";
                   // add query
                   $aSQL = "INSERT INTO 
                               $sessionTable
                           (
                               sessionID,
                               hashID,
                               remoteHost,
                               createStamp,
                               dataKey,
                               dataVal
                           )
                           VALUES (
                               '{$this->sessionH->sessionID}',
                               '$hashID',
                               '$REMOTE_ADDR',                               
                               '".time()."',
                               '$p',
                               '".$dataVal."'
                           )
                          ";
                   break;                    
                default:
                    $this->fatalErrorPage("you are attemping to use database sessions with an unsupported database type: ($dbType)");
                    break;
                }

                // remove query, if necesary
                if ($dSQL != '') {                
                    $rh = $this->dbHL[$this->dbID]->query($dSQL);
                    $this->dbErrorCheck($rh);
                }

                // add query
                $rh = $this->dbHL[$this->dbID]->query($aSQL);
                $this->dbErrorCheck($rh);

            }
        }

    }

   /**
     * load session from database
     */
    function loadSession() {

        // define some configured variables we'll be using
        $sessionTable       = $this->directive['sessionTable'];

        // get database settings
        $dbType             = $this->directive['dbType'];
        $keyField = 'dataKey';
        $dataField = 'dataVal';
        
        // good session, load persistents
        switch ($dbType) {
        case 'pgsql':
            $SQL = "SELECT \"dataKey\", \"dataVal\" FROM \"$sessionTable\" WHERE \"sessionID\"='{$this->sessionH->sessionID}'";
            break;
        case 'mysql':
            $SQL = "SELECT dataKey, dataVal FROM $sessionTable WHERE sessionID='{$this->sessionH->sessionID}'";
            break;
        case 'oci8':
            $keyField = "DATAKEY";
            $dataField = "DATAVAL";
            $SQL = "SELECT $keyField, $dataField FROM $sessionTable WHERE sessionID='{$this->sessionH->sessionID}'";
            break;
        default:
            $this->fatalErrorPage("you are attemping to use database sessions with an unsupported database type: ($dbType)");
            break;
        }

        $rh = $this->dbHL[$this->dbID]->query($SQL);
        if (empty($rh)) {
            // possibly, they have an old cookie for a session ID. ignore.
            return false;
        }

        // gather stored persistents
        while ($rr = $rh->fetch()) {

            $varName = $rr[$keyField];
            $dataVal = $rr[$dataField];

            // need to unserialize?
            if ($this->directive['serialize']){
                $dataVal = unserialize($dataVal);
            }

            // save value for later comparision, to optimize write to database
            $this->origPVal[$varName] = $dataVal;

            // update inVar manager
            $this->inVarH->setSessionVar($varName,$dataVal);

            // backwards compatibility
            if ($this->siteConfig->getVar('compatibility','2.2.x')) {
                // 2.2.0 functionality is to make the variable global
                global $$varName;
                $$varName = $dataVal;
            }

        }

        $rh = null;
        $this->debugLog("valid session, persistant variables loaded from database");
        
        return true;

    }

    /**
     * clear (remove) a session
     */
    function clearSession($sessionID) {
        
        // define some configured variables we'll be using
        $sessionTable       = $this->directive['sessionTable'];

        // get database settings
        $dbType             = $this->directive['dbType'];

        // clear session
        $this->clearSessionF = true;
        
        
        switch ($dbType) {
        case 'pgsql':
        case 'mysql':
        case 'oci8':
            $SQL = "DELETE FROM $sessionTable WHERE sessionID='{$sessionID}'";
            break;
        default:
            $this->fatalErrorPage("you are attemping to use database sessions with an unsupported database type: ($dbType)");
            break;
        }

        $rh = $this->dbHL[$this->dbID]->query($SQL);
        if (empty($rh)) {
            // possibly, they have an old cookie for a session ID. ignore.
            return false;
        }
        
        return true;
    }
    
    /**
     * garbage collect. clear all sessions older than $maxLifeTime seconds 
     *
     */
    function garbageCollect($maxLifeTime) {

        // define some configured variables we'll be using
        $sessionTable       = $this->directive['sessionTable'];

        // get database settings
        $dbType             = $this->directive['dbType'];

	// expire time
	$expireTime = time() - $maxLifeTime;
        
        switch ($dbType) {
        case 'pgsql':
	    $SQL = "DELETE FROM $sessionTable WHERE EXTRACT(EPOCH FROM createStamp) <= $expireTime";
	    break;
        case 'mysql':
	    $SQL = "DELETE FROM $sessionTable WHERE UNIX_TIMESTAMP(createStamp) <= $expireTime";
	    break;
        case 'oci8':
            $SQL = "DELETE FROM $sessionTable WHERE createStamp <= $expireTime";
            break;
        default:
            $this->fatalErrorPage("you are attemping to use database sessions with an unsupported database type: ($dbType)");
            break;
        }

        $rh = $this->dbHL[$this->dbID]->query($SQL);
        if (empty($rh)) {
            // possibly, they have an old cookie for a session ID. ignore.
            return false;
        }
        
        return true;	
	
    }

    
}



?>
