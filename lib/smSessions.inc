<?php

/**
*  Roadsend SiteManager
*******************************************************************
*  Copyright (c) 2001-2009 Roadsend, Inc.(http://www.roadsend.com)
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

// defines
define("SM_SESSION_REGEXP","/^[0-9a-f]{32}$/");  // session regexp pattern
define("SM_MEMBER_PVAR"   ,"__SM_mpv__");           // persistent member data pvar
define("SM_PAGETRACK_PVAR", "__SM_ppv__");      //persistent last page acces var
define("SM_AUTOCOOKIE_VAR", "SMC");  // auto cookie check var

// rewrite rule operations
define('SM_REWRITEURL_ADD_ABSOLUTE_BASE', 1);

/**
 *
 * the SM_session class is used to maintain a user session, in which is can store
 * persistent information about the user currently on the site. the information it
 * stores is arbitrary and expandable - ie, any variable can be passed around as part
 * of the session.
 *
 * cookies are used, if requested, to automically create a valid session if the user
 * has logged in previously.
 *
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_session
 */
class SM_session extends SM_object {
    
    /**
     * a list of variables to keep persistent
     * @var array
     */
    var $persistentVars = array();
        
    /**
     * the current session ID
     * @var string
     */
    var $sessionID = '';
    
    /**
     * the membersystem object
     * @var SM_memberSystems
     */
    var $memberSystem = NULL;
    
    /**
     * PHP SELF variable points to current script location, relative to HTML doc root
     * @var string
     */
    var $PHP_SELF;

    /**
     * this is the current page name, without path
     */
    var $currentPage;
    
    /**
     * a variable set to thwart caching mechanisms out to destroy the idea of a dynamic page
     * @var string
     */
    var $antiCacheVar = '';
    
    /**
     * session container
     * @var SM_sessionContainer
     */
    var $sessionContainer = NULL;


    /**
     * if we received a session cookie, this is true. otherwise, we don't know if they have
     * cookies so it's false
     * @var bool
     */
    var $hasCookies = false;

    /**
     * whether this is a new session (newly created sessionID) this load or not
     */
    var $newSession = false;
    
    /**
     * an array of regex rewriting rules to apply to links created in hLink
     */
    var $urlRewriteRules = NULL;
    
    /**
     * session has been saved
     */
    var $sessionSaved = false;
    
    /**
     * constructor. initialize, define persistents
     */
    
