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
* Author(s): Shannon Weyrick (weyrick@roadsend.com)
*
*/

if (!defined("SM_RAWHTML")) {
define("SM_RAWHTML",1);


/*

// PAGE OUTPUT definition
$pageOutput = <<< END

<br>
<b>To test, <a href="/home/test.phtml?$sessionVars">click here</a>

END;

*/

/**
 * A module to output strait HTML (or text)
 */
class SM_rawHTML extends SM_module {

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
                
        $this->say($this->directive['output']);
        
    }
    
}


} // end module

?>
