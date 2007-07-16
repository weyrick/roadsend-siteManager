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
 * Author(s): (gollum@roadsend.com)
 *
 */

/**
 *
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class xmlFilter extends SM_entityFilter {
	
    // constructor 
    function filterConfig() {


    }


    // apply this filter 
    function filterThink() {

        $xml_parser = xml_parser_create();
        xml_parser_set_option($xml_parser, XML_OPTION_CASE_FOLDING, 1);

        if (!xml_parse($xml_parser, $this->data, true)) {
            $this->directive['badMessage'] = sprintf("XML error: %s at line %d",
                        xml_error_string(xml_get_error_code($xml_parser)),
                        xml_get_current_line_number($xml_parser));
            xml_parser_free($xml_parser);
            return false;
        }

        xml_parser_free($xml_parser);
        return true;

    }

}

?>
