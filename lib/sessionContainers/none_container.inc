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
 * a class for faking the saving and loading of persistent variables
 * "loading" comes from variables passed to the script
 * there is no saving (it's handled by the session class with hLink and formID) 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_session 
 */
class SM_sessionContainer_none extends SM_sessionContainer {

  // "load" perisistents from GET/POST values
    
    function loadSession() {
        
        // get list of persistents
        $pVarList = $this->sessionH->getPersistentList();

        // loop through list
        foreach ($pVarList as $pVar) {

            // don't add sessionIDname
            if ($pVar == $this->sessionH->directive['sessionIDName'])
                continue;

            // update inVar manager
            $this->inVarH->setSessionVar($pVar, $_REQUEST[$pVar]);

        }

        return true;
    }

}



?>
