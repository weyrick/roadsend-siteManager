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
 * class representation of a VAR tag 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smConfig
 */
class SM_varTag {

    /** 
    * attributes for this tag. key/value pairs 
    * @var array 
    */
    var $attributes = array();

    /**
     * constructor: create new VAR tag
     * @param array $attr array of attributes 
     */
    function SM_varTag($attr) {
        $this->attributes = $attr;
    }

    /**
     * serialize to PHP includeable data
     */
    function serialize() {
        $op = '';
        foreach ($this->attributes as $attrKey => $attrVal) {
            $op .= '$attributes[\''.$attrKey.'\'] = <<< SMCACHEEND'."\n";
            $op .= $attrVal."\nSMCACHEEND;\n";
        }
        $op .= '$newVarTag = new SM_varTag($attributes);'."\n";
        return $op;
    }

    /**
     * return a list of attributes this VAR maintains
     * @return array a list of attributes
     */
    function getAttrList() {
        return array_keys($this->attributes);
    }

    /**
     * determine if the attribute exists in this var tag
     * @return bool true if the attribute exists
     */
    function attrExists($attrKey) {
        return isset($this->attributes[$attrKey]);
    }

    /**
     * return an attribute from this VAR tag
     * @param string $attrKey the attribute key to lookup and return
     * @return mixed the value of the attribute, or null if not found
     */
    function getAttr($attrKey) {
        if (isset($this->attributes[$attrKey])) {
            return $this->attributes[$attrKey];
        }
        else {
            SM_debugLog("requested attribute [$attrKey] for VAR tag [{$this->attributes['NAME']}] wasn't found",$this,1);
            return NULL;
        }
    }

}

/**
 * class representation of a SECTION tag
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smConfig 
 */
class SM_sectionTag {

    /** 
     * our SECTION NAME
     * @var string
     */
    var $sectionName = '';

    /**
     * variable representation of a VAR tag [SECTIONID][VARNAME] = SM_varTag object
     * @var array
     */
    var $varList = array();

    /** 
    * a hash for caching getVarAttr() requests 
    * @var array
    */
    var $reqCache = array();

    /**
     * new section tag
     * @param string $sectionName our new SECTION name
     */
    function SM_sectionTag($sectionName) {
        $this->sectionName = $sectionName;
    }
    
    function __sleep() {
        // don't serialize reqCache
        return array('sectionName','varList');
    }

     /**
     * serialize to PHP includeable data
     */
     
    function serialize() {

        $op = '$newSectionTag = new SM_sectionTag(\''.$this->sectionName.'\');'."\n";
        foreach ($this->varList as $sectionID => $vData) {
            foreach ($vData as $varName => $varTag) {
                if (is_array($varTag)) {
                    foreach ($varTag as $realTag) {
                        $op .= $realTag->serialize();
                        $op .= '$newSectionTag->varList[\''.$sectionID.'\'][\''.$varName.'\'][] = $newVarTag;'."\n";
                    }
                }
                else {
                    $op .= $varTag->serialize();
                    $op .= '$newSectionTag->varList[\''.$sectionID.'\'][\''.$varName.'\'] = $newVarTag;'."\n";
                }
            }
        }
        return $op;
    }

    /**
     * add a variable to a section ID
     * @param string $sectionID sectionID to add this variable to
     * @param string $varKey KEY of the variable
     * @param mixed $varVal all attributes of the variable
     * @param bool $replace if true, overlapping values or overwritten, otherwise an array of values is created
     */
    function addVar($sectionID, $varKey, $varVal, $replace=false) {

        // create new VAR tag
        $newVar = new SM_varTag($varVal);

        // if it's already there, make it or add it to an array of values
        if (isset($this->varList[$sectionID][$varKey]) && (!$replace)) {
            if (is_array($this->varList[$sectionID][$varKey]))
                // add to array already there
                $this->varList[$sectionID][$varKey][] = $newVar;
            else
                // create array
                $this->varList[$sectionID][$varKey] = array($this->varList[$sectionID][$varKey],$newVar);
        }
        else {
            // value wasn't there before, so it's a single value
            $this->varList[$sectionID][$varKey] = $newVar;
        }

        // if it's in the cache, update the cache                
        /*
        // FIXME: slower?        
        if (isset($this->reqCache[$sectionID][$varKey])) {
            foreach ($varVal as $attr => $aVal)  {
                if (isset($this->reqCache[$sectionID][$varKey][$attr])) {
                    if (is_array($this->reqCache[$sectionID][$varKey][$attr])) {
                        $this->reqCache[$sectionID][$varKey][$attr][] = $aVal;
                    }
                    else {
                        $this->reqCache[$sectionID][$varKey][$attr] = array($this->reqCache[$sectionID][$varKey][$attr], $aVal);
                    }
                }
            }
        } 
        */  
        // FIXME: faster?
        if (isset($this->reqCache[$sectionID][$varKey]))
            unset($this->reqCache[$sectionID][$varKey]);

    }

