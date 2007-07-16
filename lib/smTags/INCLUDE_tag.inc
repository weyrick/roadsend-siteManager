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
 * class representation of an INCLUDE tag 
 * for including a template (or HTML) inside of a template
 */
class SM_smTag_INCLUDE extends SM_smTag {

    /**
    * template NAME 
    * @var string
    */
    var $templateName = '';

    /** 
    * reference to the instantiated template 
    * @var SM_smTag_INCLUDE
    */
    var $tptPointer = NULL;

    /**
    * main think method. 
    * this will run the template
    */
     
    function tagThink() {
        if (is_object($this->tptPointer))        
            $this->say($this->tptPointer->run());
        else {
            $this->debugLog("template tag has null tptPointer [".$this->templateName."]");
        }
    }

    /**
     * called by templates to reset the tag to its initial state
     */ 
    function tagReset () {
        if (is_object($this->tptPointer))        
            $this->say($this->tptPointer->templateReset());
    }

    /**
     * tag config. handles parsing of name and directives, as well
     * as the loading and instantiation of the template
     */
    function tagConfig() {

        global $SM_siteManager;

        // template tag
        // requires NAME tag
        if ($this->attributes['NAME']) {
            $this->templateName = $this->attributes['NAME'];
        }
        else {
            // warning -- type not found, ignore
            $this->debugLog("warning: SM tag type INCLUDE had no NAME parameter");
            return;
        }

        // now load that template and stick it in the new area
        $tpt = $SM_siteManager->loadTemplate($this->templateName);
        if (is_object($tpt)) {

            // yes! convert it into a template and add it in
            $this->tptPointer = $tpt;

        }
        else {                           
            // if it wasn't an object (ie, the template failed to load)
            // it will be ignored by the template
            $this->debugLog("warning: SM tag type INCLUDE, file name ($templateName) failed to load, ignoring");
        }

    }

}
  
?>
