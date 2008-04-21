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
 *  a class to handle member logins
 *  works with session code
 *  uses database for storage
 *  
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_member
 */
class SM_memberSystem_default extends SM_memberSystem {

    /**
    * ID of the database connection we're using 
    * @var string
    */
    var $dbID = '';

    /**
     * setup the member system
     * @param string $sessionID the current session ID, as passed by the session engine
     */
    function memberSystemConfig() {

        // backwards compatibility
        if ($this->siteConfig->getVar('compatibility','2.2.x')) {
            // get values from members directive in sessions
            $this->configure($this->sessionH->directive['members']);
            $this->debugLog("warning: using backward compatibility mode - you should update to ".SM_VERSION.' compatible');
        }

        if ($this->directive['dataBaseID'] == '')
            $this->directive['dataBaseID'] = 'default';

        $this->dbID = $this->directive['dataBaseID'];

        $this->directive['dbType']  = $this->siteConfig->getVar('db','dbType',false,$this->dbID);

        if (!isset($this->dbHL[$this->dbID])) {
            $this->fatalErrorPage("database connection ID '$this->dbID' does not exist. check dataBaseID in your config.");
        }

    }

    /**
     * attempt to create a member session with the username/password combo provided
     * @param string $userName the username to use for authentication
     * @param string $passWord the password to use for authentication
     * @return bool true if the user authenticated succesfully, false otherwise
     */
    function attemptLogin($userName, $passWord) {

        if (!$this->sessionH->hasCookies)
            return false;
        
        // define some configured variables we'll be using
        $memberTable            = $this->directive['memberTable'];
        $dbType                 = $this->directive['dbType'];
        $uIDField               = $this->directive['uIDField'];
        $userNameField          = $this->directive['userNameField'];
        $passWordField          = $this->directive['passWordField'];

	$quoteduserName = $this->dbHL[$this->dbID]->quote($userName);
	$quotedpassWord = $this->dbHL[$this->dbID]->quote($passWord);

        switch ($dbType) {
        case 'pgsql':
            $SQL = "SELECT
                        *
                    FROM
                        \"{$memberTable}\"
                    WHERE
                        \"{$userNameField}\" = $quoteduserName AND
                        \"{$passWordField}\" = $quotedpassWord
                   ";
            break;
        case 'oci8':
        case 'mysql':
            $SQL = "SELECT
                        *
                    FROM
                        {$memberTable}
                    WHERE
                        {$userNameField} = $quoteduserName AND
                        {$passWordField} = $quotedpassWord
                   ";
            break;
        default:
            $this->fatalErrorPage("you are attemping to use member sessions with an unsupported database type: ($dbType)");
            break;
        }

        // issue query
        $rh = $this->dbHL[$this->dbID]->query($SQL);
        $this->dbErrorCheck($rh);

        $rr = $rh->fetch();

        // if $rr has data, it's a good session
        if ($rr) {

            // create a member session
            $this->_createMemberSessionEntry($rr[$uIDField]);

            // finally, indicate they are now a member
            $this->isGuest      = false;
            $this->isMember     = true;
            $this->memberData   = $rr;
            $this->memberData['TTL']    = time();

            // if they are using persistent members, update var
            if ($this->directive['usePersistentMembers']) {
                global $SM_siteManager;
                $SM_siteManager->sessionH->setPersistent(SM_MEMBER_PVAR, $this->memberData);
            }

            return true;
            
        }
        else {
            // invalid login
            return false;
        }


    }


    /**
     * attempt to remove a valid member session, based on current sessionID
     */
    function attemptLogout($sessionID=NULL) {
        
        if (!empty($sessionID)) {
            $sID = $sessionID;
        }
        else {
            $sID = $this->sessionID;
        }
        
        // define some configured variables we'll be using
        $memberSessionsTable    = $this->directive['memberSessionsTable'];
        $dbType                 = $this->directive['dbType'];
        $sIDField               = $this->directive['sIDField'];

        switch ($dbType) {
        case 'pgsql':
            $SQL = "DELETE
                    FROM
                        \"{$memberSessionsTable}\"
                    WHERE
                        \"{$sIDField}\" = '$sID'
                   ";
            break;
        case 'oci8':
        case 'mysql':
            $SQL = "DELETE
                    FROM
                        {$memberSessionsTable}
                    WHERE
                        {$sIDField} = '$sID'
                   ";
            break;
        default:
            $this->fatalErrorPage("you are attemping to use member sessions with an unsupported database type: ($dbType)");
            break;
        }

        // issue query
        $rh = $this->dbHL[$this->dbID]->query($SQL);
        $sessionRemoved = $rh->rowCount();
        $this->dbErrorCheck($rh);

        // member session removed
        $this->isGuest  = true;
        $this->isMember = false;
        unset($this->memberData);

        // if using persistent member data, unset that persistent
        if ($this->directive['usePersistentMembers']) {
            global $SM_siteManager;
            $SM_siteManager->sessionH->setPersistent(SM_MEMBER_PVAR);
        }

        return (bool)$sessionRemoved;

    }

