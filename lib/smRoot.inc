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

// defines
define('SM_UNKNOWN',0);             // undefined
define('SM_TEMPLATE',1);            // SM_layoutTemplate object
define('SM_CODEPLATE',2);           // SM_codePlate object

/**
 * SM_siteManagerRoot is the highest level class for implementing
 * a site using siteManager
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 */
class SM_siteManagerRoot extends SM_object {
    
    /** 
    * the Global Site Configuration object 
    * @var SM_siteConfig
    */
    var $siteConfig = NULL;

    /** 
    * the root layout template 
    * @var SM_layoutTemplate
    */
    var $rootTemplate = NULL;

    /** 
    * list of modules we have loaded 
    * @var SM_module
    */
    var $moduleList = array();

    /** 
    * list of loaded templates 
    * @var SM_layoutTemplates
    */
    var $templateCache = array();

    /**
     * list of prefixes to remove from error warnings and notices
     */
    var $filePrefixRemove = array();

    /**
    * constructor. load global configuration
    */
     
    function boot() {

        global $SM_rootDir, $SM_siteID, $HTTP_SERVER_VARS, $SM_MODULE_COUNT;

        // remove file prefix from debuglog
        $this->addFilePrefixRemove(substr(__FILE__, 0, strpos(__FILE__,'/siteManager')));

        // check SITEID for validity
        if (!preg_match('/^\w+$/',$SM_siteID))
            SM_fatalErrorPage("\$SM_siteID was not set or invalid [$SM_siteID]");

        // detect accepted browser locale
        if (isset($_SERVER['HTTP_ACCEPT_LANGUAGE'])) {
            global $SM_clientLocale;
            $SM_clientLocale = split(', *',$_SERVER['HTTP_ACCEPT_LANGUAGE']);
        }

        // startup inVar manager
        $this->inVarH = new SM_inVarManager();

        // CREATE GLOBAL CONFIGURATION OBJECT
        $this->siteConfig = new SM_siteConfig();
        $this->_updateGlobals();

        // add redist directories to include_path
        $curPath = ini_get('include_path');
        (substr(PHP_OS, 0, 3) == "WIN") ? $pSep = ';' : $pSep = ':';
        $newPath = $curPath.$pSep.$this->siteConfig->getVar('dirs','pear');
        ini_set('include_path', $newPath);

        // load initial/default error handler
        $errorHandlerClass = $this->siteConfig->getGlobalVar('debug','errorHandler');
        if (!empty($errorHandlerClass)) {
            $this->_loadErrorHandler($errorHandlerClass);
        }
        else {
            SM_fatalErrorPage("A default error handler was not defined in globalConfig.xsm", $this);
        }

        // Start with no modules loaded
        $SM_MODULE_COUNT = 0;

    }

    /**
     * add a new file prefix to remove from warnings and notices in debug log
     * @param string $p prefix to add
     */
    function addFilePrefixRemove($p) {
        $this->filePrefixRemove[] = $p;
    }
    
    /**
     * remove prefix from given file if matches our internal list
     * @return string the given file, possibly with prefix removed
     */
    function maybeRemovePrefix($file) {
        foreach ($this->filePrefixRemove as $f) {
            if (substr($file, 0, strlen($f)) == $f) {
                return substr($file, strlen($f)+1);
            }
        }
        return $file;
    }
    
    /**
     * load a new error handler
     * @param string $errorHandlerClass class name of new error handler. should extend SM_errorHandler
     */
    function _loadErrorHandler($errorHandlerClass) {
        if (!class_exists($errorHandlerClass)) {
            SM_fatalErrorPage("Unknown error handler: $errorHandlerClass (class not defined)",$this);
        }
        $this->errorHandler = new $errorHandlerClass();
    }

    /**
     * load local site configuration file
     * @param string $file the site file to load. must specify path
     */
    function loadSite($file,$tryCache=true) {

        $this->siteConfig->loadSite($file,$tryCache);

        // check for new error handler loaded in site
        $newErrorHandler = $this->siteConfig->getVar('debug','errorHandler');
        if (($newErrorHandler != NULL) && (strtoupper($newErrorHandler)) != strtoupper(get_class($this->errorHandler))) {
            $this->_loadErrorHandler($newErrorHandler);
        }

        $this->_updateGlobals(); 

    }

