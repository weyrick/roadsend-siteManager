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
 * class representation of a SESSIONVARS tag 
 */
class SM_smTag_SESSIONVARS_NA extends SM_smTag {


    /**
     * tag config. this tag needs to be delayed,
     * since it requires output sessions vars gathered
     * from the modules that have been run
     */
    function tagConfig() {
        $this->delayThink = true;
    }

    /**
     * main think method. 
     * output current (and final) sessionVars
     */
    function tagThink() {

	$sVars = $this->sessionH->getSessionVars();
	if (!empty($sVars))
	    $this->say('?'.$sVars);

    }

}

?>
