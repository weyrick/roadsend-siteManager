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
* Author(s): Jon Michel pym@roadsend.com
*
*/
/**
 * A module to output strait HTML (or text)
 */
class SM_incHTML extends SM_module {


    function moduleConfig() {


        // turn this on to show logout notice if logged in
        $this->addDirective('url', false);

    }


    /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {

        $url = $this->directive['url'];
        if (!$url) {
            $this->say('Set URL to include');
        } else {
            //list($protocol, $uri) = split('//', $url);
            $html = file_get_contents($url);
        }
        $this->say($html);

        
    }
    
}


?>
