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
* class representation of a HTMLTITLE tag 
*/
 
class SM_smTag_HTMLTITLE extends SM_smTag {

    /**
    * tag config
    */
     
    function tagConfig() {

       // DEFAULT and NAME are synonymous
       if (isset($this->attributes['DEFAULT'])) {
	  $this->attributes['NAME'] = $this->attributes['DEFAULT'];
       }
       
       if (isset($this->attributes['NAME']) && empty($this->directive['htmlTitle'])) {
	  $this->directive['htmlTitle'] = $this->attributes['NAME'];
       }


    }

   
    /** 
    * called when template is reset
    */
     
    function tagReset() {
        unset($this->directive['htmlTitle']);
    }

    /**
    * main think method. 
    */
     
    function tagThink() {
        
        $this->say($this->directive['htmlTitle']);        

    }

}

?>