    function SM_session() {
        
        global $SM_siteManager;
            
        // configure
        $this->_smoConfigure();
            
        // setup PHP_SELF
        $this->PHP_SELF = (isset($_SERVER['REDIRECT_URL'])) ? $_SERVER['REDIRECT_URL'] : $_SERVER['PHP_SELF'];
        $this->currentPage = basename($this->PHP_SELF);
            
        // retrieve session configuration
        $this->directive            = $this->siteConfig->getSection('sessions');
        $this->directive['pVars']   = $this->siteConfig->getSection('pVars');
        $this->directive['members'] = $this->siteConfig->getSection('members');

        // backwards compatibility < 2.5.2
        if (empty($this->directive['useCookies'])) {
            if (empty($this->directive['sessionCookie']))
                $this->directive['useCookies'] = 'none';
            else
                $this->directive['useCookies'] = 'auto';
        }
        
        // check to see if we are in cli mode and if so, turn off cookies (gollum 6/18/2004)
        if (php_sapi_name() === 'cli') {
            $this->directive['useCookies'] = 'none';
            if ($GLOBALS['SM_CLI_SESSION'] !== true)
                $this->directive['containerType'] = 'none';
            $this->debugLog("useCookies, session type set to 'none' in CLI mode");
        }
        
        // setup session container
        $this->_loadContainer();
            
        // try cookies first
        if ($this->_useCookies()) {
                
            // yes we should check the client for a session cookie
            $sC = $this->directive['sessionCookie'];
            $sCV = $this->inVarH->getCookieVar($sC);
                
            // now if it's a valid session, use that
            if (!empty($sCV)&&(preg_match(SM_SESSION_REGEXP,$sCV))) {
                $this->hasCookies = true;
                $this->sessionID = $sCV;
                $this->debugLog("valid session ID obtained through cookie: ".$this->sessionID);
            }
                
        }
                
        // if we don't have it yet...
        // setup local sessionID variable. it will attempt to retrieve it via GET/POST
        // but only if we don't *require* cookies
        if (empty($this->sessionID) && ($this->directive['useCookies'] != 'require')) {
            $this->sessionID = $this->inVarH->getVar($this->directive['sessionIDName']);
            // now if it's a valid session, use that
            if (!empty($this->sessionID)&&(preg_match(SM_SESSION_REGEXP,$this->sessionID))) {
                $this->debugLog("valid session ID obtained through GET/POST");
                // since we only support loading a session through a cookie, we try to set one then reload
                $this->reloadPage(NULL,NULL,array($this->directive['sessionIDName']),true);
            }
        }

        // if we *still* don't have it, we'll be setting up a new session
        if (!preg_match(SM_SESSION_REGEXP, $this->sessionID)) {

            // new session
            $this->_createSession();

        }
                
        // now add the name of the session ID variable as a persistent. this one is required.
        $sN = $this->directive['sessionIDName'];
        $this->addPersistent($sN,$this->sessionID);
            
        if ($this->directive['members']['usePersistentMembers']) {
            // add persistent member pvar
            $this->addPersistent(SM_MEMBER_PVAR);
            // for security, dont let user POST or GET this variable
            $this->inVarH->addBadVar(SM_MEMBER_PVAR);
        }
            
        //If we are tracking the last page access time,
        //add our persistant var to the list.
        if ($this->directive['trackLastPageAccess']) {
            // add persistent member pvar
            $this->addPersistent(SM_PAGETRACK_PVAR);
            // for security, dont let user POST or GET this variable
            $this->inVarH->addBadVar(SM_PAGETRACK_PVAR);
        }
            
        // read in persistents from siteconfig
        if (is_array($this->directive['pVars'])) {
            foreach ($this->directive['pVars'] as $pV=>$val) {
                $this->addPersistent($pV,$val);
            }
        }
            
        // double check - we need at least 1 persistant (session ID)
        if (sizeof($this->persistentVars) == 0)
            $this->fatalErrorPage("persistentVars not set for session config");
            
        // anti cache?
        if (!empty($this->directive['forceAntiCacheVar'])) {
            $this->inVarH->addInVarP($this->directive['forceAntiCacheVar']);
            $this->inVarH->setInVar($this->directive['forceAntiCacheVar'], rand());
        }
        
    } // end constructor

    /**
     * utility function
     */
    function _useCookies() {
        return ($this->directive['useCookies'] == 'auto' ||
                $this->directive['useCookies'] == 'require');
    }
    
    /**
     * load the selected session container
     */
    
    function _loadContainer() {
        
        // backwards compatibility
        if ($this->siteConfig->getVar('compatibility','2.2.x')) {
            // 2.2.0 functionality: check for useDatabase in sessions section
            if (isset($this->directive['useDatabase']) && $this->directive['useDatabase']) {
                $this->directive['containerType'] = 'database';
            }
        }

        // try setup the session container
        SM_loadSessionContainer($this->directive['containerType']);
            
        // instantiate
        $cName = 'SM_sessionContainer_'.$this->directive['containerType'];
        if (!class_exists($cName)) {
            $this->fatalErrorPage("could not load requested session container: $cName");
        }
        $this->sessionContainer = new $cName($this, $this->directive['containerType']);
        
    }
    
    /*************************** PERSISTENTS (session vars) **********************************/
    
