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


/** (definition) current version. must match .XML files it reads */
define ("SMCONFIG_VERSION", 1.2);

/** (definition) match a config variable */
define ("SM_CONFIGVAR_REGEXP", '/{\$((_*\w+\.?)\[*\'*(\w*)\'*\]*)}/U');

/**
 * class representation of an SMCONFIG configuration
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smConfigReader 
 */
class SM_configReader_SMCONFIG extends SM_configReader {

    /**
    * current section in config file (used only for parsing) 
    * @var string
    */
    var $_section = '';

    /** 
    * current section ID in config file (used only for parsing) 
    * @var string
    */
    var $_sectionID = '';

    /**
     * constructor
     */     
    function SM_configReader_SMCONFIG() {

        // call parent constructor
        parent::SM_configReader();

        // replace or append?
        $this->addDirective('replace',false);

    }

    /**
     * function triggered when an XML start element appears
     * here we will parse tag's we recognize from the config file
     * NOTE: this function is private and for use by the XML parser only
     * @param resource $parser the XML parser
     * @param string $name the tag found
     * @param array $attrs the attributes this tag carried
     */
    function startElement($parser, $name, $attrs) {
        
        global $SM_siteID;

        $this->_XMLstate['in'.$name] = true;

        switch ($name) {
        case 'SMCONFIG':

            // check version
            if ($attrs['VERSION'] > SMCONFIG_VERSION)
                $this->fatalErrorPage("version incompatibility with config file $this->file (I'm version ".SMCONFIG_VERSION.", it's version {$attrs['VERSION']})");

            break;
        case 'SITE':

            // site tag

            break;
        case 'SECTION':

            // sanity 
            if (!$this->_XMLstate['inSITE']) {
                $this->fatalErrorPage('malformed SMCONFIG file: SECTION tag not inside of SITE tag');
            }

            // we're in a specific section
            $this->_section = $attrs['NAME'];
            if (!empty($attrs['ID']))
                $sectionID = $attrs['ID'];
            else            
                $sectionID = 'default';

            // section ID to use
            $this->_sectionID = $sectionID;

            // update (ie, add if not there) this section
            $this->userObject->updateSection($attrs['NAME'], $sectionID);

            break;
        case 'VAR':

            // we're getting vars for a section
            if (!isset($this->_section)) {
                $this->fatalErrorPage("malformed SMCONFIG file: not in section or site for VAR tag");
            }

            // sanity check
            if (!isset($attrs['NAME'])) {
                $this->fatalErrorPage("malformed SMCONFIG file: required NAME attribute for VAR tag in section {$this->_section} is missing");
            }

            // sainty check
            if (!isset($attrs['VALUE'])) {
                // allow blank VALUE attributes
                $attrs['VALUE'] = '';
            }

            // do bools correctly
            foreach (array_keys($attrs) as $aKey) {
                if (strtoupper($attrs[$aKey]) == 'TRUE')
                    $attrs[$aKey] = true;
                elseif (strtoupper($attrs[$aKey]) == 'FALSE')
                    $attrs[$aKey] = false;
            }

            // if there's a variable in the value, do a replacement            
            if (preg_match_all(SM_CONFIGVAR_REGEXP,$attrs['VALUE'],$varMatch)) {
                foreach ($varMatch[1] as $key => $vm) {

                    // if there is a dot, we might use another section's var
                    if (strstr($vm,'.'))
                        list($vm, $tmpSection) = array_reverse(explode('.', $vm));                   
                    else
                        $tmpSection = $this->_section;
                   
                    // first, try to sub a value from this file,section
                    $sVal = $this->userObject->getVar($tmpSection,$vm);

                    if ($sVal == '') {

                        // if it's blank, try to use the global version

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

                    }

                    $attrs['VALUE'] = str_replace($varMatch[0][$key],$sVal,$attrs['VALUE']);
                }
            }

            // add the final variable in
            $this->userObject->addVar($this->_section,$this->_sectionID,$attrs['NAME'],$attrs,$this->directive['replace']);

            break;
        case 'SM_XML':
            // root tag
            break;
        default:
            $this->debugLog("unknown tag in SMCONFIG: [$name], ignoring");
            break;
        }

    }
    

    /** 
     * cache an SM_config file to PHP includable data
     * may be overridden by readers
     *
     */
    /*
       XXX deprecated 04/08 for xcache version (weyrick)
    
    function writeCache($cacheFile, $data) {

        // open the file
        $cFile = fopen($cacheFile, 'w');
        if ($cFile == false) {
            $this->fatalErrorPage("Unable to write to cache file $cacheFile");
        }

        // write data
        fputs($cFile, '<?php global $SM_READER_CACHECONTENTS; '."\n");
        
        foreach ($data as $sectionID => $sectionTag) {
            $op = $sectionTag->serialize();
            $op .= '$sectionList[\''.$sectionID.'\'] = $newSectionTag'.";\n";
            fputs($cFile, $op);
        }

        fputs($cFile, '$SM_READER_CACHECONTENTS = $sectionList;'."\n");

        fputs($cFile, ' ?>');

        // close file
        fclose($cFile);

    }
    */

}

?>
