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
 * the main interface to the Global Configuration System
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_config 
 */
class SM_siteConfig {

    /** 
    * GLOBAL site configuration, from SM_GLOBAL_CONFIG_FILE 
    * @var SM_config
    */
    var $globalConfig = NULL;

    /** 
    * LOCAL site configuration, possibly from mutliple files 
    * @var SM_config
    */
    var $localConfig = NULL;

    /**
    * constructor, will load SM_GLOBAL_CONFIG_FILE if not passed as global
    */    
    function SM_siteConfig($globalFile=NULL) {

        global $SM_rootDir;

        if ($globalFile == NULL) {
            $globalFile = $SM_rootDir.SM_GLOBAL_CONFIG_FILE;
        }

        // local global config file, no caching
        $this->globalConfig = new SM_config($globalFile, false);

    }

    /**
     * load a local site configuration into $localConfig object
     * @param string $file the SMCONFIG XSM file to load
     */
    function loadSite($file,$tryCache=true) {

        if (!isset($this->localConfig)) {
	    // if we can't find it, set SM_develState by hand
	    // since it might not be set appropriately yet
	    if (!file_exists($file)) {
		$GLOBALS['SM_develState'] = true;
	    }
            $this->localConfig = new SM_config($file);
        }
        else {
            $this->localConfig->loadSite($file,$tryCache);
        }

    }

    /**
     * dump config values for this SITE and GLOBAL     
     */
    function dumpInfo() {

        $_dOp = '';
        $_dOp .= get_class($this)." :: configuration information:<br />\n";

        $_dOp .= "<br />- GLOBAL VALUES -<br />\n";
        $_dOp .= $this->globalConfig->dumpInfo();
        $_dOp .= '<hr />';
        $_dOp .= "<br />- LOCAL SITE VALUES -<br />\n";
        $_dOp .= $this->localConfig->dumpInfo();

        return $_dOp;

    }

    /**
     * get a value from the GLOBAL config file (only)
     * @param string $section the section to use for lookup
     * @param string $name the "key" from the key/value pair you're looking for
     * @param string $sectionID the sectionID to examine
     * @param string $attr the attribute to retrieve from this VAR tag. default: VALUE     
     */
    function getGlobalVar($section, $name, $sectionID='default', $attr='VALUE') {
        return $this->globalConfig->getVar($section,$name,$sectionID,$attr);
    }

    /**
     * get a value from the LOCAL config file (only)
     * @param string $section the section to use for lookup
     * @param string $name the "key" from the key/value pair you're looking for
     * @param string $sectionID the sectionID to examine
     * @param string $attr the attribute to retrieve from this VAR tag. default: VALUE     
     */
    function getLocalVar($section, $name, $sectionID='default', $attr='VALUE') {
        return $this->localConfig->getVar($section,$name,$sectionID,$attr);
    }

    /**
     * get a variable from this config class
     * TODO optimize this (weyrick)
     * @param string $section the section to use for lookup
     * @param string $name the "key" from the key/value pair you're looking for
     * @param bool $mergeGlobal when true, GLOBAL and local siteID values will be merged,
     *                                          and the results returned as an array. otherwise, local
     *                                          siteID would be used by default, only going to GLOBAL if
     *                                          a local value wasn't set
     * @param string $sectionID the sectionID to examine
     * @param string $attr the attribute to retrieve from this VAR tag. default: VALUE
     * @return mixed the value for the variable requested
     */
    function getVar($section, $name, $mergeGlobal=false, $sectionID='', $attr='VALUE') {
        
        // get global value first
        $gVar = $this->globalConfig->getVar($section,$name,$sectionID,$attr);

        // if for some reason a local site hasn't been loaded, just return global
        if (!isset($this->localConfig)) {
            return $gVar;
        }

        // get local value
        $lVar = $this->localConfig->getVar($section,$name,$sectionID,$attr);

        if ($mergeGlobal && isset($lVar)) {

            // we should mergeglobal, AND there's already a value in
            // siteID -- so merge
            
            // local must come before global in the array

            // four possibilities: 
            // 1) global is single, local is single
            // 2) global is an array, local is array
            // 3) global is an array, local is single
            // 4) global is single, local is an array


            // Get the values once, insted of in each if block 
            $gVarIsArray = is_array($gVar);
            $lVarIsArray = is_array($lVar);
            
            // 1
            if (!$gVarIsArray && !is_array($lVar)) {

                // both single, so merge to array
                $endVar = array($lVar, $gVar);

            }
            // 2
            elseif ($gVarIsArray && $lVarIsArray) {

                // both arrays, so merge arrays
                $endVar = array_merge($lVar, $gVar);

            }
            // 3
            elseif ($gVarIsArray && !$lVarIsArray) {

                // GLOBAL is array, local is not
                $endVar = $gVar;
                $endVar[] = $lVar;
                $endVar = array_reverse($endVar);   // reverse so local overrides global

            }
            // 4
            elseif (!$gVarIsArray && $lVarIsArray) {

                // GLOBAL is single, local is array
                $lVar[] = $gVar;
                $endVar = $lVar;

            }

            // whew! 
            return $endVar;

        }
        elseif (!isset($lVar)) {

            // they want mergeglobal, but local wasn't set, just return global
            //    OR
            // they only want local, but it's not set, return global instead
            return $gVar;

        }
        elseif (!$mergeGlobal && isset($lVar)) {

            // they only want local, and it's set so return it
            return $lVar;

        }
        else {

            // shouldn't get here!
            $this->fatalErrorPage("getVar(): shouldn't reach this!");

        }
        
    }

