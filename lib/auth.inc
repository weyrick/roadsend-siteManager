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
 * @package roadsend_siteManager
 */
/**
 *  authenticate a user based on username / password combo
 * 
 *  this routine mimics functionality of apache Basic authentication
 * 
 *  the username and password to authenticate against can be setup in
 *  one of two ways:
 * 
 *  1) Define global variables $authName and $authPass, with the corresponding
 *     values to authenticate against
 *  2) Define a global hash $authList setup in the following way:
 * 
 *      $authList   = array(
 *          "username1" => "password1",
 *          "username2" => "password2")
 * 
 *  Set these up before calling this function.
 * 
 *  If the user is NOT authenticated, this function will NOT return.
 *  
 * 
 * @param string $siteName the "Realm" to use. current SITEID if not set
 *  
 */
function SM_basicAuth($siteName="") {

    global $PHP_AUTH_USER, $PHP_AUTH_PW, $authName, $authPass, $authList, $SM_siteID;

    // Fix so that this works with newer versions of php and doesn't
    // break older versions of php (gollum 09/21/2004)
    if (empty($PHP_AUTH_USER) && !empty($_SERVER['PHP_AUTH_USER'])) {
        $PHP_AUTH_USER = $_SERVER['PHP_AUTH_USER'];
    }
    
    // Fix so that this works with newer versions of php and doesn't
    // break older versions of php (gollum 09/21/2004)
    if (empty($PHP_AUTH_PW) && !empty($_SERVER['PHP_AUTH_PW'])) {
        $PHP_AUTH_PW = $_SERVER['PHP_AUTH_PW'];
    }

    if ((($authName == '') || ($authPass == '')) && (!is_array($authList))) {
        SM_fatalErrorPage("the script was setup incorrectly - authName and authPass, or authList were not defined.");
    }

    if ($siteName == '') {
        $siteName = $SM_siteID;
    }

    if (!isset($PHP_AUTH_USER)) {
        Header("WWW-Authenticate: Basic realm=\"$siteName Admin\"");
        Header("HTTP/1.0 401 Unauthorized");
        SM_fatalErrorPage("You must supply a correct username / password for this page. [nup]");
        exit;
    }
    else {

        if (is_array($authList)) {
            // new multiple user, hash way 
            $noMatch = true;
            // verify username / password
            while (list($authName, $authPass) = each($authList)) {
                if ($authName == $PHP_AUTH_USER) {
                    // username ok, check password
                    if ($authPass == $PHP_AUTH_PW) {
                        $noMatch = false;
                    }
                }
            }
            if ($noMatch) {
                    Header("HTTP/1.0 401 Unauthorized");
                    SM_fatalErrorPage("You must supply a correct username / password for this page. [ui]");
                    exit;            
            }
        }
        else {

            // old fasion way
            // verify username / password 
            if ($authName != $PHP_AUTH_USER) {
                Header("HTTP/1.0 401 Unauthorized");
                SM_fatalErrorPage("You must supply a correct username / password for this page. [ui]");
                exit;
            }
            else {
                // username ok, check password
                if ($authPass != $PHP_AUTH_PW) {
                    Header("HTTP/1.0 401 Unauthorized");
                    SM_fatalErrorPage("You must supply a correct username / password for this page. [pi]");
                    exit;
                }
            }

        }
    }
}

?>