    /**
     * determine if var exists
     * @param string $sectionID sectionID to use
     * @param string $varKey variable key, matches NAME attribute from VAR tag
     * @param string $varAttr the attribute to retrieve. defaults to VALUE attribute
     * @return bool true if variable exists
     */
    function varExists($sectionID, $varKey, $varAttr='VALUE') {

        if ($sectionID == '') {
            // search all section ID's
            $found = false;
            foreach ($this->varList as $sectionID => $varTA) {
                foreach ($varTA as $varKey => $varTag) {
                    if (is_array($varTag)) {
                        foreach ($varTag as $vT) {
                            $found = $vT->attrExists($varAttr);
                            if ($found)
                                break;
                        }
                    }
                    else {
                        $found = $varTag->attrExists($varAttr);
                        if ($found)
                            break;
                    }
                }
                if ($found)
                    break;
            }
            // return result
            return $found;
        }
        else {
            // section ID passed

            // check sectionID/varKey
            if (!isset($this->varList[$sectionID][$varKey])) {
                return false;
            }

            return $this->varList[$sectionID][$varKey]->attrExists($varAttr);
        }

    }

    /**
     * get an attribute from a VAR. if sectionID is left blank, and there is
     * more than one ID, and the requested variable shows up in more than one ID,
     * all values will be returned as an array
     * @param string $sectionID sectionID to use
     * @param string $varKey variable key, matches NAME attribute from VAR tag
     * @param string $varAttr the attribute to retrieve. defaults to VALUE attribute
     * @return mixed requested variable value, or NULL if not found
     */
    function getVarAttr($sectionID, $varKey, $varAttr='VALUE') {

        // uppercase varAttr
        $varAttr = strtoupper($varAttr);

        // key for the cache
        ($sectionID == '') ? $hKeySection = 'default' : $hKeySection = $sectionID;        

        // if we've already looked this up, return cached version
        if (isset($this->reqCache[$hKeySection][$varKey][$varAttr])) {            
            return $this->reqCache[$hKeySection][$varKey][$varAttr];
        }

        // mutliple sections 
        if (($sectionID == '')&&(isset($this->varList))&&(sizeof($this->varList)>1)) {
            
            // combine all sectionID's (pretend no sectionID's)
            $allVars = array();
            foreach ($this->varList as $secID=>$secVal) {
                foreach ($secVal as $vK => $vTag) {
                    if (is_array($vTag)) {
                        foreach ($vTag as $vTagObj) {
                            $allVars[$vK][] = $vTagObj->getAttr($varAttr);
                        }
                    }
                    else {
                        $allVars[$vK][] = $vTag->getAttr($varAttr);
                    }
                }
            }
            // cache it
            if (isset($allVars[$varKey])) {
                $this->reqCache[$hKeySection][$varKey][$varAttr] = $allVars[$varKey];
                return $allVars[$varKey];
            }
            else
                return NULL;
        }
        else {     

            // default to 'default' SECTION
            if ($sectionID == '')
                $sectionID = 'default';

            if (isset($this->varList[$sectionID][$varKey])) {            
                if (is_array($this->varList[$sectionID][$varKey])) {
                    // return all VALUE attributes for this $varKey as array
                    // unless all values are NULL, in which case return a single NULL
                    $vList = array();
                    $nonNull = false;
                    foreach ($this->varList[$sectionID][$varKey] as $varTag) {
                        $val = $varTag->getAttr($varAttr);
                        if ($val !== NULL)
                            $nonNull = true;
                        $vList[] = $val;
                    }
                                                            
                    if ($nonNull) {
                        // cache it
                        $this->reqCache[$hKeySection][$varKey][$varAttr] = $vList;
                        return $vList;
                    }
                    else {
                        return NULL;
                    }
                }
                else {
                    // single value
                    // cache it
                    $this->reqCache[$hKeySection][$varKey][$varAttr] = $this->varList[$sectionID][$varKey]->getAttr($varAttr);
                    return $this->reqCache[$hKeySection][$varKey][$varAttr];
                }
            }
            else
                // not found
                return NULL;
        }
    }

