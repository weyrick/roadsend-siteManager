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
 * Author(s): Jonathan Michel (pym@roadsend.com)
 *
**/

/**
 *
 *  This filter allows for a check on a remote URL (post)
 *  and returns true/false based on  the existance
 *  of a string match in the returned page
 *  Sample use is checking the availablity of a user name on a third party system.
 *    
 * @author Jonathan Michel <pym@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class remotePostFilter extends SM_entityFilter {


    /** setup filter */
    function filterConfig() {            
    
        $this->directive['postField']=NULL;
        $this->directive['postURL']=NULL;
        $this->directive['matchString']=NULL;
    }


    /** apply this filter */
    function filterThink() {
            $ch = curl_init();
            
            // set URL and other appropriate options
            curl_setopt($ch, CURLOPT_URL, $this->directive['postURL']);
            curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE);
            curl_setopt($ch, CURLOPT_POST, 1);
            curl_setopt($ch,CURLOPT_POSTFIELDS, $this->directive['postField']."=".$this->data);
            
            // grab URL and pass it to the browser
            $rVal = curl_exec($ch);
            
            // close cURL resource, and free up system resources
            curl_close($ch);
            
            if (strpos($rVal,$this->directive['matchString']) > 0) {
                return true;
            } else {
                return false;
            };
    }

}

?>