    /**
     * search the GCS module paths for the requested library file, and include it
     * @param string $libFile the file to look for, in lib dirs
     * @param bool $fatalFNF when true, if the file wasn't found, will exit with SM_fatalErrorPage
     * @return bool true on success, false on failure (or fatal error)
     */
    function includeLib($libFile, $fatalFNF=true) {

        // find the requested file in the GCS modules path
        $libFile = $this->findSMfile($libFile, 'libs', 'inc', $fatalFNF);
        if ($libFile === false) {
            return false;
        }

        // try to require the module they've specified
        include_once($libFile);

        return true;

    }


    /**
     * search the GCS module paths for the requested module file, and include it
     * so it's available for instantiation. does NOT create an instance of it.
     * use this for modules that extend other modules
     * @param string $moduleFile the file to look for (NOT the class name of the module, the file it's in!)
     * @param bool $fatalFNF when true, if the file wasn't found, will exit with SM_fatalErrorPage
     * @return bool true on success, false on failure (or fatal error)
     */
    function includeModule($moduleFile, $fatalFNF=true) {

        // find the requested file in the GCS modules path
        $moduleFile = $this->findSMfile($moduleFile, 'modules', 'mod',$fatalFNF);
        if ($moduleFile === false) {
            return false;
        }

        // try to require the module they've specified
        include_once($moduleFile);

        return true;
    
    }

    /**
     * load a module and return a reference to it
     * this function will check the module paths if a path was not specified
     * @param string $moduleFile module to load. may include a path, in which case the specific name
     *                             passed will be included for use. otherwise it will search the module
     *                             path for the module to load
     * @param string $moduleName after including the module, the function will try to create an object
     *                             of the same type as the file name given - if this variable is set, however,
     *                             it will use this as the object name instead.
     * @param bool $fatalFNF when true, if the file wasn't found, will exit with SM_fatalErrorPage
     * @return SM_module reference to new module, or null if not loaded
     */
    function loadModule($moduleFile, $moduleName='', $fatalFNF=true) {
        
        // did they specify a path?
        if ( (strstr($moduleFile, '/')) || (strstr($moduleFile, '\\')) )  {

            // yes, they specified a path
            // default object name
            if ($moduleName == '')
                $moduleName = basename($moduleFile);
    
        }
        else {
        
            // default object name
            if ($moduleName == '') {

                // but if they included extension, leave that off the object name
                if (substr($moduleFile, strlen($moduleFile)-4, 4) == '.mod') {
                    $moduleName = substr($moduleFile, 0, -4);
                }
                else {
                    $moduleName = $moduleFile;
                }

            }    

        }
        
        // include the module file
        if ($this->includeModule($moduleFile, $fatalFNF) === false) {
            $this->debugLog("Unable find module for inclusion: $moduleFile");
            return NULL;
        }

        // did we get the module we're looking for?
        if (class_exists($moduleName)) {
            $newModule = new $moduleName();
        }
        // check for SM_ module
        elseif (class_exists('SM_'.$moduleName)) {
            $SMmoduleName = 'SM_'.$moduleName;
            $newModule = new $SMmoduleName();
        }
        else {
            $this->fatalErrorPage("could not load requested module (check class name): $moduleFile, $moduleName");
        }
            
        // add to our list of modules
        $this->moduleList[] = $newModule;

        return $newModule;
    
    }

    /**
     * determine if the variable passed is currently in the list of variables
     * to propagate
     * deprecated, use inVarH->isInVarP for this
     * @param string $varName the variable to check
     * @return bool whether this variable will propagate or not, at time of check
     */
    function isInVarP($varName) {
        return in_array($varName, $this->inVarH->inVarPList);
    }

