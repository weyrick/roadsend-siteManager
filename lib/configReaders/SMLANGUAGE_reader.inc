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

/** (definition) current language definition version. must match .XML files it reads */
define ("SMLANGUAGE_VERSION", 1);

/**
 * class representation of an SMLANGUAGE configuration
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smConfigReader 
 */
class SM_configReader_SMLANGUAGE extends SM_configReader {

    /**
    * function triggered when an XML start element appears
    * here we will parse tag's we recognize from the config file
    * NOTE: this function is private and for use by the smObject XML parser only
    * @param resource $parser the XML parser
    * @param string $name the tag found
    * @param array $attrs the attributes this tag carried
    */
    function startElement($parser, $name, $attrs) {
    
        // update state for this element
        $this->_XMLstate['in'.$name] = true;

        // examine tag
        switch ($name) {
        
        case 'SMLANGUAGE':
            // check version
            if ($attrs['VERSION'] > SMLANGUAGE_VERSION)
                $this->fatalErrorPage("version incompatibility with language file $this->file (I'm version ".SMLANGUAGE_VERSION.", it's version {$attrs['VERSION']})");
            break;

        case 'LDEF':
            $this->_XMLstate['cLocale'] = $attrs['LOCALE'];
            break;

        case 'TEXT':
            $this->_XMLstate['ctID'] = $attrs['ID'];
            break;

        case 'SM_XML':
            // root tag
            break;

        default:
            // ignore unknown tags
            break;
        }

    }
    
    // character data handler
     
    function cDataHandler($parser, $data) {

        if (trim($data) == '')
            return;        

        // ignore <![CDATA[ or ]]> XML tag
        if ($data == '<![CDATA[' || $data == ']]>')
            return;        
        
        // only handle data from language file
        if ((!( ($this->_XMLstate['inSM_XML'])&&
             ($this->_XMLstate['inSMLANGUAGE'])&&
             ($this->_XMLstate['inLDEF'])&&
             ($this->_XMLstate['inTEXT'])) ) ||
             (empty($this->_XMLstate['cLocale']))||(empty($this->_XMLstate['ctID']))) {
            return;
        }

        $cLocale = $this->_XMLstate['cLocale'];
        $ctID  = $this->_XMLstate['ctID'];

        // silly warning
        if (!empty($this->userObject->liText[$cLocale][$ctID])) {
            $this->userObject->liText[$cLocale][$ctID] .= $data;
        }
        else {
            $this->userObject->liText[$cLocale][$ctID] = $data;
            // alias for just language id
            if ( (strstr($cLocale,'_') !== false) && ($cLocale[2] == '_') ) {
                list($cLang, $cCtry) = split('_', $cLocale, 2);
                $this->userObject->liText[$cLang][$ctID] = $this->userObject->liText[$cLocale][$ctID];
            }
        }

    }


    // default handler for XML parser
    // this handles entities
     
    function defaultHandler($parser, $data) {
        $this->cDataHandler($parser, $data);
    }

}

?>
