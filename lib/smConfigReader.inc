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
 * implementation for a base class that will read configuration data
 * from either an XML file or a database schema
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smConfigReader
 */
class SM_configReader extends SM_Object {

    /** 
    * current state of XML elements 
    * @var array
    */
    var $_XMLstate = array();
    
    /** 
    * a reference to the user object to set object properties on 
    * @var SM_configReader
    */
    var $userObject = NULL;
    
    /**
     * XML read buffer
     *
     */
    var $contentBuffer = '';

    /**
     * current file loaded, if any
     */
    var $currentFile = '';

    /**
     * XML file encoding
     */
    var $encoding = 'ISO-8859-1';
    
    /**
    * constructor
    */
     
    function SM_configReader() {

        // SM_object
        $this->_smoConfigure();

        // determine my reader object name
        $className = get_class($this);
        $readerName = substr($className, strlen('SM_configReader_'));

        // defaults
        $this->directive['useCache']   = true;  // whether to use this cache at all or not
        $this->directive['cacheTTL']   = 3600;  // TTL of 1 hour
        
        // get cache configuration from GCS (only if siteConfig exists at this point)
        /*
        if (isset($this->siteConfig)) {
            $cacheConfig = $this->siteConfig->getSection('cacheConfig',$readerName);
            $this->configure($cacheConfig);
        }
        */

    }

    /**
    * set the user of the reader
    */
     
    function setUser($user) {
        $this->userObject = $user;
    }

    /**
     * set the XML content to parse, retrieved from some external place
     * $param string $data a string with XML data to be parsed
     */
    function XMLsetContent($data) {
        $this->contentBuffer = $data;
        $this->currentFile = '';
    }

    /**
     * parse an XML file and retrieve it's contents
     * @param string $fileName the full path of the XML file
     */
    function readConfigXML($fileName) {

        $this->currentFile = $fileName;
        $this->_getFileXML($fileName);
        $this->_parseXML();
    }

    /**
     * retrieve configuration information from a database
     * to implement: fill $this->contentBuffer with XML data then call $this->_parseXML 
     * @param string $dbHandle the index of the database connection to use (to $dbHL)
     */
    function readConfigDB($dbHandle) {
        // virtual
        $this->debugLog("this configReader does not implement readConfigDB()");
    }

    /**
     * generate a (hopefully unique) cache file name based on the
     * full path and file name of a file to be cached
     * @param mixed $fileName full path and file name of file to be cached
     */
    /*
    function getCacheFileName($fileName) {

        if (is_array($fileName)) {
            // generate file name
            $fileID = join('_',$fileName);
            $fileID = preg_replace('/\W/', '_', $fileID);
        }
        else {
            // generate file name
            $fileID = preg_replace('/\W/', '_', $fileName);
        }

        // get base directory
        $cacheDir = $this->siteConfig->getGlobalVar('dirs','cache'); 

        return $cacheDir.$fileID.'.inc';

    }
    */

    /**
     * determine if this reader should cache it's contents or not, based on GCS settings
     */
    function useCache() {
        return ($this->directive['useCache'] && function_exists('xcache_set'));
    }

    /**
     * determine if a file has been cached
     * @param mixed $fileName full path and file name of file to be cached
     */
    /*
    function isFileCached($fileName) {
        $cacheFile = $this->getCacheFileName($fileName);
        return (is_file($cacheFile));
    }
    */