    /**
     * add a persistent var to the list
     * @param string $varName the variable name to keep persistent
     * @param string $varValue default value for this variable
     */
    function addPersistent($varName, $varValue='') {
        // if it's not already in the list, add it
        if (!in_array($varName, $this->persistentVars)) {
            $this->persistentVars[] = $varName;
            $this->inVarH->addSessionVar($varName, $varValue);
        }
        
        // special hook for session container 'none'
        // need this to pull value of new persistent into sessions from $_REQUEST
        if ($this->directive['containerType'] == 'none')
            $this->sessionContainer->loadSession();
        
    }
    
    /**
     * wrapper function for addPersistent
     */
    function addSessionVar($varName, $varValue) {
        $this->addPersistent($varName, $varValue);
    }
    
    /**
     * set the current value of a persistent variable.
     * using this function will cause all futher calls to hLink, puLink and
     * SmartForms to use the value you specify to keep persistent. It will also
     * save this value as the new value in the database, if that's turned on.
     *
     * @param string $varName persistent variable to change
     * @param string $varValue value for this variable. leave blank to reset
     */
    function setPersistent($varName, $varValue='') {
        if (!in_array($varName, $this->persistentVars)) {
            $this->debugLog("setPersistent called for non-session variable $varName");
            return;
        }
        $this->inVarH->setSessionVar($varName, $varValue);
    }
    
    /**
     * wrapper function for setPersistent
     */
    function setSessionVar($varName, $varValue='') {
        $this->setPersistent($varName, $varValue);
    }
    
    /**
     * get current value of a session variable
     * @param string $varName persistent variable to retrieve
     */
    function getPersistent($varName) {
        return $this->inVarH->getSessionVar($varName);
    }
    
    /**
     * get current value of a session variable
     * @param string $varName persistent variable to retrieve
     */
    function getSessionVar($varName) {
        return $this->inVarH->getSessionVar($varName);
    }
    
    /**
     * return the current list of variables to be kept persistent
     * @return array list of persistent variable name
     */
    function getPersistentList() {
        return $this->persistentVars;
    }
    
    
    //*************************** LINKS **********************************

    /**
     * @param string $regex a pcre regex rule to match a URL against
     * @param array  $rule an array, where $rule[0] specifies the rewrite operation,
     *                     and further entries in the array of operation specific
     */
    function addURLRewriteRule($regex, $rule) {
        $this->urlRewriteRules[$regex] = $rule;
    }
    
    /**
     * generate a url encoded list of persistents suitable for passing in a URL
     * @param array $eList a list of variable to exclude from the link
     * @param string $retType return type: FORM or URL
     */
    function _urlEncodePersistents($eList,$retType) {
        
        // if we're using a database, we just need sessionID (possibly) since the persistents
        // are kept in the database
        if ($this->directive['containerType'] != 'none') {
                
            // if forceSessionID isn't on, just return (rely on cookie)
            if ($this->directive['forceSessionID'] == false)
                return '';
                
            // otherwise, add sessionID
            if ($retType == 'form')
                return "<input type=\"hidden\" name=\"".$this->directive['sessionIDName']."\" value=\"".$this->sessionID."\">\n";
            else
                return $this->directive['sessionIDName'].'='.$this->sessionID;
                
        }
            
        // for each variable we want to keep persistent
        $pURL = '';
        foreach ($this->persistentVars as $p) {
                
            // don't include exclude list
            if (isset($eList) && is_array($eList) && in_array($p, $eList))
            continue;
                
            $pVal = $this->inVarH->getSessionVar($p);
            if (isset($pVal)&&($pVal != '')) {
            if ($retType == 'form') {
                $pURL .= "<input type=\"hidden\" name=\"".$p."\" value=\"".$pVal."\">\n";
            }
            else {
                $pURL .= $p.'='.urlencode($pVal).'&';
            }
            }
        }
            
        if (($retType == 'url')&&(substr($pURL, strlen($pURL)-1,1) == '&'))
            $pURL = substr($pURL, 0, strlen($pURL)-1);
            
        return $pURL;
        
    }
    
