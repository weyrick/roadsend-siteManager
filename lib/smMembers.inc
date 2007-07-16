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
 *  a class to handle member logins. works closely with session code
 * 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_member
 */
class SM_memberSystem extends SM_object {

    /**
     * if the current user is a guest, this is true     
     * @var bool
     */
    var $isGuest = true;

    /**
     * if the current user is a logged in member, this is true   
     * @var bool
     */
    var $isMember = false;

    /**
     * the session ID we're working with
     * @var string
     */
    var $sessionID = '';

    /**
     * member info, from member table
     * @var array
     */
    var $memberData = array();

    /** 
    * ID of the database connection we're using 
    * @var string
    */
    var $dbID = '';

    /**
     * setup the member system
     * @param string $sType the member system type
     */
    function SM_memberSystem($sType) {

        // configure
        $this->_smoConfigure();

        // current session ID
        $this->sessionID = $this->sessionH->sessionID;

        // load base directives
        $this->directive            = $this->siteConfig->getSection('memberSystem',$sType);

        // debug        
        if ($GLOBALS['SM_develState'])
            $this->debugLog("member system loaded");

        // config
        $this->memberSystemConfig();

    }

    /**
     * accessor for isMember bool
     * @return bool whether or not current user is a member
     */
    function isMember() {
        return $this->isMember;
    }
    
    /**
     * accessor for isGuest bool
     * @return bool whether or not current user is a guest
     */
    function isGuest() {
        return $this->isGuest;
    }

    /**
     * retrieve current member data
     * @return array current member data
     */
    function getMemberData() {
        return $this->memberData;
    }
    
    /**
     *  configure the member system before it runs
     */
    function memberSystemConfig() {

        // virtual

    }

    /**
     * attempt to create a member session with the username/password combo provided
     * @param string $userName the username to use for authentication
     * @param string $passWord the password to use for authentication
     * @return bool true if the user authenticated succesfully, false otherwise
     */
    function attemptLogin($userName, $passWord) {
        
        // virtual

    }


    /**
     * attempt to remove a valid member session, based on current sessionID
     */
    function attemptLogout() {
        
        // virtual

    }


    /**
     * optiona method to clean out old sessions (past their TTL) if this option is turned on
     * (maintainTables var in 'members' SECTION of GCS)
     */
    function maintainTables() {

        // virtual

    }

    /**
     * load a members information from the database, if the current
     * session ID is a member session (otherwise, user is a guest)
     *
     * favor a persistent member data load rather than a database query,
     * if that feature is turned on
     *
     */
    function attemptLoadMember() {

        // virtual

    }

    /**
     * remove all member session entries for specified sessionID
     * @var string $sessionID session ID to remove
     */
    function clearMemberSession($sessionID) {

        $this->isGuest = true;
        $this->isMember = false;
        $this->memberData = array();
        
    }
    

    /**
    * dump debug session information
    */
     
    function dumpInfo() {

        $op = "current user is a: ";
        ($this->isMember) ? $op .= "MEMBER" : $op .= "GUEST";

        $op .= "<br /><br />";

        if ($this->isMember) {
            global $_dOp;
            $_dOp = '';
            $op .= "member info:<br />";
            array_walk($this->memberData, '_ddPrint');
            $op .= $_dOp;
        }

        //$op .= "time left in session: ".($this->directive['loginTTL'] - (time() - $this->memberData['TTL']))."<br />";

        $op .= $this->dumpDirectives();
        $op .= $this->getClassLog();

        return $op;

    }

}

?>