    /**
     * determine if a file needs to be recached
     * @param mixed $fileName full path and file name of file to be cached
     */
    /*
    function isCacheStale($fileName) {

        // if file isn't cached..
        if (!$this->isFileCached($fileName)) {
            $this->debugLog("isCacheStale: file isn't cached",2);
            return true;
        }

        $cacheFile = $this->getCacheFileName($fileName);

        if (is_array($fileName)) {
            foreach ($fileName as $fn) {
                $modTime = filemtime($fn);
                if ($modTime == false || $modTime <= 0) {
                    $this->fatalErrorPage("tried to examine cache file $fileName, but an error occured");
                }
                $mT[$fn] = $modTime;
            }
        }
        else {
            // get last modified time of file
            $modTime = filemtime($fileName);
            if ($modTime == false || $modTime <= 0) {
                $this->fatalErrorPage("tried to examine cache file $fileName, but an error occured");
            }
        }

        // get last modified time of cache file
        $cModTime = filemtime($cacheFile);
        if ($cModTime == false || $cModTime <= 0) {
            $this->fatalErrorPage("tried to examine cache file $cacheFile, but an error occured");
        }

        // if using autoExpire, if the mod time on the file is later than the cache, recache it
        if ($this->directive['autoExpire']) {
            if (is_array($fileName)) {
                foreach ($fileName as $fn) {
                    if ($mT[$fn] > $cModTime) {
                        $this->debugLog("isCacheStale: autoExpire in effect",2);
                        // file should be expired because it's been changed since it was cached
                        return true;
                    }
                }
            }
            else {
                if ($modTime > $cModTime) {
                    $this->debugLog("isCacheStale: autoExpire in effect",2);
                    // file should be expired because it's been changed since it was cached
                    return true;
                }
            }
        }

        // otherwise check TTL
        $lifeTime = (time() - $cModTime);
        if ($lifeTime > $this->directive['cacheTTL']) {
            $this->debugLog("isCacheStale: cacheTTL time hit ($lifeTime > {$this->directive['cacheTTL']})",2);
            // file is old
            return true;
        }        

        // if we get here, file is not stale
        return false;

    }
    */

    /** 
     * cache the contents of our reader to a PHP include file
     * @param mixed $fileName full path and file name of file to be cached
     * @param mixed $data data to be cached, passed by controlling object
     */
    /*
    function cacheContents($fileName, $data) {

        // if we're told not to cache in GCS, don't
        if ($this->directive['useCache'] == false) {
            return false;
        }

        // invoke the overridden cache method
        return $this->writeCache($this->getCacheFileName($fileName), $data);

    }
    */

    /**
     * return contents from a cached file
     * @param mixed $fileName full path and file name of file to be cached
     */
    /*
    function getCachedContents($fileName) {

        // check for stale (or non-existant) cache file
        if ($this->isCacheStale($fileName)) {
            return NULL;
        }

        $cacheFile = $this->getCacheFileName($fileName);

        // include the PHP file
        include_once($cacheFile);

        // globalize data structure
        global $SM_READER_CACHECONTENTS;

        // return data structure that should be declared global by the include file
        return $SM_READER_CACHECONTENTS;

    }
    */

    /**
    * cache an SM_config file to PHP includable data
    * may be overridden by readers, default uses serialization
    */
    /*
    function writeCache($cacheFile, $data) {

        // open the file
        $cFile = fopen($cacheFile, 'w');
        if ($cFile == false) {
            $this->fatalErrorPage("Unable to write to cache file $cacheFile");
        }

        // write data
        fputs($cFile, '<?php global $SM_READER_CACHECONTENTS; ');
        
        fputs($cFile, "\$data = <<< END\n".serialize($data)."\nEND;\n");
        fputs($cFile, '$SM_READER_CACHECONTENTS=unserialize($data);');

        fputs($cFile, ' ?>');

        // close file
        fclose($cFile);

    }
    */
    
    /**
    * function triggered when an XML start element appears
    * here we will parse tag's we recognize from the config file
    * @param reference $parser the XML parser
    * @param string $name the tag found
    * @param array $attrs the attributes this tag carried
    */
    function startElement($parser, $name, $attrs) {    
        // update state for this element
        $this->_XMLstate['in'.$name] = true;
    }
    
    /**
    * function triggered when an XML end element appears
    * @param reference $parser the XML parser
    * @param string $name the tag found    
    */
    function endElement($parser, $name) {
        // update state for this element
        $this->_XMLstate['in'.$name] = false;
    }