    /**
     * generate a url encoded list of module inVars suitable for passing in a URL
     * @param array $eList a list of variable to exclude from the link
     * @param string $retType return type: FORM or URL
     */
    function _urlEncodeInVars($eList,$retType) {
        
        global $SM_siteManager;
            
        $ivArray = $SM_siteManager->_getModuleInVars();
            
        if (!is_array($ivArray))
            return;
            
        $ivURL = '';
        foreach ($ivArray as $p) {
                
            // don't include exclude list
            if (isset($eList) && is_array($eList) && in_array($p, $eList))
            continue;
                
            $pVal = $this->inVarH->getVar($p);
            if (isset($pVal)&&($pVal != '')) {
            if ($retType == 'form') {
                $ivURL .= "<input type=\"hidden\" name=\"".$p."\" value=\"".$pVal."\">\n";
            }
            else {
                $ivURL .= $p.'='.urlencode($pVal).'&';
            }
            }
        }
            
        if (($retType == 'url')&&(substr($ivURL, strlen($ivURL)-1,1) == '&'))
            $ivURL = substr($ivURL, 0, strlen($ivURL)-1);
            
        return $ivURL;
        
    }
    
    
    /**
     * this function returns a list of variables we want added to all links and redirects
     * it will NOT return the begining ? or & needed after the scriptname
     * @param array $eList a list of variable to exclude from the link
     */
    function getSessionVars($eList=NULL) {
        
        $pLink  = $this->_urlEncodePersistents($eList,'url');
        $ivLink = $this->_urlEncodeInVars($eList,'url');
            
        if ($ivLink != '') {
            if ($pLink != '')
            $urlLink = $pLink.'&'.$ivLink;
            else
            $urlLink = $ivLink;
        }
        else {
            $urlLink = $pLink;
        }
            
        // update SM_sessionVars while we're here
        global $SM_sessionVars;
        $SM_sessionVars = $urlLink;
            
        return $urlLink;
        
    }
    
    /**
     * pull and return a list of variable names from a URL link
     * @return array list of variable names
     */
    function _getLinkVars($link) {
        
        if (strstr($link, '?')) {
            $link = substr($link, strpos($link,'?')+1);
        }
            
        $lvList = array();
        $vList = explode('&',$link);
        foreach ($vList as $v) {
            /*
            // FIXME: faster to avoid preg?
            if (preg_match('/^(\w+)=.*$/',$v,$m)) {
            $lvList[] = $m[1];
            }
            */
            $ar = explode('=',$v);
            $lvList[] = $ar[0];
        }
            
        return $lvList;
        
    }
    
    /**
     * this function builds a URL link, and adds the session ID if available
     * @param string $link the URL location to link to
     * @param array $exList a list of variables to specifically NOT include in the URL
     * @param string $anchor optional anchor to add to URL (ie, #anchor at the end of URL)
     */
    function uLink($link, $exList=NULL, $anchor=NULL) {
        
        // is link blank? use default script
        if ($link{0} == '?') {
            $link = $this->directive['defaultScript'].$link;
        }

        // if link already has an anchor, use it
        if (($aPos = strrpos($link, '#')) !== false) {
            if ($anchor == NULL) {
                $anchor = substr($link, $aPos);
            }
            $link = substr($link, 0, $aPos-strlen($link));
        }

        // do we have an absoluteLinkBase?
        if ($this->directive['absoluteLinkBase']) {
            $link = $this->directive['absoluteLinkBase'].$link;
        }
        
        // include default search location
        (strstr($link,$this->directive['scriptWithVars'])) ? $connect = '&' : $connect = '?';
        
        // generate variable exclude list
        $eList = $this->_getLinkVars($link);

        // merge in user specified exclude
        if (is_array($exList) && sizeof($exList)) {
            $eList = array_merge($eList, $exList);
        }
        
        // get session vars
        $sVars = $this->getSessionVars($eList);
        
        // if none, clear it
        if (empty($sVars))
            $connect = '';
        
        
        $a='';
        if ($anchor != NULL) {
            if ($anchor{0} != '#') {
                $anchor = '#'.$anchor;
            }
            $a = $anchor;
        }
        
        $output = $link.$connect.$sVars.$a;

        /** URL REWRITING **/
        if ($this->urlRewriteRules) {
            foreach ($this->urlRewriteRules as $regex => $rules) {
                if ($regex{0} == '!') {
                    $match = !preg_match(substr($regex, 1),$output);
                }
                else {
                    $match = preg_match($regex, $output);
                }
                if ($match) {
                    // matched a rewrite regex, handle rewrite operation
                    switch ($rules[0]) {
                        case SM_REWRITEURL_ADD_ABSOLUTE_BASE:
                            $output = $rules[1].$output;
                            break;
                        default:
                            $this->debugLog(print_r($rules,true));
                            $this->debugLog('unknown rewrite rule');
                            break;
                    }
                }
            }
        }
        
        return $output;
        
        
    }
    
