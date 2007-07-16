<?php

/*********************************************************************
*  Roadsend SiteManager
*  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
**********************************************************************
*
* This source file is subject to version 1.0 of the Roadsend Public
* License, that is bundled with this package in the file 
* LICENSE, and is available through the world wide web at 
* http://www.roadsend.com/license/rpl1.txt
*
**********************************************************************
* Author(s): weyrick
*
*/

/**
 * a skeleton module. extends base SM_module class.
 *
 */
class smVersion extends SM_module {
     
    /**
     * config
     */
    function moduleConfig() {
        $this->addDirective('outputInTable',false);
    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
    
        $this->say("SiteManager v".SM_VERSION);

    }
    
}


?>