    /**
     * load and set the root template
     * @param string $fName the filename of the template/codeplate to load. will search template paths.
     *                        if type is left blank, you must include an extension so it knows which type
     *                        of template it's loading
     * @param const $type either SM_TEMPLATE or SM_CODEPLATE
     * @return SM_layoutTempalte layout template object
     */
    function rootTemplate($fName, $type=SM_UNKNOWN) {

        // if type is unknown use extension
        if ($type == SM_UNKNOWN) {

            $fExt = substr($fName, strlen($fName)-3, 3);
            switch ($fExt) {
            case 'cpt':
                $type = SM_CODEPLATE;
                break;
            case 'tpt':
                $type = SM_TEMPLATE;
                break;
            default:
                $type = SM_TEMPLATE;
                $this->debugLog("rootTemplate chose SM_TEMPLATE by default for $fName ($fExt)");
                break;
            }
        }

        // load either a template or a code plate for the root template
        if ($type == SM_TEMPLATE)
            $this->rootTemplate = $this->loadTemplate($fName);
        else
            $this->rootTemplate = $this->loadCodePlate($fName);

        // return new template
        return $this->rootTemplate;

    }


    /**
     * search the GCS module paths for the requested CodePlate file, and include it
     * so it's available for instantiation. does NOT create an instance of it.
     * use this for codePlates that extend other codePlates
     * @param string $cpFile the file to look for (NOT the class name of the codePlate, the file it's in!)
     * @param bool $fatalFNF when true, if the file wasn't found, will exit with SM_fatalErrorPage
     * @return bool true on success, false on failure (or fatal error)
     */
    function includeCodePlate($cpFile, $fatalFNF=false) {

        // make sure component is loaded
        SM_loadComponent('SM_codePlate');

        // find the requested file in the GCS modules path
        $cpFile = $this->findSMfile($cpFile, 'codePlates', 'cpt', $fatalFNF);
        if ($cpFile === false) {
            return false;
        }

        // try to require the codePlate they've specified
        include_once($cpFile);

        return true;

    }

    /**
     * load a codePlate and return a reference to it
     * this function will check the codeplate paths if a path was not specified
     * @param string $cpFile codePlate to load. may include a path, in which case the specific name
     *                             passed will be included for use. otherwise it will search the codePlate
     *                             path for the codePlate to load
     * @param string $cpName  after including the codePlate, the function will try to create an object
     *                             of the same type as the file name given - if this variable is set, however,
     *                             it will use this as the object name instead.
     * @param bool $fatalFNF when true, if the file wasn't found, will exit with SM_fatalErrorPage
     * @return SM_codePlate reference to new SM_codePlate, or null if not loaded
     */
    function loadCodePlate($cpFile, $cpName='',$fatalFNF=true) {
        
        // did they specify a path?
        if ( (strstr($cpFile, '/')) || (strstr($cpFile, '\\')) )  {
    
            // yes, they specified a path
            // default object name
            if ($cpName == '')
                $cpName = basename($cpFile);
    
        }
        else {
        
            // default object name
            if ($cpName == '') {

                // but if they included extension, leave that off the object name
                if (substr($cpFile, strlen($cpFile)-4, 4) == '.cpt') {
                    $cpName = substr($cpFile, 0, -4);
                }
                else {
                    $cpName = $cpFile;
                }

            }
    
        }

        // try to require the cp they've specified
        if ($this->includeCodePlate($cpFile, $fatalFNF) === false) {
            $this->debugLog("Unable to find codePlate for inclusion: $cpFile");
            return NULL;
        }
    
        // did we get the cp we're looking for?
        if (class_exists($cpName)) {
            $newCp = new $cpName();
        }
        else {
            $this->fatalErrorPage("could not load requested codePlate (check class name): $cpFile, $cpName");
        }

        return $newCp;
    
    }