    /**
     * this function creates a link to the desired URL, and adds the session ID if available
     * @param string $link the URL location to link to
     * @param string $text the text to link (ie, the text between <A></A>
     * @param string $class the CLASS (ie, style sheet element) to use for this text
     * @param string $extra extra data to stick into the <A> tag. may be CLASS, TARGET, etc.
     * @param array $exList a list of variables to specifically NOT include in the URL
     * @param string $anchor optional anchor to add to URL (ie, #anchor at the end of URL)
     */
    function hLink($link, $text, $class='', $extra='', $exList=NULL, $anchor=NULL) {
        
        // class?
        ($class != '') ? $cText = 'class="'.$class.'"' : $cText = '';
            
            
        $output = "<a href=\"".$this->uLink($link, $exList,$anchor)."\" $cText $extra>$text</a>";
            
        return $output;
        
    }
    
    /**
     * this function returns a hidden field in a form that includes persistent variables
     * @param bool $eList list of variables to exclude from the HIDDENs
     * @return string a list of hidden INPUT's for inclusion in a form
     */
    function formID($eList) {
        
        return $this->_urlEncodePersistents($eList,'form').$this->_urlEncodeInVars($eList,'form');
        
    }
        
    
    /**
     * transfer to another page, keeping session variables in tact
     * @param string $location the URL to transfer to
     * @param array $varList a list of variables (key/val) to include in the reload of the page
     * @param array $exList a list of variables to specifically NOT include in the URL
     * @param string $anchor optional anchor link to add to URL
     */
    function transferToPage($location, $varList=NULL, $exList=NULL,$anchor=NULL) {
        $this->reloadPage($varList, $location, $exList,false,$anchor);
    }
    