    /**
     * get VALUE attribute from a section VAR. if sectionID is left blank, and there is
     * more than one ID, and the requested variable shows up in more than one ID,
     * all values will be returned as an array
     * NOTE: this is a wrapper function for backward compatibility. see getVarAttr()
     * @param string $sectionID sectionID to retrieve this variable from
     * @param string $varKey key used to retrieve the value requested. matches NAME attribute
     * @return mixed the value of the requested variable, or NULL if not found
     */
    function getVar($sectionID, $varKey) {
        return $this->getVarAttr($sectionID, $varKey, 'VALUE');
    }

    /** 
     * return an entire section. if sectionID is left blank, and there are multiple ID's
     * for this section, all ID's will be combined into a singular SECTION
     * @param string $sectionID the ID of the section to retrieve
     * @param string $attr the attribute to retrieve from VARs in this SECTION. default: VALUE
     * @return array hash of all VAR NAME=>VALUE tags in the requested SECTION
     */
    function getSection($sectionID,$attr='VALUE') {
        
        // uppercase varAttr
        $attr = strtoupper($attr);
        
        if (($sectionID == '')&&(sizeof($this->varList)>1)) {
            // combine all sectionID's (pretend no sectionID's)
            $allID = array();

            // foreach sectionID...
            foreach ($this->varList as $secID=>$secVal) {

                // get the vars in this sectionID
                $varList = array_keys($secVal);

                // for each var in this section..
                foreach ($varList as $varKey) {

                    if (isset($allID[$varKey])) {
                        // add values
                        $allID[$varKey] = array_merge((array)$allID[$varKey],(array)$this->getVarAttr($secID, $varKey, $attr));
                    }
                    else {
                        // set value
                        $allID[$varKey] = $this->getVarAttr($secID, $varKey, $attr);
                    }
                }
            }
            return $allID;
        }
        else {

            // either specified sectionID or there was only one anyway

            // default sectionID
            if ($sectionID == '')
                $sectionID = 'default';

            // if it's there, gather all VALUE attributes from all VAR tags in this section
            if (isset($this->varList[$sectionID])) {
                foreach ($this->varList[$sectionID] as $varKey => $varTag) {
                    if (is_array($varTag)) {
                        foreach ($varTag as $varObj) {
                            $sectionArray[$varKey][] = $varObj->getAttr($attr);
                        }
                    }
                    else {
                        $sectionArray[$varKey] = $varTag->getAttr($attr);
                    }
                }
                return $sectionArray;
            }
            else {            
                // otherwise, not found.
                SM_debugLog("getSection ($this->sectionName): requested sectionID [$sectionID] wasn't found: ignoreing",$this,1);
                return NULL;
            }
        }
    }

    /**
     * determine whether this section has multiple ID's
     * @return int number of id's this section holds
     */
    function hasMultipleID() {
        return sizeof($this->varList);
    }

    /**
     * determine whether the requested sectionID exists
     * @return bool true if sectionID was defined
     */
    function sectionIDexists($sectionID) {
        return isset($this->varList[$sectionID]);
    }

    /**
     * return a list of id's used by this section
     * @return array list of id's
     */
    function getIDlist() {
        return array_keys($this->varList);
    }

    /**
     * dump info for debug purposes
     */
    function dumpInfo() {
        global $_dOp;
        $op = "SECTION: $this->sectionName<br />";
        $_dOp = '';
        foreach ($this->varList as $secID => $vars) {
            $op .= "  -- section ID: $secID<br />";
            foreach ($vars as $varKey => $varTag) {
                if (is_array($varTag)) {
                    foreach ($varTag as $varObj) {
                        $op .= " .... VAR: $varKey<br />\n";
                        $aList = $varObj->getAttrList();
                        foreach ($aList as $attr) {
                            if ($attr == 'NAME')
                                continue;
                            $v = $varObj->getAttr($attr);
                            if ($v === false)
                                $v = 'false';
                            if ($v === true)
                                $v = 'true';
                            if ($v === NULL)
                                $v = '[not set]';
                            
                            $op .= "........ $attr => ".$v."<br />\n";
                        }
                    }
                }
                else {
                    $op .= " .... VAR: $varKey<br />\n";
                    $aList = $varTag->getAttrList();
                    foreach ($aList as $attr) {
                        if ($attr == 'NAME')
                            continue;
                        $v = $varTag->getAttr($attr);
                        if ($v === false)
                            $v = 'false';
                        if ($v === true)
                            $v = 'true';
                        if ($v === NULL)
                            $v = '[not set]';
                        $op .= "........ $attr => ".$v."<br />\n";
                    }
                }
            }
        }
        return $op;
    }

}