    /**
     * a member has authenticated, create a member session for them
     * @param string $uID the unique global user ID for the user to login
     */
    function _createMemberSessionEntry($uID) {

        // define some configured variables we'll be using
        $memberSessionsTable    = $this->directive['memberSessionsTable'];
        $dbType                 = $this->directive['dbType'];
        $uIDField               = $this->directive['uIDField'];
        $sIDField               = $this->directive['sIDField'];
        $dcField                = $this->directive['dcField'];

        // QUERY: REMOVE ANY CURRENT SESSIONS FOR THIS MEMBER
        switch ($dbType) {
        case 'pgsql':
            $SQL = "DELETE FROM 
                        \"{$memberSessionsTable}\"
                    WHERE
                        \"{$uIDField}\" = '$uID'
                   ";
            break;
        case 'oci8':
        case 'mysql':
            $SQL = "DELETE FROM 
                        {$memberSessionsTable}
                    WHERE
                        {$uIDField} = '$uID'
                   ";
            break;
        default:
            $this->fatalErrorPage("you are attemping to use member sessions with an unsupported database type: ($dbType)");
            break;
        }

        // issue query
        $rh = $this->dbHL[$this->dbID]->query($SQL);
        $this->dbErrorCheck($rh);

        // QUERY: ADD SESSION ENTRY
        switch ($dbType) {
        case 'pgsql':
            $SQL = "INSERT INTO 
                        \"{$memberSessionsTable}\"
                        (
                            \"{$uIDField}\",
                            \"{$sIDField}\",
                            \"{$dcField}\"
                        )
                    VALUES (
                        '$uID',
                        '$this->sessionID',
                        current_timestamp
                    )
                   ";
            break;
        case 'mysql':
            $SQL = "INSERT INTO 
                        {$memberSessionsTable}
                    SET
                        {$uIDField} = '$uID',
                        {$sIDField} = '$this->sessionID',
                        {$dcField} = NOW()
                   ";
            break;
        case 'oci8':
            $SQL = "INSERT INTO 
                        {$memberSessionsTable}
                    ({$uIDField}, {$sIDField}, {$dcField})
                    VALUES ('$uID', '$this->sessionID','".time()."')";
            break;
        }

        // issue query
        $rh = $this->dbHL[$this->dbID]->query($SQL);
        $this->dbErrorCheck($rh);

    }