    /**
     * search Global Configuratin System paths for certain SiteManager files. it can take a name
     * like "basicTemplate" and search the template paths for "basicTemplate.tpt", for example
     *
     * this function will try to load from a PCC library if available 
     *
     * @param string $fName the file to find
     * @param array $GCSpath either a "dir" SECTION from global config, or an array of them (ie 'libs')
     * @param string $extension an extension to automaticall try and add
     * @param bool $fatalFNF when true, if the file wasn't found, will exit with SM_fatalErrorPage
     * @return mixed  on success, will return the full path of the file found. on failure, will either fatalError
     *                  or return false, based on $fatalFNF parameter
     */
    function findSMfile($fName, $GCSpath, $extension='', $fatalFNF=true) {
        if (defined('ROADSEND_PCC') == 1) {
            return $this->_findSMfile_lib($fName, $GCSpath, $extension, $fatalFNF);
        }
        else {
            return $this->_findSMfile_disk($fName, $GCSpath, $extension, $fatalFNF);
        }
    }

    /**
     * attempt to load the requested file from a Roadsend PCC library http://www.roadsend.com
     *
     */
    function _findSMfile_lib($fName, $GCSpath, $extension='', $fatalFNF=true) {

        // debug log at verbose level 1
        //$this->debugLog("PCC: attempting to load SiteManager resource file from lib: $fName ($GCSpath, $extension)",1);

        // check for a common error of passing an array as filename
        if (is_array($fName)) {
            $this->fatalErrorPage("Invalid findSMfile call: fName was array: ".join(',',$fName));
        }

        // did they specify a path?
        if ( (!strstr($fName, '/')) && (!strstr($fName, '\\')) )  {

            // nope, they didn't specify a path. search paths from globalConfig (merge GLOBAL)
            $path = $this->siteConfig->getVar('dirs', $GCSpath, true);
            
            // check multiple paths
            if (is_array($path)) {
                $realPath = '';
                foreach ($path as $p) {
                    $check1 = $p.'/'.$fName;
                    $check2 = $p.'/'.$fName.'.'.$extension;
                    if (re_lib_include_exists($check1)) {
                        $realPath = $check1;
                        break;
                    }
                    elseif (re_lib_include_exists($check2)) {
                        $realPath = $check2;
                        break;
                    }
                }
            }
            else {
                if (re_lib_include_exists($path.'/'.$fName)) {
                    $realPath = $path.'/'.$fName;
                }
            }
            
        }

        if ($realPath != '') {
            return $realPath;
        }
        else
            return $this->_findSMfile_disk($fName, $GCSpath, $extension, $fatalFNF);
        
    }
    

    /**
     * attempt to find the requested file on disk
     *
     */
    function _findSMfile_disk($fName, $GCSpath, $extension='', $fatalFNF=true) {
        
        // debug log at verbose level 1
        $this->debugLog("attempting to load SiteManager resource file from disk: $fName ($GCSpath, $extension)",6);

        // check for a common error of passing an array as filename
        if (is_array($fName)) {
            $this->fatalErrorPage("Invalid findSMfile call: fName was array: ".join(',',$fName));
        }

        // add extension if not there
        if (!empty($extension) && !empty($fName)) {
            if ( substr($fName, (strlen($fName)-strlen($extension))-1, strlen($extension)+1) != '.'.$extension)
                $fName = $fName.'.'.$extension;
        }

        // did they specify a path?
        if ( (!strstr($fName, '/')) && (!strstr($fName, '\\')) )  {

            // nope, they didn't specify a path. search paths from globalConfig (merge GLOBAL)
            $path = $this->siteConfig->getVar('dirs', $GCSpath, true);

            // check multiple paths
            if (is_array($path)) {
                $realPath = '';
                foreach ($path as $p) {
                    $check1 = $p.'/'.$fName;
                    if (file_exists($check1) && !is_dir($check1)) {
                        //If we found something, we're done.
                        $this->debugLog("found: $check1", 6);
                         return $check1;
                    }
                }
                
                // If we didn't find anything and we want to throw
                // and error because we couldn't find the file, do so.
                if ($fatalFNF)
                    $this->fatalErrorPage("Requested SiteManager file ($fName) was not found in the path array:<br>".join($path,'<br>'));

            } else if(file_exists($path.'/'.$fName)) {

                    // Only one place to check. If it's there, we're done.
                    $p = $path.'/'.$fName;
                    $this->debugLog("found: $p", 6);
                    return $p;

            } else if($fatalFNF) {

                    //If we fall through to here, the file doesn't exist and we want to throw an error.
                    $this->fatalErrorPage("Requested SiteManager file ($fName) was not found, and the search path array was empty.");

            }

        } else {
            
            // They specified a path, so we're not checking any of our paths.
            // either php can find the file or it can't 
            if (file_exists($fName)) {

                // found file
                $this->debugLog("found: $fName", 6);
                return $fName;

            } else if ($fatalFNF) {

                // File not found. throw an error.
                $this->fatalErrorPage("Requested SiteManager file ($fName) was not found, and the search path array was empty."); 

            }
        }

        // If we made it this far, we didn't find the file and
        // we didn't want to throw an error. return false.
        return false;

    }

