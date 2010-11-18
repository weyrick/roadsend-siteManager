<?php

/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2010 Roadsend, Inc.(http://www.roadsend.com)
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
 * a small container to hold the actual session data
 */
class SM_sessionData {

    /**
     * key/value pairs for this session
     * @var array
     */
    public $data=array();

    /**
     * remote ip
     */
    public $remoteHost=0;

    /**
     * create stamp
     */
    public $dateCreated=0;

    /**
     * last updated
     */
    public $lastUpdated=0;
    
}

/**
 * a class for saving/loading session data to/from memory
 * using the memcached
 * 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_session 
 */
class SM_sessionContainer_memcached extends SM_sessionContainer {

    /**
     * the memcached instance
     * @var Memcached
     */
    protected $mc = NULL;

    /**
     * actual session data container
     * @var SM_sessionData
     */
    protected $sContainer = NULL;

    /**
     * @var bool whether we've written to memcached or not this page view
     */
    protected $written = false;

    // configure before we load/save     
    function containerConfig() {

        // get session container settings
        if ($this->directive['memcachedID'] == '')
            $this->directive['memcachedID'] = 'default';

        $config  = $this->siteConfig->getSection('memcached',$this->directive['memcachedID']);
        
        if (!isset($config['host'])) {
            $this->fatalErrorPage("memcached host was not defined");
        }

        if (!isset($config['port'])) {
            $this->fatalErrorPage("memcached port was not defined");
        }

        global $SM_siteID;
        $this->mc = new Memcached($SM_siteID);
        if (!$this->mc->addServer($config['host'], $config['port'])) {
            $this->fatalErrorPage('unable to connect to memcached server at '.$host.':'.$port);
        }

    }


    /**
     * save session vars
     * @param array &$persistentVars current list of persistent variables from sessions
     */
    function saveSession(&$persistentVars) {

        // if we've cleared the session, don't save
        if ($this->clearSessionF)
            return;

        // for each variable we want to keep persistent
        $changed = false;
        foreach ($persistentVars as $p) {
            
            $pVal = $this->inVarH->getSessionVar($p);
            if (!isset($pVal) || ($p == $this->sessionH->directive['sessionIDName']))
                continue;

            if ($this->sContainer->data[$p] != $pVal) {
                $this->sContainer->data[$p] = $pVal;
                $changed = true;
            }

        }

        // if the variables we are saving haven't changed, and we've already written
        // once this page view (i.e. updated lastUpdated), then skip it
        if (!$changed && $this->written)
            return;

        // last update is always touched
        $this->sContainer->lastUpdated = time();
        $key = $this->getKey();
        $this->mc->set($key, $this->sContainer, 0/*no expiration*/);
        $this->written = true;

        // store this id in our global list, so we can walk sessions later
        //$this->saveToKeyList();

    }

    private function saveToKeyList($sessionID=NULL) {
        if (empty($sessionID))
            $sessionID = $this->sessionH->sessionID;
        $keyList = $this->mc->get('SMS_KEYLIST');

        if (empty($keyList))
            $keyList = array($sessionID => true);
        else
            $keyList[$sessionID] = true;
        $this->mc->set('SMS_KEYLIST', $keyList);
    }

    // key used on memcached server for this session
    private function getKey($sessionID=NULL) {
        if (empty($sessionID))
            $sessionID = $this->sessionH->sessionID;
        return 'SMS_'.$sessionID;
    }

   /**
     * load session from database
     */
    function loadSession() {

        $key = $this->getKey();
        $this->sContainer = $this->mc->get($key);
        if (empty($this->sContainer)) {
            $this->debugLog("no valid session for this session id");

            // setup a default, for save
            $this->sContainer = new SM_sessionData();
            $this->sContainer->dateCreated = time();
            $this->sContainer->remoteHost = $_SERVER['REMOTE_ADDR'];

            return false;
        }

        // update inVar manager
        $this->inVarH->setSessionVarList($this->sContainer->data);
        $this->debugLog("valid session, ".sizeof($this->sContainer->data)." persistant variables loaded from memcached");

        //$this->debugLog(print_r($this->sContainer,true));
        
        return true;

    }

    /**
     * clear (remove) a session
     */
    function clearSession($sessionID) {
        $key = $this->getKey();
        $this->mc->delete($key);
        return true;
    }
    
    /**
     * garbage collect. clear all sessions older than $maxLifeTime seconds 
     *
     */
    function garbageCollect($maxLifeTime) {
        return true;	
    }

    
}


?>