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
 * class representation of a JAVASCRIPT tag 
 */
class SM_smTag_JAVASCRIPT extends SM_smTag {


    /**
     * tag config. this tag needs to be delayed,
     * since it will output all javascript gathered
     * after all modules and templates have been run
     */
    function tagConfig() {
        $this->delayThink = true;
    }

    /**
     * main think method. 
     * gather main session javascript code
     */
    function tagThink() {
        
        // check for session handlers javascript output, which contains the main javascript
        // ouput for this page
        if (!empty($this->sessionH) && !empty($this->sessionH->jsOutput)) {
            $this->say($this->sessionH->jsOutput);
        }

    }

}

?>