    /**
     * create a new instance of a template from the filename given
     * @param string $fName the template file to load. will automatically add .tpt if not included, and check 
     *                        template path
     * @param bool $fatalFNF when true, if the file wasn't found, will exit with SM_fatalErrorPage
     * @return SM_loadTemplate  a new SM_loadTemplate object (by value), or false if not found and fatalFNF=false
     */
    function loadTemplate($fName, $fatalFNF=true) {
        
        // convert fName to SiteManager path'd version
        $fName = $this->findSMfile($fName, 'templates', 'tpt', $fatalFNF);
        if ($fName === false) {
            return NULL;
        }

        // in cache? return a cloned copy (post parse)
        if (isset($this->templateCache[$fName])) {
            return clone $this->templateCache[$fName];
        }

        // create the template
        $newTemplate = new SM_layoutTemplate($fName, true);

        // cache it (post parse)
        $this->templateCache[$fName] = $newTemplate;

        // return new template
        return $newTemplate;
    
    }

    /**
     * return an array of useful information about a database connection
     * @return array db info
     */
    function getDatabaseConnectionInfo($dbHandle) {
        $dbSettings = $this->siteConfig->getSection('db',$dbHandle);
        if (empty($dbSettings)) {
            return NULL;
        }
        if (preg_match('/dbname=(\w+)/', $dbSettings['DSN'], $m)) {
            $dbSettings['dbName'] = $m[1];
        }
        if (preg_match('/host=([-a-zA-Z0-9.]+)/', $dbSettings['DSN'], $m)) {
            $dbSettings['hostName'] = $m[1];
        }
        return $dbSettings;
    }
    
    /**
     * wrapper function for defineDBconnection, for backward compatibility.
     * FIXME: note, this function is deprecated, and will be removed before 2.4.x release!
     */
    function _defineDBconnection($id, $dbSettings) {
        return $this->defineDBconnection($id, $dbSettings);
    }

    /**
     * define a new database connection
     * @param string $id the database ID to use. this is the key to the dbHL hash
     * @param array $dbSettings either one key of DSN, or four keys of dbType,userName,passWord,hostName defined
     * @param bool $dieOnError when true, this function will exit, issuing an error message if the connection couldn't be made
     * @return bool true on successfull creation, false otherwise
     */
    function defineDBconnection($id, $dbSettings, $dieOnError=true) {

        // if this connection isn't active, skip it
        if (isset($dbSettings['active']) && $dbSettings['active'] === false) {
            $this->debugLog("skipping database connection [$id], since it is flagged as inactive");
            return false;
        }

        // build a DSN for them if there isn't one
        if (empty($dbSettings['DSN'])) {
            var_dump($dbSettings);
            $this->fatalErrorPage('DSN is required');
        }

        // if 'required' flag is set, update dieOnError flag. this lets XML override parameter
        if (isset($dbSettings['required']) && (is_bool($dbSettings['required'])))
            $dieOnError = $dbSettings['required'];        

        // if they didn't specify whether to make a persistent connection or not,
        // use the default from globalConfig
        if (empty($dbSettings['persistent'])) {
            $dbSettings['persistent'] = $this->siteConfig->getGlobalVar('db','persistent');
        }

        // create a PEAR database object
        try {
            $this->dbHL[$id] = new SM_dbWrapper($id,
                                                $dbSettings['DSN'],
                                                $dbSettings['userName'],
                                                $dbSettings['passWord'],
                                                array(PDO::ATTR_PERSISTENT => $dbSettings['persistent']));
        } catch (PDOException $e) {
            if ($dieOnError) {
                $this->debugLog($e->getMessage()."<br />\n");
                $this->fatalErrorPage("Unable to connect to database - please notify an administrator.");
            }
            else {            
                $this->debugLog("database connection [$id] failed, but was flagged as non required: ignoring");
                return false;
            }
        }

        // error mode
        $this->dbHL[$id]->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_WARNING);
        
