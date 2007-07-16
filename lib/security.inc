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
 *
 * Author(s): Shannon Weyrick (weyrick@roadsend.com)
 *
 * @package roadsend_siteManager
 */
/**
 * check the referer of the script
 *
 * @param string $errorFunction the function to run if the referer doesn't match
 * @param string $customReferer partial or full URL to match against current HTTP_REFERER
 */
function SM_checkReferer($errorFunction, $customReferer="") {

    global $SM_siteManager, $SM_siteID;

    $referer = getenv("HTTP_REFERER");

    if ($referer == "")
        $referer = "None";

    if (empty($customReferer)) {
    
        // if refList wasn't defined, return
        $refList = $SM_siteManager->siteConfig->getVar('security','validReferers',$SM_siteID);
        if (!is_array($refList))
            return;
            
        while (list($key, $val) = each($refList)) {
            if (eregi($val,$referer)) {
                return;
            }
        }
        
        // NO MATCH
        $errorFunction();
        
    }
    else {
        if (!eregi($customReferer, $referer))
            $errorFunction();
    }

}

/**
 * make sure the passed variable was a POST variable, and not a GET variable
 * @param string $varName the variable to verify
 * @deprecated use the inVar manager instead. see {@link SM_inVarManager}
 * @return bool whether the variable was POST or not
 */
function SM_confirmPOST($varName) {
    return isset($_POST[$varName]);
}


?>