    /**
     * return an entire section at a time
     * it will combine GLOBAL with the current SITEID, with SITEID takeing precedence over GLOBAL
     * @param string $section the section to return
     * @param string $sectionID the section ID to examine
     * @param string $attr the attribute to retrieve from VARs in this section. default; VALUE
     * @return array a hash of all values in the section requested
     */
    function getSection($section, $sectionID='',$attr='VALUE') {

        $gSection = $this->globalConfig->getSection($section, $sectionID, $attr);
        if (!is_array($gSection))
            $gSection = array();

        if (isset($this->localConfig)) {
            $sSection = $this->localConfig->getSection($section, $sectionID, $attr);
            $section = array_merge((array)$gSection, (array)$sSection);
        }
        else {
            $section = $gSection;
        }

        return $section;

    }

    /**
     * return an entire section at a time, from LOCAL config only
     * it will combine GLOBAL with the current SITEID, with SITEID takeing precedence over GLOBAL
     * @param string $section the section to return
     * @param string $sectionID the section ID to examine
     * @param string $attr the attribute to retrieve from VARs in this section. default; VALUE
     * @return array a hash of all values in the section requested
     */
    function getLocalSection($section, $sectionID='',$attr='VALUE') {

        if (isset($this->localConfig)) {
            return $this->localConfig->getSection($section, $sectionID, $attr);
        }
        else {
            return NULL;
        }

    }

    /**
     * return an entire section at a time, from LOCAL config only
     * it will combine GLOBAL with the current SITEID, with SITEID takeing precedence over GLOBAL
     * @param string $section the section to return
     * @param string $sectionID the section ID to examine
     * @param string $attr the attribute to retrieve from VARs in this section. default; VALUE
     * @return array a hash of all values in the section requested
     */
    function getGlobalSection($section, $sectionID='',$attr='VALUE') {
        return $this->globalConfig->getSection($section, $sectionID, $attr);
    }

    /**
     * discover whether a particular section has mutltiple ID's
     * @param string $section the SECTION to examine
     * @return bool true if this SECTION has mutliple ID's in either GLOBAL or LOCAL
     */
    function hasMultipleID($section) {            
        if ($this->globalConfig->hasMultipleID($section) || 
            $this->localConfig->hasMultipleID($section))
            return true;
        else
            return false;
    }

    /**
     * retrieve the list of ID's a SECTION contains
     * @param string $section the SECTION to examine
     * @return array a list of id's
     */
    function getIDlist($section) {
        $lList = $this->localConfig->getIDlist($section);
        $gList = $this->globalConfig->getIDlist($section);
        if (($lList === NULL)&&($gList === NULL)) {
            return NULL;
        }
        if ($lList === NULL) {
            return $gList;
        }
        if ($gList === NULL) {
            return $lList;
        }
        // fall through - both not null
        $idList = array_merge($gList, $lList);
        return array_unique($idList);
    }

    /**
     * set a variable, based on section, name and siteID     
     * @param string $section the section to use for lookup
     * @param string $name the "key" from the key/value pair you want to write
     * @param mixed $newVal the new value of the key specified
     * @param string $siteID the siteID to use. current SITEID by default (deprecated)
     * @param string $sectionID the section ID to update
     * @param bool $replace if the value already exists, should it be replaced or appended to (as array)?
     * @param string $attr the attribute to update, defaults to VALUE
     */    
    function setVar($section, $name, $newVal, $siteID='SITEID', $sectionID='default', $replace=false, $attr='VALUE') {
        
        // must have local site loaded first
        if (!isset($this->localConfig))
            return;

        // add it in (replace)
        $this->localConfig->addVar($section, $sectionID, $name, array($attr => $newVal), $replace);
        
    }

    /**
     * a wrapper method for replacing a simple variable
     * @param string $section the section to use for lookup
     * @param string $name the "key" from the key/value pair you want to write
     * @param mixed $newVal the new value of the key specified
     * @param string $sectionID the section ID to update
     * @param string $attr the attribute to update, defaults to VALUE
     */
    function replaceVar($section, $name, $newVal, $sectionID='default', $attr='VALUE') {

        // must have local site loaded first
        if (!isset($this->localConfig))
            return;

        // add it in (replace)
        $this->localConfig->addVar($section, $sectionID, $name, array($attr => $newVal), true);

    }

}

?>
