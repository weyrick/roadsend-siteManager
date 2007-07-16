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
 * class representation of a MODULE tag 
 */
class SM_smTag_MODULE extends SM_smTag {

    /**
    * module NAME 
    * @var string
    */
    var $moduleName = '';

    /** 
    * directives this module should recieve upon creation 
    * @var array
    */
    var $directive = array();

    /** 
    * reference to the instantiated module 
    * @var SM_smTag_MODULE
    */
    var $modPointer = NULL;

    /**
     * main think method. 
     * this will run the module
     */
    function tagThink() {
        if (is_object($this->modPointer))        
            $this->say($this->modPointer->run());
        else {
            $this->debugLog("module tag has null modPointer [".$this->moduleName."]");
        }
    }


    /**
     * tag config. handles parsing of name and directives, as well
     * as the loading and instantiation of the module
     */
    function tagConfig() {

        global $SM_siteManager;

        // module tag
        // requires NAME tag
        if ($this->attributes['NAME']) {
            $this->moduleName = $this->attributes['NAME'];
        }
        else {
            // warning -- type not found, ignore
            $this->debugLog("warning: SM tag type MODULE had no NAME parameter");
            return;
        }

        // optional directives tag
        if (!empty($this->attributes['DIRECTIVES'])) {
            $directives = $this->attributes['DIRECTIVES'];
        }
        else {
            $directives = '';
        }

        // optional file tag, for specifying a module file name
        if (!empty($this->attributes['FILE'])) {
            $fName = $this->attributes['FILE'];
        }
        else {
            $fName = '';
        }

        // now load that module and stick it in the new area
        $baMod = $SM_siteManager->loadModule($this->moduleName, $fName,FALSE);
        if (is_object($baMod)) {

            // yes! convert it into a module and add it in
            $this->modPointer = $baMod;

            // if there were directives, decode them and set them up
            if ($directives != '') {
                $dArray = split(',',$directives);
                foreach ($dArray as $dPair) {
                    list($dKey, $dVal) = split('=',$dPair);
                    $baMod->addDirective($dKey,$dVal);
                }
                $this->directive = $baMod->directive;
            }

        }
        else {                           
            // if it wasn't an object (ie, the module failed to load)
            // it will be ignored by the template
            $this->debugLog("warning: SM tag type MODULE, module name ($moduleName) failed to load, ignoring");
        }

    }

}
  
?>