    /**
     * reload the current page, keeping session variables intact
     *
     * note that if keepAllScriptVars is active, any variables passed via $varList will
     * override variables to the script
     *
     * @param array $varList a list of variables (key/val) to include in the reload of the page
     * @param string $location the URL to transfer to. (defaults to current page)
     * @param array $exList a list of variables to specifically NOT include in the URL
     * @param bool $keepAllScriptVars if true, all GET and POST variables will be reproduced in the reloaded page
     * @param string $anchor optional anchor link to add to URL. will add the # prefix if not there
     * @param bool $saveSession will save the session automatically by default
     */
    function reloadPage($varList=NULL, $location=NULL, $exList=NULL, $keepAllScriptVars=false,$anchor=NULL,$saveSession=true) {
        
        // Make sure add is blank (just in case)
        $add = '';
        $addList = array();
            
        // Loop thru all the values passsed to us
        if (isset($varList) && is_array($varList)) {
            foreach($varList as $key => $val) {
                // if we're adding it here, don't add it from session vars
                $exList[] = $key;
                $add .= "$key=" . urlencode($val) . '&';
            }
            $add = substr($add,0,-1);
        }
            
        // keep all script vars?
        if ($keepAllScriptVars) {
            if (!empty($add)) {
            $add .= '&';
            }
            if (sizeof($_GET)) {
                foreach ($_GET as $key => $val) {
                    if ( (!in_array($key, $addList)) && (!in_array($key, $exList)) ) {
                    $add .= "$key=".urlencode($val).'&';
                    $addList[] = $key;
                    }
                }
            }
            elseif (sizeof($_POST)) {
                foreach ($_POST as $key => $val) {
                    if ( (!in_array($key, $addList)) && (!in_array($key, $exList)) ) {
                    $add .= "$key=".urlencode($val).'&';
                    $addList[] = $key;
                    }
                }
            }
            $add = substr($add,0,-1);
        }
            
        // set location
        if ($location == NULL) {
            $location = $this->PHP_SELF;
        }
            
        // save the session
        if ($saveSession) {
            $this->saveSession();
        }
            
        // get session vars
        $sVars = $this->getSessionVars($exList);
            
        // build a tidy link
        if (empty($add) && empty($sVars)) {
            $link = $location;
        }
        elseif (!empty($sVars)) {
            $link = $location.'?'.$sVars;
            if (!empty($add)) {
            $link .= '&'.$add;
            }
        }
        elseif (!empty($add)) {
            $link = $location.'?'.$add;
        }
            
        $a='';
        if ($anchor != NULL) {
            if ($anchor{0} != '#') {
            $anchor = '#'.$anchor;
            }
            $a = $anchor;
        }

        header("Location: ".$link.$a);
        exit;
        
    }
    
    
    /**
     * returns the current valid session ID
     */
    
    function getSessionID() {
        
    return $this->sessionID;
        
    }
    
    
    /*************************** MEMBERS **********************************/
    
    /**
     * load the selected session member system
     */
    
    function _loadMemberSystem() {
        
        // load base member system component
        SM_loadComponent('SM_memberSystem');
            
        // backwards compatibility
        if ($this->siteConfig->getVar('compatibility','2.2.x')) {
            // 2.2.0 functionality: check for useDatabase in sessions section
            if (!isset($this->directive['members']['memberSystemType'])) {
            $this->directive['members']['memberSystem'] = 'default';
            }
        }
            
        // try to load the member system requested
        SM_loadMemberSystem($this->directive['members']['memberSystemType']);
            
        // instantiate
        $cName = 'SM_memberSystem_'.$this->directive['members']['memberSystemType'];
        if (!class_exists($cName)) {
            $this->fatalErrorPage("could not load requested member system: $cName");
        }
            
        // create
        $this->memberSystem = new $cName($this->directive['members']['memberSystemType']);
            
        // add some basic directives
        $this->memberSystem->addDirective('loginTTL',$this->directive['members']['loginTTL']);
        $this->memberSystem->addDirective('inactiveTTL',$this->directive['members']['inactiveTTL']);
        $this->memberSystem->addDirective('maintainTables',$this->directive['members']['maintainTables']);
        $this->memberSystem->addDirective('usePersistentMembers',$this->directive['members']['usePersistentMembers']);
        
    }
    
    /**
     * check the status of the user in this session
     *
     * @return bool true if the current session is a valid member session
     */
    function isMember() {
        if ($this->directive['useMemberSystem'] && $this->hasCookies)
            return $this->memberSystem->isMember();
    }
    
    /**
     * check the status of the user in this session
     *
     * @return bool true if the current sessions is a non-member session
     */
    function isGuest() {
        if ($this->directive['useMemberSystem'])
            return $this->memberSystem->isGuest();
    }
    
    /**
     * retrieve information on the current logged in member
     * @return array a key/value list of data from the members database
     */
    function getMemberData() {
        if ($this->directive['useMemberSystem'])
            return $this->memberSystem->getMemberData();
    }
    
