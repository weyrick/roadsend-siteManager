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
 *
 *  data must not be empty  
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class caseRequiredFilter extends SM_entityFilter {
    
    /** setup filter */
    function filterConfig() {

        // the var name of the submit button for which 
        // this entity is required
        $this->directive['submitCase'] = NULL;
    }

    // apply this filter 
    function filterThink() {

        $submitVar = $this->directive['submitCase'];

        if ($_POST[$submitVar] == NULL) {
            return TRUE;
        }
    }

}

?>