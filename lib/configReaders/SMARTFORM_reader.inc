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

/** (definition) current smartform version. must match .XML files it reads */
define ("SMSMARTFORM_VERSION", 2);

/** (definition) match a config variable */
define ("SM_SFVAR_REGEXP", '/{\$((_*\w+\.?)\[*\'*(\w*)\'*\]*)}/U');

/**
 * class representation of an SMOBJECT configuration
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smConfigReader 
 */
class SM_configReader_SMARTFORM extends SM_configReader {

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

        switch ($name) {
        case 'SMARTFORM':
            // check version
            if ($attrs['VERSION'] > SMSMARTFORM_VERSION)
                $this->fatalErrorPage("version incompatibility with SmartForm config file $this->file (I'm version ".SMSMARTFORM_VERSION.", it's version {$attrs['VERSION']})");
            
            break;
        
        case 'ENTITY':
            //We're in a specific entity 
            $this->_XMLstate['entity'] = $attrs['NAME'];
            break;
            
        // used for requied fields or fields used in the add function
        case 'PARAMETER';                           
            break;
        
        case 'DIRECTIVE';
            // the directive name, used for directive with array values
            if (isset($attrs['KEY']))
                $this->_XMLstate['directiveID'] = $attrs['KEY'];            
            else
                $this->_XMLstate['directiveID'] = '';
            break;
        
        case 'FILTER';
            
            // the filter name, used for directive with array values
            if (isset($attrs['KEY']))
                $this->_XMLstate['filterID'] = $attrs['KEY'];

            if (isset($attrs['MESSAGE']))
                $this->_XMLstate['filterMessage'] = $attrs['MESSAGE'];

            break; 
        
        case 'VAR':
            
            $name = $attrs['NAME'];
            
            // do bools substitutions on values unless they are in an option list values
            if ( ((isset($this->_XMLstate['inDIRECTIVE']))&&($this->_XMLstate['inDIRECTIVE'])) &&
                 ((isset($this->_XMLstate['directiveID']))&&($this->_XMLstate['directiveID'] == "optionList"))) {

                $val = $attrs['VALUE'];

            } else {

                if (strtoupper($attrs['VALUE']) == 'TRUE')
                    $val = true;
                elseif (strtoupper($attrs['VALUE']) == 'FALSE')
                    $val = false;
                else
                    $val = $attrs['VALUE'];

            }

            // if there's a variable in the value, do a replacement
            if (preg_match_all(SM_SFVAR_REGEXP,$val,$varMatch)) {

                foreach ($varMatch[1] as $key => $vm) {

                        // if $varMatch[0][0] contains a [, assume it's a hash
                        if (strstr($varMatch[0][$key],'[')) {
                            $gHash = $varMatch[2][$key];
                            $gKey = $varMatch[3][$key];
                            $sgHash = $GLOBALS[$gHash];
                            $sVal = $sgHash[$gKey];
                        }
                        else {
                            // regular global
                            $sVal = $GLOBALS[$vm];
                        }

                        $val = str_replace($varMatch[0][$key],$sVal,$val);

                }

            }


            // NON - ENTITY DIRECTIVES INFORMATION
            if (!isset($this->_XMLstate['inENTITY']) || !$this->_XMLstate['inENTITY']) {

                // Add directives to the form : this is done directly.
                if ($this->_XMLstate['inDIRECTIVE']) {
                    $name = $attrs['NAME'];
                    $this->userObject->addDirective($name, $val);
                }
            
            // ENTITY INFORMATION
            } else {
                
                $entity         = $this->_XMLstate['entity'];

                // Add required parameters to the entity
                if (($this->_XMLstate['inSMARTFORM']) && ($this->_XMLstate['inPARAMETER'])) {                    
                    $this->userObject->xEntity[$entity][$name] = $val;
                }
    
                // Add directives to the entity
                if (($this->_XMLstate['inSMARTFORM']) && 
                    ((isset($this->_XMLstate['inDIRECTIVE'])) && ($this->_XMLstate['inDIRECTIVE']))
                    ) {
                    $directiveID    = $this->_XMLstate['directiveID'];
                    $name = $attrs['NAME'];
                    $this->userObject->xEntity[$entity]['directive'][$directiveID][$name] = $val;
                }
    
                // Add filters to the entity
                if (($this->_XMLstate['inSMARTFORM']) && 
                    ((isset($this->_XMLstate['inFILTER'])) && ($this->_XMLstate['inFILTER']) )
                    ) {

                   
                    $name           = $attrs['NAME'];

                    if (isset($this->_XMLstate['filterID']))                    
                        $filterID       = $this->_XMLstate['filterID'];
                    else
                        $filterID       = '';

                    $this->userObject->xEntity[$entity]['filter'][$filterID][$name] = $val;
                    
                    if (isset($this->_XMLstate['filterMessage'])) {
                        $this->userObject->xEntity[$entity]['filter'][$filterID]['message'] = $this->_XMLstate['filterMessage'];
                    }
    
                }
            }
        break;
        case 'SM_XML':
            // root tag
            break;        
        default:
            // Removed error check on unknown tags, since
            // tags from other extened objects will be unknown at this point
            break;
        }

    }
    
}

?>