    /**
     * will clean out old sessions (past their TTL) if this option is turned on
     * (maintainTables var in 'members' SECTION of GCS)
     * this really shouldn't be done by SiteManager, as it adds an extra db query
     * for every page view. it should be done externally by a cron script.
     * but, for convenience ... 
     */
    function maintainTables() {

        if (!$this->directive['maintainTables'])
            return;

        // define some configured variables we'll be using
        $memberSessionsTable    = $this->directive['memberSessionsTable'];
        $dbType                 = $this->directive['dbType'];
        $dcField                = $this->directive['dcField'];
        $loginTTL               = $this->directive['loginTTL'];

        // QUERY: REMOVE MEMBERSESSIONS PAST THEIR TTL
        switch ($dbType) {
        case 'pgsql':
            $SQL = "DELETE FROM 
                        \"{$memberSessionsTable}\"
                    WHERE
                        date_part('epoch',current_timestamp) - date_part('epoch',\"{$dcField}\") >= {$loginTTL}
                   ";
            break;
        case 'mysql':
            $SQL = "DELETE FROM 
                        {$memberSessionsTable}
                    WHERE
                        UNIX_TIMESTAMP(NOW()) - UNIX_TIMESTAMP({$dcField}) >= {$loginTTL}
                   ";
            break;
        case 'oci8':
            $SQL = "DELETE FROM 
                        {$memberSessionsTable}
                    WHERE
                        ".time()." - {$dcField} >= {$loginTTL}
                   ";
            break;
        default:
            $this->fatalErrorPage("you are attemping to use member sessions with an unsupported database type: ($dbType)");
            break;
        }

        // issue query
        $rh = $this->dbHL[$this->dbID]->query($SQL);
        $this->dbErrorCheck($rh);

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
        
        if (!$this->sessionH->hasCookies)
            return false;

        // if using persistent members, try that first
        if ($this->directive['usePersistentMembers']) {

            $mpv = SM_MEMBER_PVAR;
            $pmData = $this->inVarH->getSessionVar($mpv);
            if (is_array($pmData) && (isset($pmData['TTL']))) {

                // appears to be a valid member data structure, check TTL
                if ($this->directive['loginTTL'] > 0) {

                    $now = time();
					
                    if ($now - $pmData['TTL'] > $this->directive['loginTTL']) {                    
                        // expire data. clear the persistent
                        global $SM_siteManager;
                        $SM_siteManager->sessionH->setPersistent(SM_MEMBER_PVAR);
                        $this->debugLog("warning: valid member login persistent session found, but it was expired, not logging in. ($now - ".$pmData['TTL']." < {$this->directive['loginTTL']})");
                        return;
                    }
                    
                    $lastPageAccess = $this->inVarH->getSessionVar(SM_PAGETRACK_PVAR);
                    if(!empty($lastPageAccess) && ($this->directive['inactiveTTL'] > 0) && ($now -$lastPageAccess > $this->directive['inactiveTTL'])){
                        global $SM_siteManager;
                        $SM_siteManager->sessionH->setPersistent(SM_MEMBER_PVAR);
                        $this->debugLog("warning: valid member login persistent session found, but it was inactive too long, not logging in. ($now - $lastPageAccess < {$this->directive['inactiveTTL']})");
                        return;
                    	
                    }
                    
                }

                // good persistent member
                $this->memberData = $pmData;
                $this->isGuest    = false;
                $this->isMember   = true;

                $this->debugLog("member data loaded from a persistent variable");

                return;
            }

        }

        // define some configured variables we'll be using
        $memberTable            = $this->directive['memberTable'];
        $memberSessionsTable    = $this->directive['memberSessionsTable'];
        $dbType                 = $this->directive['dbType'];
        $loginTTL               = $this->directive['loginTTL'];
        $uIDField               = $this->directive['uIDField'];
        $sIDField               = $this->directive['sIDField'];
        $dcField                = $this->directive['dcField'];

        switch ($dbType) {
        case 'pgsql':
            $SQL = "SELECT 
                        \"{$memberTable}\".*, date_part('epoch',\"{$memberSessionsTable}\".\"{$dcField}\") AS \"TTL\"
                    FROM 
                        \"{$memberTable}\",\"{$memberSessionsTable}\"
                    WHERE
                        \"{$sIDField}\"='$this->sessionID' AND
                        \"{$memberTable}\".\"{$uIDField}\" = \"{$memberSessionsTable}\".\"{$uIDField}\"";
            break;
        case 'mysql':
            $SQL = "SELECT 
                        {$memberTable}.*, UNIX_TIMESTAMP({$memberSessionsTable}.{$dcField}) AS TTL
                    FROM 
                        {$memberTable},{$memberSessionsTable}
                    WHERE
                        {$sIDField}='$this->sessionID' AND
                        {$memberTable}.{$uIDField} = {$memberSessionsTable}.{$uIDField}";
            break;
        case 'oci8':
            $SQL = "SELECT 
                        {$memberTable}.*, {$memberSessionsTable}.{$dcField} AS TTL
                    FROM 
                        {$memberTable},{$memberSessionsTable}
                    WHERE
                        {$sIDField}='$this->sessionID' AND
                        {$memberTable}.{$uIDField} = {$memberSessionsTable}.{$uIDField}";
            break;
        default:
            $this->fatalErrorPage("you are attemping to use member sessions with an unsupported database type: ($dbType)");
            break;
        }

        // issue query
        $rh = $this->dbHL[$this->dbID]->query($SQL);
        if (empty($rh)) {
            // possibly they had an old cookie session set, ignore
            return;
        }

        $rr = $rh->fetch();
        if ($rr) {

            // good, check TTL
            if ($this->directive['loginTTL'] > 0) {

                $now = time();
                if ($now - $rr['TTL'] > $this->directive['loginTTL']) {                    
                    // expired. leave the entry there, if they have maintainTables turned on
                    // it will be removed by that function
                    $this->debugLog("warning: valid member login session found, but it was expired, not logging in. ($now - {$rr['TTL']} < {$this->directive['loginTTL']})");
                    return;
                }
            }

            $lastPageAccess = $this->inVarH->getSessionVar(SM_PAGETRACK_PVAR);
            if(!empty($lastPageAccess) && ($this->directive['inactiveTTL'] > 0) && ($now -$lastPageAccess > $this->directive['inactiveTTL'])){
                global $SM_siteManager;
                $SM_siteManager->sessionH->setPersistent(SM_MEMBER_PVAR);
                $this->debugLog("warning: valid member login persistent session found, but it was inactive too long, not logging in. ($now - $lastPageAccess < {$this->directive['inactiveTTL']})");
                return;
            	
            }
            // found a member session
            $this->memberData = $rr;

            // they are now a member
            $this->isMember = true;
            $this->isGuest  = false;

            $this->debugLog("member information loaded from database");

        }

    }

    /**
     * remove all member session entries for specified sessionID
     * @var string $sessionID session ID to remove
     */
    function clearMemberSession($sessionID) {        
        return $this->attemptLogout($sessionID);        
    }
    
    
}

?>
