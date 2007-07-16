<?php

/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001,2002 Roadsend, Inc.(http://www.roadsend.com)
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
 * class representation of a SESSIONID tag 
 */
class SM_smTag_SESSIONID extends SM_smTag {


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
        
      if (isset($this->sessionH)) {
          $this->say($this->sessionH->directive['sessionIDName'].'='.$this->sessionH->getSessionID());
        }

    }

}

?>