/**
 * SM_config is a class that reads in a siteManager XML configuration file
 * the settings in that file may then be accessed through the getVar() method
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smConfig 
 */
class SM_config extends SM_object {
    
    /** 
    * hash of SECTIONS [SECTIONAME] = SM_sectionTag 
    * @var array
    */
    var $sectionList = array();

    /** 
    * our SMCONFIG config reader 
    * @var object:SM_configReader_SMCONFIG
    */
    var $cfgReader = NULL;

    /** 
    * a list of files loaded into this config. used only for caching 
    * @var string
    */
    var $fileLoadList = NULL;

    /**
     * constructor accepts config file to read, parses it.
     * @param string $file the config file to read
     * @param bool $tryCache wether to try and cache this file or not
     */
    function SM_config($file, $tryCache=true) {

        // configure
        $this->_smoConfigure();

        // load the config file
        $this->loadSite($file, $tryCache);
        
    }


    /**
     * load additional configuration file, but replace values from the original, don't
     * append
     * @param string $file the local configuration file to read
     * @param bool $tryCache if true, we will attempt to cache, if it's turned on
     */
    function loadSiteReplace($file, $tryCache=true) {
        $this->loadSite($file, $tryCache, true);
    }

    /**
     * load SITEID specific variables
     * @param string $file the local configuration file to read
     * @param bool $tryCache if true, we will attempt to cache, if it's turned on
     * @param bool $replace if true, values encountered in this file will overwrite values already in
     *                        the current configuration. if false, overlapping values will be appended to (as arrays)
     */
    function loadSite($file, $tryCache=true, $replace=false) {

        // if we dont have a reader yet, make it
        if ($this->cfgReader == NULL) {
            global $SM_rootDir;
            $this->cfgReader = SM_loadConfigReader('SMCONFIG', $this, $SM_rootDir.SM_SMCONFIG_READER_FILE);
        }

        // set replace
        $this->cfgReader->addDirective('replace',$replace);
        $tryCache = $tryCache && $this->cfgReader->useCache(); 

        if ($tryCache) {

            // XXX this may not return the correct results if you have two different config objects that load the 
            //     same base file, and then the same consecutive file where one objects uses replace and one doesn't
            //     we never do that so i'm ignoreing that case
            $this->fileLoadList .= $file;
            if (xcache_isset($this->fileLoadList)) {
                $this->debugLog("loading cached xsm file: {$this->fileLoadList}");
                $this->sectionList = unserialize(xcache_get($this->fileLoadList));
                if ($this->sectionList)
                    return;
                // if we get here, we couldn't unserialize, so fall through
            }

        }

        // fall through. either caching is off, cache didn't exist, or cache was stale
        // read in config, setting my properties at the same time
        $this->cfgReader->readConfigXML($file);
        
        // should we cache the config file?
        if ($tryCache) {
            xcache_set($this->fileLoadList, serialize($this->sectionList), $this->cfgReader->directive['cacheTTL']);
        }

    }
    

    /**
     * find out if a section (and optional sectionID) is defined
     * @param string $sectionName the SECTION name to check for
     * @param string $sectionID the SECTION ID to check for
     * @return bool true if the section (and optionally sectionID) is defined, false otherwise
     */
    function sectionExists($sectionName, $sectionID='') {
        // if section doesn't exist at all, return
        if (!isset($this->sectionList[$sectionName])) {
            return false;
        }
        // check for sectionID?
        if ($sectionID == '') {
            return true;
        }
        else {
            return $this->sectionList[$sectionName]->sectionIDexists($sectionID);
        }
    }

    /**
     * add or update a section
     * @param string $sectionName the SECTION name to add or update
     * @param string $sectionID the SECTION ID to update
     * @return bool true on success, false otherwise
     */
    function updateSection($sectionName,$sectionID) {
        if (!isset($this->sectionList[$sectionName])) {        
            $this->sectionList[$sectionName] = new SM_sectionTag($sectionName);        
            return true;
        }
        else {
            $this->debugLog("updateSection: sectionName [$sectionName] already existed, ignoring.",6);
            return false;
        }
    }
    