    /**
     * attempt to create a member session with the username/password combo provided
     * just pass it on the to member system here, if it's active
     * @param string $userName the username to use for authentication
     * @param string $passWord the password to use for authentication
     * @return bool true if the user authenticated succesfully, false otherwise
     */
    function attemptLogin($userName, $passWord) {
        if ($this->directive['useMemberSystem'] && $this->hasCookies)
            return $this->memberSystem->attemptLogin($userName, $passWord);
        else
            return false;
    }
    
    
    /**
     * attempt to remove a current valid member session
     */
    function attemptLogout() {
        if ($this->directive['useMemberSystem'])
            return $this->memberSystem->attemptLogout();
    }
    
    /**
     * flush and reload member information from member system
     * call this if you update the member database
     */
    function flushMemberInfo() {
        
        if (!$this->directive['useMemberSystem'])
            return;
            
        global $SM_siteManager;
        unset($this->memberSystem->memberData);
        $SM_siteManager->sessionH->setPersistent(SM_MEMBER_PVAR);
            
        // try to recache member info
        if ($this->memberSystem->isMember) {
            $this->memberSystem->attemptLoadMember();
                
            // if they are using persistent members, update var
            if ($this->memberSystem->directive['members']['usePersistentMembers']) {
            $SM_siteManager->sessionH->setPersistent(SM_MEMBER_PVAR, $this->memberSystem->memberData);
            }
        }
            
    }
    
    /**
     * return a reference to the current memberSystem object
     * @return SM_memberSystem member system object
     */
    function getMemberSystem() {
        return $this->memberSystem;
    }
    
    /*************************** SESSION LOADING/SAVING **********************************/
    
    /**
     * this function creates a new session for member members_idxNum, using cookies if desired
     * @return string new 32 character MD5'd session string
     */
    function _createSession() {

        // new
        $this->newSession = true;
        
        // create a session ID. hopefully unique!
        $this->sessionID = md5(uniqid(rand(),true));
            
        // a little dump info for debug
        $this->debugLog("created new session (either none passed, or invalid)");
            
        // notify container of new session
        $this->sessionContainer->createSession();
        
        // return for caller to use
        return $this->sessionID;
        
    }
    
    /**
     * private function to set the session cookie
     *
     */
    function _setSessionCookie() {
        
        // if we are to use cookies, save it here
        if ($this->_useCookies() && $this->directive['sessionCookie'] != '') {
            if ($this->directive['cookieTTL'] > 0) {
                $cTTL = time() + $this->directive['cookieTTL'];
            }
            else {
                $cTTL = 0;
            }
            setcookie($this->directive['sessionCookie'], $this->sessionID, $cTTL, $this->directive['cookiePath']);
        }

    }
    
    /**
     * clear out a specific session, removing all session information for this sessionID, and removing
     * any session cookie
     * @param string $sessionID the session ID to clear. if not passed, defaults to current session
     * @param bool $removeMemberSession if true, and memberSystem is in use, will remove member session entry too
     */
    function clearSession($sessionID=NULL, $removeMemberSession=true) {
        
        if ($sessionID == NULL) {
            $sessionID = $this->sessionID;
        }
        $this->sessionContainer->clearSession($sessionID);
            
        // if there is a member session, clear that as well
        if ($removeMemberSession && $this->directive['useMemberSystem']) {
            $this->memberSystem->clearMemberSession($sessionID);
        }
            
        // reset cookie
        if ($this->_useCookies() && $this->directive['sessionCookie'] != '') {
            setcookie($this->directive['sessionCookie']);
        }
        
    }
    
    /**
     * this function is called when the page is complete, and will save all session information
     */
    function saveSession() {

        if ($this->sessionSaved || php_sapi_name() == 'cli')
            return;
        
        $this->sessionSaved = true;            
        
        //if we're tracking last page access, set it to now()
        if ($this->directive['trackLastPageAccess']) {
            $this->setPersistent(SM_PAGETRACK_PVAR, time());
        }
            
        // use database structure to save and load persistent variable ?
        if ($this->hasCookies)
            $this->sessionContainer->saveSession($this->persistentVars);
            
        if ($this->_useCookies() && $this->directive['sessionCookie'] != '') {
            $this->_setSessionCookie();
        }
            
    }
    
