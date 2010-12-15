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
    public $mc = NULL;

    /**
     * actual session data container
     * @var SM_sessionData
     */
    public $sContainer = NULL;

    /**
     * @var bool whether we've written to memcached or not this page view
     */
    protected $written = false;

    /**
     * @var int timeout for vars stored in memcached
     */
    protected $ttl = 0;

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

        // pull ttl from session config
        $this->ttl = $this->sessionH->directive['gcMaxLifeTime'];

        global $SM_siteID;
        $this->mc = new Memcached($SM_siteID);
        if (!$this->mc->addServer($config['host'], $config['port'])) {
            $this->fatalErrorPage('unable to connect to memcached server at '.$host.':'.$port);
        }

        $this->mc->setOption(Memcached::OPT_NO_BLOCK, true);
        $this->mc->setOption(Memcached::OPT_TCP_NODELAY, true);
        //$this->mc->setOption(Memcached::OPT_LIBKETAMA_COMPATIBLE, true);

    }

    // called to create a session from an external source, e.g. the command line import script
    // data is a hash of key/val session data pairs
    // dateCreated is a unix timestamp
    // remoteHost is a string of dotted quad
    function addDirectSession($sessionID, $dateCreated, $lastUpdated, $remoteHost, $data) {

        if (!preg_match('/^\d+$/', $dateCreated))
            $this->fatalErrorPage('invalid date: '.$dateCreated);

        if (!preg_match('/^\d+$/', $lastUpdated))
            $lastUpdated = 0;

        $container = new SM_sessionData();
        $container->dateCreated = $dateCreated;
        $container->remoteHost = $remoteHost;
        $container->data = $data;
        $container->lastUpdated = $lastUpdated;

        $key = $this->getKey($sessionID);
        $this->mc->set($key, $container, time()+$this->ttl);
        $this->saveToKeyList($sessionID);

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
        $this->mc->set($key, $this->sContainer, time()+$this->ttl);
        $this->written = true;

        // store this id in our global list, so we can walk sessions later
        $this->saveToKeyList();

    }

    private function saveToKeyList($sessionID=NULL) {
        if (empty($sessionID))
            $sessionID = $this->sessionH->sessionID;
        $keyList = $this->mc->get('SMS_KEYLIST');
        if (empty($keyList))
            $keyList = array('SMS_'.$sessionID => true);
        else
            $keyList['SMS_'.$sessionID] = true;
        $this->mc->set('SMS_KEYLIST', $keyList, 0);
    }

    // key used on memcached server for this session
    public function getKey($sessionID=NULL) {
        if (empty($sessionID))
            $sessionID = $this->sessionH->sessionID;
        return 'SMS_'.$sessionID;
    }

    // reverse
    public function getSessionID($sKey) {
        if (strlen($sKey) == 32+4)
            return substr($sKey, 4);
        else
            return NULL;
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

    function getFullKeyList() {
        return $this->mc->get('SMS_KEYLIST');
    }


    /**
     * garbage collect. clear all sessions older than $maxLifeTime seconds 
     *
     */
    function garbageCollect($maxLifeTime) {

        // memcached maintains ttl on the actual session data itself
        // all we do here is maintain the list of active sessions
        $mainKeyList = $this->mc->get('SMS_KEYLIST');
        $skeyList = array_keys($mainKeyList);

        if (sizeof($skeyList) == 0)
            return;

        // request all of the sessions we have in our keylist
        // XXX note, this returns all of the data for all of the keys?? an isset() is what we need, not a get*
        // but that's not available?
        $sVarList = $this->mc->getMulti($skeyList);

        // sVarList will contain all of the keys that still exist
        // calculate which we need to purge
        $remList = array_diff($skeyList, array_keys($sVarList));

        /*
        var_dump($mainKeyList);
        var_dump($remList);
        var_dump($skeyList);
        var_dump($sVarList);
         *
         */

        $this->debugLog('gcing '.sizeof($remList).' keys');

        // remove from list
        foreach ($remList as $sKey) {
            unset($mainKeyList[$sKey]);
        }

        $this->mc->set('SMS_KEYLIST', $mainKeyList,0);

    }

    function clearAllSessions() {
        $keyList = $this->mc->get('SMS_KEYLIST');
        foreach ($keyList as $sKey => $d) {
            $this->mc->delete($sKey);
        }
        $this->mc->set('SMS_KEYLIST', array(),0);
    }
    
}


?>