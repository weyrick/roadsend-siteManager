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
* Author(s): pym
*
*/

/**
 * module to view source of a php script
 * also good example of how to capture output with PHP's output buffering support
 * and output it in a module
 */
class SM_viewSource extends SM_module {
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {
        // Directives
        $this->addDirective('fileName',"/var/www/html/index.php");
        $this->addDirective('centerInTable',false);
        $this->addDirective('tableBgColor','#FFFFAA');
        $this->addDirective('tableBorder','4');
        
        
    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
         $fileName= $this->directive['fileName'];
         ob_start();
         show_source($fileName);
         $contents= ob_get_contents();
         ob_end_clean();
         $this->say($contents);
    }
    
}


?>