    /**
     * run the session system.
     * after this session exits, either a member or guest session will exist
     */
    function runSession() {
        
        // we don't run sessions in command line mode
        if (php_sapi_name() == 'cli') {
            $this->directive['useMemberSystem'] = false;
            return;
        }

        // garbage collect?
        if ($this->directive['garbageCollect']) {

            $prob = mt_rand(0, $this->directive['gcDivisor']);
            if ($prob <= $this->directive['gcProbability']) {
                $this->sessionContainer->garbageCollect($this->directive['gcMaxLifeTime']);
            }
            
        }
        
        // use container to save and load persistent variables
        // if it returns false, the sessionID was invalid and we should create
        // a new one
        if (!$this->newSession && !$this->sessionContainer->loadSession())
            $this->_createSession();            
            
        // by the time we get here, we have a session ID and know if we're a guest or member
        
        // setup global $SM_sessionVars variable for the rest of the scripts to use
        global $SM_sessionVars;
        $SM_sessionVars = $this->getSessionVars();
            
        // setup the memberSystem if it's turned on
        if ($this->directive['useMemberSystem']) {
                
            // startup member system
            $this->_loadMemberSystem();
                
            // load up the member info if this is a member session
            if ($this->hasCookies && !$this->newSession)
                $this->memberSystem->attemptLoadMember();
                
            // if a user defined function was specified for loading additional member information,
            // run here
            if ($this->memberSystem->isMember &&
                isset($this->directive['memberLoadFunction']) &&
                function_exists($this->directive['memberLoadFunction']) ) {
                $mlF = $this->directive['memberLoadFunction'];
                $mlF($this->memberSystem->memberData);
            }
                
            // maintain tables
            $this->memberSystem->maintainTables();
                
        }
        
    }
    
    /*************************** DEBUG **********************************/
    
    /**
     * dump debug session information
     *
     */
    function dumpInfo() {
        
        $op = "current session ID: $this->sessionID<br /><br />\n";
        $op .= $this->getClassLog();
            
        $op .= $this->dumpDirectives();
            
        $op .= "persistant variables:<br />\n";
        $op .= "<ul>";
            
        foreach ($this->persistentVars as $pVars) {
            $op .= $pVars."<br />\n";
        }
            
        $op .= "</ul><br />";
            
        if ($this->directive['useMemberSystem']) {
            $op .= "member system is on:<br /><br />";
            $op .= $this->memberSystem->dumpInfo();
        }
        else {
            $op .= "member system is off<br /><br />";
        }
            
        return $op;
        
    }
    
}


/**
 * load (include) a session container
 * @param string $cName the container to load
 * @returns bool if false, couldn't find it. true means successful load
 */
function SM_loadSessionContainer($cName) {
    
    // if already here, don't reload
    if (class_exists('SM_sessionContainer_'.$cName))
        return true;
    
    // globalize
    global $SM_siteManager;
    
    // find file
    $cName = $SM_siteManager->findSMfile($cName.'_container', 'sContainers', 'inc', true);
    
    // load it up
    // include_once will not let duplicate modules be loaded
    include_once($cName);
    
    // all good and loaded, return
    return true;
    
}

/**
 * load (include) a member system
 * @param string $sName the system to load
 * @returns bool if false, couldn't find it. true means successful load
 */
function SM_loadMemberSystem($sName) {
    
    // if already here, don't reload
    if (class_exists('SM_memberSystem_'.$sName))
    return true;
    
    // globalize
    global $SM_siteManager;
    
    // find file
    $sName = $SM_siteManager->findSMfile($sName.'_memberSystem', 'memberSystems', 'inc', true);
    
    // load it up
    // include_once will not let duplicate modules be loaded
    include_once($sName);
    
    // all good and loaded, return
    return true;
    
}

?>
