<?php

/**
 *  a class to handle member logins with LDAP authentication
 * 
 */



// perhaps some way to load that base class with more elegance?
global $SM_siteManager;
$sName = $SM_siteManager->findSMfile('default_memberSystem', 'memberSystems', 'inc', true);
require_once($sName);

class SM_memberSystem_ldap extends SM_memberSystem_default {

    /**
     * attempt to create a member session with the username/password combo provided
     * @param $userName (string) the username to use for authentication
     * @param $passWord (string) the password to use for authentication
     * @return (bool) true if the user authenticated succesfully, false otherwise
     */
    function attemptLogin($userName, $passWord) {

        if (!$passWord) {
            return false;
        }

        // define some configured variables we'll be using
        $uIDField               = $this->directive['uIDField'];

        // We're goign to be using LDAP authentication, so load config:
        global $SM_siteManager;

        $ldapParam = $SM_siteManager->siteConfig->getSection('ldap');
        list ($userId, $domain) = explode ('@', $userName );

        if (!$domain || $domain == $ldapParam['domainString']) {
            $ds = ldap_connect($ldapParam['host'], $ldapParam['port']);     //	Must be a valid LDAP Server
            if ($ds) {
                $authenticated = ldap_bind($ds, "uid=".$userId.','.$ldapParam['baseDN'], $passWord);
            } else {
                SM_fatalErrorPage("Unable to connect to DS");
            }
        }

        if ($authenticated) {
            $rr = $this->getUserData($userName);
        } 

        // if $rr has data, it's a good session
        if ($rr) {
            // create a member session in database
            $this->_createMemberSessionEntry($rr[$uIDField]);

            // finally, indicate they are now a member
            // replace guest/member with more extensive role system?
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
        } else {

            // invalid login
            return false;
        }
    }


    function _recursiveConvertEnc (&$value, $key = NULL, $encTo = 'ISO-8859-1', $encFrom = 'UTF-8' ) {
        if (is_array($value)) {
            foreach (array_keys($value) as $key) {
                $this->_recursiveConvertEnc ($value[$key], $key, $encTo, $encFrom);
            }
        } else {
            if ($key == 'count') {
                unset($key);
            }
            $value = mb_convert_encoding($value, $encTo, $encFrom);
        }
    }
    
    function getUserData($userId) {

        // try to fetch data from LDAP:
        global $SM_siteManager;
        $ldapParam = $SM_siteManager->siteConfig->getSection('ldap');
        list ($ldap_userId, $domain) = explode ('@', $userId);

        if ( !$domain || $domain == $ldapParam['domainString']  ) {
            $ds = ldap_connect($ldapParam['host']);
            if ($ds) {
                ldap_bind($ds);
            } else {
                SM_fatalErrorPage("Unable to connect to DS");
            }
        }

        // Attributes we fetch from LDAP:
        $attrs = array (

            // Basic stuff:
            'dn', 
            'uid', 
            'sn', 
            'sn;lang-el', 
            'givenname', 
            'givenname;lang-el', 

            // Phones and faxes:
            'telephonenumber',          // voice phone number
            'facsimiletelephonenumber', // fax

            // Email-related:
            'mail',                     // used by qmail - must be SINGLE valued
            'mailalternateaddress',     // all other email addresses go here.

            'title',            
            'employeenumber',   
            'employeetype',     
            'departmentnumber', 
            'businesscategory', 
            'roomnumber',       
            'labeleduri'        
       
        );


        $searchResult = ldap_search($ds, $ldapParam['baseDN'], "uid=".$ldap_userId, $attrs);

        if ($searchResult) {
            $info = ldap_get_entries($ds, $searchResult);
            if ($info['count']) {
                $convertTo      = $this->directive['convertTo'];
                $convertFrom    = $this->directive['convertFrom'];
                $this->recursiveConvertEnc($info[0],NULL,$convertTo, $convertFrom);
                $userDataRow = $info[0];
            }
        }
        
        return $userDataRow;
    }

}

?>