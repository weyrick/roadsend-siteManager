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


// expression ripped from code by Michael A. Alderete <michael@aldosoft.com>
define('EMAIL_REGEXP', '/^[-!#$%&\'*+\\.\/0-9=?A-Z^_`{|}~]+@([-0-9A-Z]+\.)+([0-9A-Z]){2,4}$/i');

/**                     
 *  data must be a valid email address
 *
 *  directives:
 *  
 *      domainCheck   - when true, we will do a DNS query
 *                      on the domain name in the email
 *                      address to make sure it's valid
 *                      default (true)
 *   
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class emailFilter extends SM_entityFilter {

    // constructor 
    function filterConfig() {
            
        // do a dns query on the host? (windows doesn't support)
        (substr(PHP_OS, 0, 3) == "WIN") ?
            $this->directive['domainCheck'] = false :
            $this->directive['domainCheck'] = true;

    }


    //  apply this filter 
    function filterThink() {

        $emailAddress = $this->data;

        if (!strstr($emailAddress,'@'))
            return false;

        list($user, $host) = split("@", $emailAddress);
    
        if ((trim($user) == '')||(trim($host) == ''))
            return false;

        // check hostname
        if ($this->directive['domainCheck']) {
            if (!checkdnsrr($host, "ANY"))
                return false;
        }
        
        // all clear, final regexp
        return preg_match(EMAIL_REGEXP, trim($emailAddress));
                    
    }

}

?>