    /**
     * add a variable to a section
     * @param string $sectionName SECTION to add this variable to
     * @param string $sectionID which ID this section belongs to
     * @param string $varKey the variables KEY (NAME)
     * @param mixed $varVal the variables VALUE (VAL)
     * @param bool $replace if true and value already exists in the config, it will be replaced with the new
     *                            value. otherwise it will be appened to (as an array of values)
     * @return bool true on success, false otherwise
     */
    function addVar($sectionName, $sectionID, $varKey, $varVal, $replace=false) {
        if (isset($this->sectionList[$sectionName])) {        
            $this->sectionList[$sectionName]->addVar($sectionID, $varKey, $varVal, $replace);
            return true;
        }
        else {
            $this->debugLog("addVar: sectionName [$sectionName] wasn't found, ignoring.",6);
            return false;
        }
    }

    /**
     * get a variable from a section. if sectionID is left blank, and there is
     * more than one ID, and the requested variable shows up in more than one ID,
     * all values will be returned as an array
     * @param string $sectionName the SECTION to retrieve the variable from
     * @param string $varKey the KEY pointing to the VALUE to be retrieved
     * @param string $sectionID which sectionID to retrieve this value from
     * @param string $attr attribute to retrieve from this VAR tag. 
     * @return mixed requested variable value, or NULL if not found
     */
    function getVar($sectionName, $varKey,  $sectionID='', $attr='VALUE') {
        if (isset($this->sectionList[$sectionName]))        
            return $this->sectionList[$sectionName]->getVarAttr($sectionID, $varKey, $attr);
        else {        
            $this->debugLog("getVar: sectionName [$sectionName] wasn't found, ignoring.",6);
            return NULL;
        }
    }
    /**
     * determine if the given variable exists in the configuration
     * @param string $sectionName the SECTION to retrieve the variable from
     * @param string $varKey the KEY pointing to the VALUE to be retrieved
     * @param string $sectionID which sectionID to retrieve this value from
     * @param string $attr attribute to retrieve from this VAR tag. default: VALUE
     * @return bool true if variable exists
     */
    function varExists($sectionName, $varKey, $sectionID='', $attr='VALUE') {
        if (isset($this->sectionList[$sectionName]))        
            return $this->sectionList[$sectionName]->varExists($sectionID, $varKey, $attr);
        else {        
            return false;
        }
    }

    /**
     * return an entire section. if sectionID is left blank, and there are multiple ID's
     * for this section, all ID's will be combined into a singular SECTION
     * @param string $sectionName the SECTION to retrieve
     * @param string $sectionID which sectionID to retrieve this value from
     * @param string $attr attribute to retrieve from VARs in this SECTION. 
     * @return array an array of all variabes in the requested SECTION, or NULL if section not found
     */
    function getSection($sectionName,$sectionID='', $attr='VALUE') {
        if (isset($this->sectionList[$sectionName])) {        
            return $this->sectionList[$sectionName]->getSection($sectionID, $attr);
        }
        else {
            $this->debugLog("getSection: sectionName [$sectionName] wasn't found, ignoring.",6);
            return NULL;
        }
    }

    /**
     * discover whether the specified section has multiple id's, or only default
     * @param string $sectionName the SECTION to examine
     * @return bool true of the specified SECTION has multiple ID's, false otherwise of if section not found
     */
    function hasMultipleID($sectionName) {
        if (isset($this->sectionList[$sectionName])) {
            return $this->sectionList[$sectionName]->hasMultipleID();
        }
        else {
            $this->debugLog("hasMultipleID: sectionName [$sectionName] wasn't found, ignoring.",6);
            return false;
        }
    }

    /**
     * return the list of id's used by this SECTION
     * @param string $sectionName the SECTION whose id's we should retrieve
     * @return array a list of id's used by this SECTION, or NULL if section not found
     */
    function getIDlist($sectionName) {
        if (isset($this->sectionList[$sectionName])) {        
            return $this->sectionList[$sectionName]->getIDlist();
        }
        else {
            $this->debugLog("getIDlist: sectionName [$sectionName] wasn't found, ignoring.",6);
            return NULL;
        }
    }
    
    /**
     * dump info for debug purposes
     */                                                                           
    function dumpInfo() {
        $op = '';
        foreach ($this->sectionList as $sectionName=>$sec) {
            $op .= $sec->dumpInfo();
        }
        return $op;
    }

}

?>
