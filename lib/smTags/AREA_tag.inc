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
* class representation of an AREA tag 
*/
 
class SM_smTag_AREA extends SM_smTag {

    /** 
    * the AREA NAME 
    * @var string
    */
    var $areaName = '';

    /** 
    * the list of objects contained in this area 
    * @var mixed
    */
    var $itemList = array();

    /**
     * a method for adding an item to this area's object list
     * @param mixed $data data to added to the area, such as module, template, or string
     */
    function addItem($data) {
        $this->itemList[] = $data;
    }

    /**
    * called by templates to reset the tag to its initial state
    */
      
    function tagReset () {
        
        unset($this->itemList);
        $this->itemList=array();

    }

    /**
    * configure the tag
    */
     
    function tagConfig() {

        // requires NAME tag
        $this->areaName = $this->attributes['NAME'];

    }

    /**
     * main tag think class
     * this will handle the data list, including running modules and templates
     * and retrieving their output
     */
    function tagThink() {

        // clear area output for this area before we run modules for this this area
        $areaOutput = '';

        if (sizeof($this->itemList) == 0) {
            $this->debugLog("tagThink: no items in area [$this->areaName], leaving blank");
            return;
        }

        // run through all objects in this area
        foreach ($this->itemList as $key => $areaData) {

    	    if (is_string($this->itemList[$key])) {

                    // string output
                    $areaOutput .= $this->itemList[$key];

            }
    	    // is this actually a module or a subtemplate/codeplate?
            elseif ($this->itemList[$key] instanceof SM_layoutTemplate) {

                // either template or codeplate

                // run template
                $areaOutput .= $this->itemList[$key]->run();

                // get javascript
                $this->sayJS($this->itemList[$key]->jsOutput);

            }
            elseif ($this->itemList[$key] instanceof SM_module) {

                // module

                // run module
                $areaOutput .= $this->itemList[$key]->run();

                // get javascript code from this module, if any
                $this->sayJS($this->itemList[$key]->getJS());

            }
            else {
                $this->debugLog("unknown object added to template area [$this->areaName] (was not decendent of SM_module or SM_layoutTemplate): ignoring");
                $this->debugLog("object appeared to be: ".get_class($this->itemList[$key]));
            }

        }                

        // wrap in span with ID or CLASS
        if (isset($this->attributes['CLASS'])) {
            $class = $this->attributes['CLASS'];
            $areaOutput = '<span class="'.$this->attributes['CLASS']."\">\n".
                          $areaOutput.
                          "\n</span>";
        }

        if (isset($this->attributes['ID'])) {
            $class = $this->attributes['ID'];
            $areaOutput = '<span id="'.$this->attributes['ID']."\">\n".
                          $areaOutput.
                          "\n</span>";
        }


        // add output to this areas output
        $this->say($areaOutput);

    }

}

?>
