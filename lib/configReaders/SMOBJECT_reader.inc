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

/** (definition) current object version. must match .XML files it reads */
define ("SMOBJECT_VERSION",   2);

/**
 * class representation of an SMOBJECT configuration
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smConfigReader 
 */
class SM_configReader_SMOBJECT extends SM_configReader {

    /**
    * function triggered when an XML start element appears
    * here we will parse tag's we recognize from the config file
    * @param resource $parser the XML parser
    * @param string $name the tag found
    * @param array $attrs the attributes this tag carried
    */
    function startElement($parser, $name, $attrs) {
    
        // update state for this element
        $this->_XMLstate['in'.$name] = true;

        // examine tag
        switch ($name) {
        case 'SMOBJECT':
            // check version
            if ($attrs['VERSION'] > SMOBJECT_VERSION)
                $this->fatalErrorPage("version incompatibility with config file $this->file (I'm version ".SMOBJECT_VERSION.", it's version {$attrs['VERSION']})");
            break;
        
        case 'DIRECTIVE':
            // no functionality
            break;

        case 'VAR':
            // we're getting vars for a section
            $name = $attrs['NAME'];

            // do bools correctly
            if (strtoupper($attrs['VALUE']) == 'TRUE')
                $val = true;
            elseif (strtoupper($attrs['VALUE']) == 'FALSE')
                $val = false;
            else
                $val = $attrs['VALUE'];
    
            // if we're in a valid SMOBJECT and DIRECTIVE tag, 
            // add directive from the directive section
            if (($this->_XMLstate['inSMOBJECT']) && ($this->_XMLstate['inDIRECTIVE'])) {
                $this->userObject->addDirective($name,$val,$this->directive['owD']);
            }
            break;

        case 'SM_XML':
            // root tag
            break;
        default:
            // ignore unknown tags
            break;
        }

    }
    
}

?>