        // backwards compatibility -- buffered by default
        if ($this->dbHL[$id]->getAttribute(PDO::ATTR_DRIVER_NAME) == 'mysql') {
            $this->dbHL[$id]->setAttribute(PDO::MYSQL_ATTR_USE_BUFFERED_QUERY,true);
        }

        // debug log it
        $this->debugLog("created database connection with id [$id]");

        // if this is the first database connection, or they specified this
        // connection to be the default... make it so number one
        if (empty($this->dbH) || (isset($dbSettings['defaultConnection']) && ($dbSettings['defaultConnection'] === true)) ) {
            $this->dbH = $this->dbHL[$id];
            $this->debugLog("database id [$id] is now the default database connection");
        }

        return true;

    }


    /**
     * create a new database connection using settings for this SITE
     */
    function dbConnect() {
        
        if (!class_exists('PDO')) {
            $this->fatalErrorPage("PDO DB routines are not available! You will need to recompile PHP or load the extension first.");
        }

        // check for multiple database connections
        if ($this->siteConfig->hasMultipleID('db')) {

            // multiple db connections
            $idList = $this->siteConfig->getIDlist('db');
            
            // loop through id's and setup database connections
            foreach ($idList as $dbID) {

                $dbSettings = $this->siteConfig->getSection('db',$dbID);

                // if global active flag is set to false, and local db is defined
                // but doesn't set active flag, set active to true
                if (($this->siteConfig->getGlobalVar('db','active',$dbID) === false) &&
                    ($this->siteConfig->getLocalSection('db',$dbID) !== NULL) &&
                    ($this->siteConfig->getLocalVar('db','active',$dbID) === NULL) ) {
                    $dbSettings['active'] = true;
                }

                $this->defineDBconnection($dbID, $dbSettings);

            }

        }
        else {        

            // setup single default database connection
            $dbSettings = $this->siteConfig->getSection('db');            
            $this->defineDBconnection('default',$dbSettings);

        }


    }

    /**
     * start sessions. this will create and setup an instance of SM_session
     * use addPersistent to add variables to keep persistant between sessions
     * @see SM_siteManagerRoot::addPersistent()
     */
    function startSessions() {

        $this->sessionH = new SM_session();
        $this->sessionH->runSession();

    }

    /**
     * add persistent variables to the session. once variables have been added
     * to the persistent list, they will be kept alive throughout session-aware modules
     * once they obtain a value     
     * @param string $varName the variable name to keep persistant
     */
    function addPersistent($varName) {

        if (isset($this->sessionH))
            $this->sessionH->addPersistent($varName);

    }

    /**
     * complete the page. called in a directive template to run all modules,
     * collect their output and display the final page in the root template.
     * this should be the last function that runs in a directive script
     * also saves session state upon completion
     * @param bool $fatalDebug when true, a debug page will be generated
     */
    function completePage($fatalDebug=false) {

        global $SM_scriptStartTime;

        // can't do this if there's no root template
        if (isset($this->rootTemplate)) {
            // run the root template
            $pageOutput = $this->rootTemplate->run();
        }
        else {
            $this->debugLog("completePage() - root template was not created");
            $pageOutput = '';
            $fatalDebug = true;
        }

        // save session state
        if (isset($this->sessionH))
            $this->sessionH->saveSession();

        // get script end execution time
        $microtime = split(' ', microtime());
        $SM_scriptStopTime = $microtime[1].substr( $microtime[0], 1);
        $scriptLoadTime = $SM_scriptStopTime - $SM_scriptStartTime;

        // debug?
        $debugOutput = '';
        if ($fatalDebug) {
            
            // header
            $debugOutput = "<br /><b>SITEMANAGER ROOT CLASS FATAL DEBUG PAGE</b><br /><hr /><br />";

            $debugOutput .= "<b>Script Load Time</b>: $scriptLoadTime<br /><br />\n";

            // dump site config
            $debugOutput .= $this->siteConfig->dumpInfo();

            // database info
            if ($numDB = sizeof($this->dbHL)) {
                $debugOutput .= "<hr /><b>DATABASE INFORMATION</b><hr /><br />\n";
                $debugOutput .= "There are $numDB database connection(s) open<br /><br />";
            }
    
            // session debug output
            if (isset($this->sessionH)) {
                $debugOutput .= "<hr /><b />SESSION INFORMATION</b><hr /><br />\n";
                $debugOutput .= $this->sessionH->dumpInfo();
    
                $debugOutput .= "Propagated InVars:<br />";
                if (is_array($this->inVarH->inVarPList)) {            
                    foreach ($this->inVarH->inVarPList as $ivpVar) {
                        $debugOutput .= "[$ivpVar]<br />";
                    }
                }
                else {
                    $debugOutput .= 'None<br />';
                }
    
                $debugOutput .= "<br /><hr /><br />\n";
            }
    
            // dump modules loaded information
            if (is_array($this->moduleList)) {
                $debugOutput .= "<hr /><b>MODULE INFORMATION</b><hr />\n";
                foreach ($this->moduleList as $module) {
    
                    $debugOutput .= $module->dumpInfo();
                    $debugOutput .= "<hr />\n";
    
                }
            }
        }

        // stop buffer
        $SM_outputBuffer = ob_get_contents();
        ob_end_clean();

        // SiteManager Powered By header
        header("X-Powered-By: Roadsend PHP SiteManager ".SM_VERSION);
        
        // if debugOnVar, output the debug_log first
        if (($fatalDebug) || ($this->siteConfig->getVar('debug','debugOnVar')) && ($this->inVarH->getGET('SM_debug')==1)) {

            // output buffer
            if (trim($SM_outputBuffer) != '')
                $this->debugLog('<br /><b>OUTPUT BUFFER:</b><br /><pre>['.$SM_outputBuffer.']</pre>');

            // don't output page contents if fatalDebug, just debug info
            if ($fatalDebug)
                $this->debugLog($debugOutput);

            $pageOutput = $this->errorHandler->makeDebugPage();

        }

        // send headers to stop browsers from caching page?
        /*
        if ($this->siteConfig->getVar('flags','sendNoCacheHeaders')) {
            header("Expires: Mon, 26 Jul 1997 05:00:00 GMT");    // Date in the past
            header("Last-Modified: " . gmdate("D, d M Y H:i:s") . " GMT");                                                                                                              // always modified
            header("Cache-Control: no-cache, must-revalidate");  // HTTP/1.1
            header("Pragma: no-cache");                          // HTTP/1.0
        }
        */

        // echo page
        echo $pageOutput;

        // show page load time?
        if ($this->siteConfig->getVar('flags','showLoadTime'))
            echo "\n<!-- page load time: $scriptLoadTime seconds -->\n";

        // end!
        exit;

    }

    /**
     * private function used by the session system to get a list of inVars from the 
     * modules currently loaded.
     * @return array a list of inVars from each module loaded
     */
    function _getModuleInVars() {
        return $this->inVarH->inVarPList;
    }

    /**
     * compile a debug page that includes all current SiteManager information, 
     * including configuration settings, database settings, session settings,
     * which modules are loaded, etc.
     * NOTE: this method is deprecated, use completePage(true)
     */
    function fatalDebugPage() {
        $this->completePage(true);
    }


    /**
    * refresh some global variables. called after reading in configuration files
    */
     
    function _updateGlobals() {

        global $SM_develState, $SM_defaultLocale, $SM_debugLevel;

        $SM_develState      = $this->siteConfig->getVar('debug','develState');
        $SM_debugLevel      = $this->siteConfig->getVar('debug','debugLevel');
        $SM_defaultLocale   = $this->siteConfig->getVar('localization','defaultLocale');

    }

}

?>