    /**
    * function triggered when character data is encountered in an XML file
    */
     
    function cDataHandler($parser, $data) {
        // virtual
    }

    /**
    * function triggered when an external entity ref is encountered
    */
     
    function externalEntityRefHandler($parser, $openEntityNames, $base, $systemID, $publicID) {
        // virtual                                                                                                
    }

    /**
    * function triggered when an unparsed notation declaration is encouneted
    */
     
    function unparsedEntityDeclHandler($parser, $entityName, $base, $systemID, $publicID, $notationName) {
        // virtual                                                                                                
    }

    /**
    * function triggered when a notation declration is encountered
    */
     
    function entityDeclHandler($parser, $notationName, $base, $systemID, $publicID) {
        // virtual                                                                                                
    }


    /**
    * function triggered when a processing instruction entity is encountered
    */
     
    function piHandler($parser, $target, $data) {
        // virtual
    }

    /**
    * default handler for XML parser
    */
     
    function defaultHandler($parser, $data) {
        // virtual
    }

    /**
     * get XML content from file into bufer
     * @param string $file the file to parse
     */
    function _getFileXML($file) {

        if (empty($file))
            $this->fatalErrorPage("XML file to parse was not set");

        if (!($fp = fopen($file, "r", 1))) {
            $this->fatalErrorPage("could not open XML input from file $file");
        }
        
        $this->contentBuffer = NULL;
        while ($data = fread($fp, 4096)) {
                $this->contentBuffer .= $data;
        }

       fclose($fp);

    }

    /**
     * backward compatibility
     */
    function _parseXML() {
        $this->parseXML();
    }

    /**
    * private function for parsing the XML data. actually creates XML parser
    * and sets up element handlers
    */
    function parseXML() {
    
        $xml_parser = xml_parser_create($this->encoding);
        xml_parser_set_option($xml_parser, XML_OPTION_CASE_FOLDING, 1);

        xml_set_object($xml_parser, $this);
        xml_set_element_handler($xml_parser, 'startElement', 'endElement');
        xml_set_character_data_handler($xml_parser, 'cDataHandler');

        xml_set_external_entity_ref_handler($xml_parser, 'externalEntityRefHandler');
        xml_set_processing_instruction_handler($xml_parser, 'piHandler');
        xml_set_unparsed_entity_decl_handler($xml_parser, 'unparsedEntityDeclHandler');
        xml_set_notation_decl_handler($xml_parser, 'entityDeclHandler');
        xml_set_default_handler($xml_parser, 'defaultHandler');
        
        if (!xml_parse($xml_parser, $this->contentBuffer, true)) {
            ($this->currentFile != '') ? $inFile = "in file $this->currentFile" : $inFile = '';
            if ($GLOBALS['SM_develState']) {
                echo "XML dump: \n".$this->contentBuffer;
            }
            $this->fatalErrorPage(sprintf(get_class($this).": XML error: %s at line %d $inFile",
                        xml_error_string(xml_get_error_code($xml_parser)),
                        xml_get_current_line_number($xml_parser)));
        }

        xml_parser_free($xml_parser);

    }

}

/**
 * load a config reader
 * @param string $rName the config reader to load
 * @param SM_configReader $user the user of the reader
 * @return SM_configReader reference to newly created reader
 */
function SM_loadConfigReader($rName, $user, $rFile='') {
    
    // globalize
    global $SM_siteManager;

    // rFile shouldn't be passed unless this is SMCONFIG calling us
    if ($rFile == '') {
        // find file
        $rFile = $SM_siteManager->findSMfile($rName.'_reader', 'configReaders', 'inc', true);
    }

    // include the file
    include_once($rFile);

    // found it, instantiate and return
    $className = 'SM_configReader_'.$rName;
    $reader = new $className();
    $reader->setUser($user);

    return $reader;
}

?>
