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
 * class representation of an EXAMPLE SM tag 
 */
class SM_smTag_EXAMPLE extends SM_smTag {

    var $props = NULL;

    /**
     * tag config
     */
    function tagConfig() {

        // always require name
        if (!isset($this->attributes['FOO'])) {
            $this->fatalErrorPage("EXAMPLE tag missing FOO attribute");
        }

    }

    /**
     * called when template is reset
     */
    function tagReset() {
        $this->props = NULL;
    }

    /**
     * main think method. 
     */
    function tagThink() {
        
        $this->props = $this->attributes['FOO'];
        $this->say('EXAMPLE tag: '.$this->props);

    }

}

?>
