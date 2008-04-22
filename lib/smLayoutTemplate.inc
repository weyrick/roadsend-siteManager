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

// SM tag handler classes
include($SM_rootDir.'lib/smTags.inc');

// defines
define('SM_TAG_IDENTIFIER','SM'); // if you want something other than <SM> tags, change this
define('SM_TAG_PREGEXP','/<\s*'.SM_TAG_IDENTIFIER.'\s(.+)\s*\/*>/Ui');
define('SM_TAG_EXTD_PREGEXP','/<\s*'.SM_TAG_IDENTIFIER.'\s(.+)\s*>(.*?)<\/'.SM_TAG_IDENTIFIER.'\s*>/Ui');
define('SM_TAG_ATTR_PREGEXP','/([-_a-zA-Z0-9]+)\s*=\s*[\"\'](.*)[\"\']/SUi');
define('SM_TAG_EMBEDDEDTPT','/%%%(\w+)%%%(.+)%%%\\1%%%/si');
define('SM_AREAID_KEY','_SMarea-');
define('SM_TAG_CLASS','SM_smTag_');

/**
 *
 * the SM_layoutTemplate class is the base class for creating a 
 * layout template in which to add modules
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager 
 * @subpackage siteManager_template 
 */
class SM_layoutTemplate extends SM_object {
        
    /** (array) list of valid SM tags in this template, keyed by AREA name, which might be generated */
    var $smTagList = array();

    /** (array) list of quick tags in this template, keyed by tagID */
    var $quickTagList = array();

    /** (string) the actual HTML template file name, with encoded AREA information */
    var $htmlTemplateFile = NULL;
    
    /** (array) template broken on newlines. this could come from a file (htmlTemplateFile) or
                 set through the setTemplateData() method */
    var $htmlTemplate = '';

    /** (string) the raw block of text our template uses */
    var $htmlRawTemplate = '';

    /** (array) subtemplates extracted from this template */
    var $subTemplateList = array();
    
    /** (string) the html output before tags are run, but after parsing is done. one big string. */
    var $htmlParsedOutput = '';

    /** (string) the final html output. one big string. */
    var $htmlOutput = '';
        
    /** (array) a hash of tag priority levels, so we know in which order to execute tags: [priority][] = areaName */
    var $tagPriorityList = array();

    /** (string) collection of javascript output used in this template. used by tag system (AREA) */
    var $jsOutput = '';

    /** (array) a list of SM tag TYPE's we will allow in this template. if empty, list is ignored */
    var $validTagTypeList = array();

    /** (array) a list of SM tag TYPE's we will specifically not allow. if emtpy, list is ignored */
    var $excludeTagTypeList = array();

    /** (bool) flag to keep track of whether we've parsed or not */
    var $parsed = false;

     /**
      * constructor loads in and parses an HTML template. also sets up
      * database and session handlers
      *
      * @param $htmlTemplate (string) the full path of the HTML template to open, if any
      * @param $parse (bool) when true, the template data will be parsed immediately. otherwise,
      *                      you must call parseTemplate() yourself
      */
    function SM_layoutTemplate($htmlTemplateFile = NULL, $parse = false) {
             
        // defaults
        $this->htmlTemplateFile = $htmlTemplateFile;

        // setup handlers
        $this->_smoConfigure();

        // configure template
        $this->_templateConfig();
    
        // parse?
        if ($parse)
            $this->parseTemplate();

    }
    
    /**
     * base template configuration
     */
    function _templateConfig() {

        // grab template directives
        $this->directive  = $this->siteConfig->getSection('templates');

        // if we're allowing quick tags, do some configuration
        if ($this->directive['allowQuickTags']) {
            $this->directive['SM_QUICKTAG_PREGEXP'] = '/'.$this->directive['quickTagIdentifier'].'(\w+)'.$this->directive['quickTagIdentifier'].'/';
        }

    }

    /**
     * override SM_object::addDirective() to handle compatibility issues
     *
     */
    function addDirective($key, $val='', $overWrite=true) {

        // backwards compatibility
        if ($this->siteConfig->getVar('compatibility','2.2.x')) {
            
            // convert to new directive format

            // HTMLBODY tag
            if ($key == 'bodyBGCOLOR')
                $this->directive['HTMLBODY']['bodyBGCOLOR']     = $val;
            if ($key == 'bodyLINK')
                $this->directive['HTMLBODY']['bodyLINK']        = $val;
            if ($key == 'bodyALINK')
                $this->directive['HTMLBODY']['bodyALINK']       = $val;
            if ($key == 'bodyVLINK')
                $this->directive['HTMLBODY']['bodyVLINK']       = $val;
            if ($key == 'bodyBACKGROUND')
                $this->directive['HTMLBODY']['bodyBACKGROUND']  = $val;
            if ($key == 'bodyONLOAD')
                $this->directive['HTMLBODY']['bodyONLOAD']      = $val;
            if ($key == 'bodyOther')
                $this->directive['HTMLBODY']['bodyOther']       = $val;

            // HTMLTITLE tag
            if ($key == 'htmlTitle')
                $this->directive['HTMLTITLE']['htmlTitle']      = $val;

            $this->debugLog("warning: using backward compatibility mode - you should update to ".SM_VERSION.' compatible');

        }

        // now call parent
        parent::addDirective($key, $val, $overWrite);

    }


    /**
     * method for setting directives for tags in this template
     * note that these directives apply to all tags of this type in this template
     */
    function configureTag($tagName, $directives) {        
        $this->addDirective(strtoupper($tagName), $directives);
    }

    /**
     * define a list of tag TYPE's to specifically exclude from this template
     * @param $eList (array) an array of tag TYPE's to exclude: NOTE tags should be in uppercase!
     */
    function setExcludeTagTypeList($eList) {
        $this->excludeTagTypeList = $eList;
    }

    /**
     * define a list of tag TYPE's to specifically allow in this template
     * @param $vList (array) an array of valid tag TYPE's (all others ignored) NOTE tags should be in uppercase!
     */
    function setValidTagTypeList($vList) {
        $this->validTagTypeList = $vList;
    }

    /**
     * return an array of smTags that appear in this template by tag type
     * @param $tagType (string) the tag TYPE to match on
     */
    function getTagsByType($tagType) {

        $tagList = array();
        foreach ($this->smTagList as $areaName => $tagObj) {
            $tType = $tagObj->getAttr('TYPE');
            if (strtoupper($tType) == strtoupper($tagType)) {
                $tagList[] = $this->smTagList[$areaName];
            }
        }

        return $tagList;

    }

    /** 
     * set the template text block to parse. use this if you want to parse a block
     * of text (for example, loaded from a database) instead of from a template file
     * @param $templateData (string) the text to parse     
     * @param $parse (bool) when true, the text will be parsed after it's set. otherwise you must
     *                      call parseTemplate() yourself
     * @param $doSubTemplates (bool) when true, extra sub templates from templateData
     */
    function setTemplateData($templateData, $parse=true, $doSubTemplates=true) {
        // capture template, normalizing line endings
        $this->htmlRawTemplate = str_replace(array("\r\n","\n\r","\r"),"\n",$templateData);
        if ($doSubTemplates)
            $this->extractSubTemplates();
        $this->htmlTemplate = explode("\n",$this->htmlRawTemplate);
        if ($parse) {
            return $this->parseTemplate();
        }
    }


    /**
     * extract any sub templates within this template. these are named as part of the 
     * sub template specificatino: %%%SUBTEMPLATENAME%%%
     * we store a hash, keyed by the sub template name, of SM_layoutTemplate objects which
     * are created as needed
     *
     * after creating the sub template, the main template will have the sub template code extracted,
     * and in it's place a new SM AREA tag is inserted, with the same name as the sub template
     * 
     */
    function extractSubTemplates() {

        if (preg_match_all(SM_TAG_EMBEDDEDTPT, $this->htmlRawTemplate, $m)) {
                
            for ($i=0; $i < sizeof($m[0]); $i++) {
                
                // create sub template
                $subTpt = new SM_layoutTemplate();
                $subTpt->setTemplateData($m[2][$i]);
                $this->subTemplateList[$m[1][$i]] = $subTpt;
                
                // replace with area tag
                $this->htmlRawTemplate = str_replace($m[0][$i],'<sm type="area" name="'.$m[1][$i].'">', $this->htmlRawTemplate);
                
            }
            
        }
        
    }

    /**
     * fetch a sub template, if it exists.
     * @param $name (string) the sub template name, as specified in the sub template tag
     * @return (mixed) the SM_layoutTemplate object of the sub template, or NULL if not found
     */
    function getSubTemplate($name) {
        if (isset($this->subTemplateList[$name]))
            return $this->subTemplateList[$name];
        else
            return NULL;
    }
    
    /**
     * initial parse template routine. if siteManager php extension is available, it will be
     * used for parsing and output. otherwise, use the builtin PHP code
     *
     */
    function parseTemplate() {

        // use slower php based parser
        $this->_php_parseTemplate();

    }

    /**
     * parse the HTML template file. find and handle all SM tags
     *
     * @return (bool) true on success, false on failure
     */
    function _php_parseTemplate() {

        global $SM_siteManager;

        // TEMPLATE FILE
        // if htmlTemplateFile is set, load template from a file

        // if this->htmlTemplate doesn't have a path, use SM_siteManager to figure it out
        if (isset($this->htmlTemplateFile))  {

            // no path, so search siteManager paths. error if not found.
            if (!preg_match('/[\/\\\]/',$this->htmlTemplateFile))
                $this->htmlTemplateFile = $SM_siteManager->findSMfile($this->htmlTemplateFile, 'templates', 'tpt');
            
            // check for existence
            if (!file_exists($this->htmlTemplateFile)) {
                $this->debugLog("template file $this->htmlTemplateFile was not found, ignoring");
                return false;
            }

            // detect infinite parse loops
            global $SM_TEMPLATE_PARSE_LIST;
            if (!isset($SM_TEMPLATE_PARSE_LIST[$this->htmlTemplateFile]))
                $SM_TEMPLATE_PARSE_LIST[$this->htmlTemplateFile] = 1;
            else
                $SM_TEMPLATE_PARSE_LIST[$this->htmlTemplateFile]++;
            if ($SM_TEMPLATE_PARSE_LIST[$this->htmlTemplateFile] > 50) {
                $this->debugLog("infinite loop detected");
                echo SM_makeErrorPage();
                die();
            }

            // load the template in, normalizing line endings
            $this->htmlRawTemplate = str_replace(array("\r\n","\n\r","\r"),"\n",file_get_contents($this->htmlTemplateFile));
            // extra sub templates, if any
            $this->extractSubTemplates();
            // setup for parsing
            $this->htmlTemplate = explode("\n", $this->htmlRawTemplate);

        }

        // make sure we have data
        if (sizeof($this->htmlTemplate) == 0) {
            $this->debugLog("template data was blank, ignoring");
            return false;
        }

        // go line by line and handle SM tags, swapping out areaName's for a uniq id
        // you CAN have multiple SM tags on one line
        // but you CANNOT break them across multiple lines!
        $this->htmlParsedOutput   = '';
        
        // for each line in the file...
        foreach ($this->htmlTemplate as $line) {

            // add our newline in (was removed during load)
            $line .= "\n";
            
            // use this to determine what kind of tag we're dealing with
            $tagType = '';
            $areaSub = '';

            // check for standard SM tags
            if (preg_match_all(SM_TAG_PREGEXP,$line,$smTagMatch)) {
                
                // for each match we found on this line...
                for ($i=0; $i < sizeof($smTagMatch[1]); $i++) {
                    
                    // grab TYPE parameter
                    $smParms = $smTagMatch[1][$i];

                    // parse the attributes
                    unset($attrs);
                    preg_match_all(SM_TAG_ATTR_PREGEXP,$smParms,$matchList);
                    for ($x = 0; $x<count($matchList[1]); $x++) {
                        $key = strtoupper($matchList[1][$x]);
                        $attrs[$key] = $matchList[2][$x];
                    }

                    if (isset($attrs['TYPE'])) {
                        $tagType = strtoupper($attrs['TYPE']);
                    }
                    else {
                        // warning -- type not found, ignore
                        $this->debugLog("warning: SM tag had no TYPE parameter");
                        continue;
                    }
                    
                    // if validList is set, make sure this tag is in it
                    if (sizeof($this->validTagTypeList) > 0) {
                        if (!in_array($tagType, $this->validTagTypeList)) {
                            // wasn't in the valid list
                            $this->debugLog("skipping tag $tagType during parsing: not in valid tag list");
                            continue;
                        }
                    }

                    // if excludeList is set, make sure this tag is in it
                    if (sizeof($this->excludeTagTypeList) > 0) {
                        if (in_array($tagType, $this->excludeTagTypeList)) {
                            // was in the exclude list
                            $this->debugLog("skipping tag $tagType during parsing: in exclude tag list");
                            continue;
                        }
                    }

                    // try to create an SM_smTag class that matches this tag type
                    $tagClassName = SM_TAG_CLASS.$tagType;

                    if (SM_loadTag($tagType)) {

                        // tag handler classes
                        $tagClass = new $tagClassName();
                        $tagClass->setParent($this);
                        
                        // initial configuration of tag
                        $tagClass->setTagData($smTagMatch[0][$i], $attrs);

                        // cache for area tags
                        if ($tagType == 'AREA') {                        
                            $areaName = $attrs['NAME'];
                            if (empty($areaName)) {
                                $this->debugLog("found SM AREA tag with no NAME attribute");
                            }
                        }
                        else {
                            $areaName = SM_AREAID_KEY.count($this->smTagList);
                        }

                        // add it to the smTagList (NO REFERENCE)
                        $this->smTagList[$areaName] = $tagClass;

                        // add it to priority list
                        $this->tagPriorityList[$tagClass->thinkPriority][] = $areaName;

                    }
                    else {
                        $this->debugLog("warning: SM tag has TYPE we didn't know how to handle ($tagType) ignoring");
                        continue;
                    }
                
                }

            } // end of main tag block
                        
            // check for extended SM tags
            if ($this->directive['allowExtendedTags']) {

                if (preg_match_all(SM_TAG_EXTD_PREGEXP,$line,$smTagMatch)) {

                    // for each match we found on this line...
                    for ($i=0; $i < sizeof($smTagMatch[1]); $i++) {

                        // grab TYPE parameter
                        $smParms = $smTagMatch[1][$i];

                        // any data?
                        if (isset($smTagMatch[2][$i]))
                            $tagData = $smTagMatch[2][$i];

                        // parse the attributes
                        unset($attrs);
                        preg_match_all(SM_TAG_ATTR_PREGEXP,$smParms,$matchList);
                        for ($x = 0; $x<count($matchList[1]); $x++) {
                            $key = strtoupper($matchList[1][$x]);
                            $attrs[$key] = $matchList[2][$x];
                        }

                        if (isset($attrs['TYPE'])) {
                            $tagType = strtoupper($attrs['TYPE']);
                        }
                        else {
                            // warning -- type not found, ignore
                            $this->debugLog("warning: SM tag had no TYPE parameter");
                            continue;
                        }

                        // if validList is set, make sure this tag is in it
                        if (sizeof($this->validTagTypeList) > 0) {
                            if (!in_array($tagType, $this->validTagTypeList)) {
                                // wasn't in the valid list
                                $this->debugLog("skipping tag $tagType during parsing: not in valid tag list");
                                continue;
                            }
                        }

                        // if excludeList is set, make sure this tag is in it
                        if (sizeof($this->excludeTagTypeList) > 0) {
                            if (in_array($tagType, $this->excludeTagTypeList)) {
                                // was in the exclude list
                                $this->debugLog("skipping tag $tagType during parsing: in exclude tag list");
                                continue;
                            }
                        }

                        // try to create an SM_smTag class that matches this tag type
                        $tagClassName = SM_TAG_CLASS.$tagType;

                        if (SM_loadTag($tagType)) {

                            // tag handler classes
                            $tagClass = new $tagClassName();
                            $tagClass->setParent($this);

                            // initial configuration of tag
                            $tagClass->setTagData($smTagMatch[0][$i], $attrs, $tagData);

                            // cache for area tags
                            if ($tagType == 'AREA') {
                                $areaName = $attrs['NAME'];
                                if (empty($areaName)) {
                                    $this->debugLog("found SM AREA tag with no NAME attribute");
                                }
                            }
                            else {
                                $areaName = SM_AREAID_KEY.count($this->smTagList);
                            }

                            // add it to the smTagList (NO REFERENCE)
                            $this->smTagList[$areaName] = $tagClass;

                            // add it to priority list
                            $this->tagPriorityList[$tagClass->thinkPriority][] = $areaName;

                        }
                        else {
                            $this->debugLog("warning: SM tag has TYPE we didn't know how to handle ($tagType) ignoring");
                            continue;
                        }

                    }

                } // end of main tag block

            }// end extended tags
            
            // check for quick tags            
            if ($this->directive['allowQuickTags']) {            
                unset($smTagMatch);
                if (preg_match_all($this->directive['SM_QUICKTAG_PREGEXP'],$line,$smTagMatch)) {
                    
                    // for each match we found on this line...
                    for ($i=0; $i < sizeof($smTagMatch[1]); $i++) {
                                                
                        // grab 'type'
                        $tagType = strtoupper($smTagMatch[1][$i]);

                        // if we're already done this type, continue
                        if (isset($this->quickTagList[$tagType]))
                            continue;

                        // if validList is set, make sure this tag is in it
                        if (sizeof($this->validTagTypeList) > 0) {
                            if (!in_array($tagType, $this->validTagTypeList)) {
                                // wasn't in the valid list
                                $this->debugLog("skipping tag $tagType during parsing: not in valid tag list");
                                continue;
                            }
                        }

                        // if excludeList is set, make sure this tag is in it
                        if (sizeof($this->excludeTagTypeList) > 0) {
                            if (in_array($tagType, $this->excludeTagTypeList)) {
                                // was in the exclude list
                                $this->debugLog("skipping tag $tagType during parsing: in exclude tag list");
                                continue;
                            }
                        }

                        // try to create an SM_smTag class that matches this tag type
                        $tagClassName = SM_TAG_CLASS.$tagType;
    
                        if (SM_loadTag(strtoupper($tagType))) {
    
                            // tag handler classes
                            $tagClass = new $tagClassName();
                            $tagClass->setParent($this);
                            
                            // initial configuration of tag
                            $tagClass->setTagData($smTagMatch[0][$i], array('TYPE'=>$tagType));
        
                            // generate areaName
                            $areaName = SM_AREAID_KEY.count($this->smTagList);

                            // add it to the smTagList (NO REFERENCE)
                            $this->smTagList[$areaName] = $tagClass;

                            // add it to priority list
                            $this->tagPriorityList[$tagClass->thinkPriority][] = $areaName;

                            // add it to quickTagList
                            $this->quickTagList[$tagType] = true;
    
                        }
                        else {
                            $this->debugLog("warning: quick tag $tagType had no corresponding tag handler");
                            continue;
                        }
                    
                    }
    
                }            

            } // end of quick tag block            
              
            // add it to our final output string                
            $this->htmlParsedOutput .= $line;
            
        }

        // parsing complete
        $this->parsed = true;

        return true;

    }

             
    /**
     * return true if the areaName passed exists in the template     
     * @param string $areaName areaName to check
     * @return bool true if areaName passed exists in template
     */
    function areaExists($areaName) {
        return in_array($areaName,array_keys($this->smTagList));
    }


    /**
     * return a list of area names in this template     
     *
     * @return array list of areas in this template
     */
    function getAreaList() {
        return array_keys($this->smTagList);
    }

    /**
     * return the attributes that were in the SM AREA tag
     * for a given area as a hash, where the key is the attribute name and
     * the value is the value of the attribute. note that the attribute key
     * will be converted to uppercase
     * @param string $areaName the area to lookup
     * @return array the attributes from the SM AREA tag in the template for $areaName
     */
    function getAreaAttributes($areaName) {
        
        if (!isset($this->smTagList[$areaName])) {
            return NULL;
        }

        // return attributes
        $aList = $this->smTagList[$areaName]->attributes;
        return $aList;

    }


    /**
     * set the priority of an area. this affects when the objects in that
     * area are exectued. setting a priority higher will cause it to run
     * sooner in the template. values are from 0-n
     * @param string $areaName areaName to set
     * @param int $priority priority value, 0-n
     */
    function setAreaPriority($areaName, $priority) {

        // sanity
        if (!isset($this->smTagList[$areaName])) {
            return NULL;
        }

        // dig it out of the current priority list
        $found = false;
        foreach ($this->tagPriorityList as $p => $aL) {
            foreach ($aL as $id => $aName) {    
                if ($aName == $areaName) {
                    unset($this->tagPriorityList[$p][$id]);
                    $found = true;
                }
                if ($found)
                    break;
            }
            if ($found)
                break;
        }

        // set new priority
        $this->tagPriorityList[$priority][] = $areaName;

        return true;

    }

     /**
      * add a module to an area in the template
      * 
      * @param SM_module $module a reference to a module
      * @param string $areaName the arbitrary area name this module should appear in
      *
      */
    function addModule($module, $areaName) {
            
        // error checking
        if (!is_object($module))
            $this->fatalErrorPage("module added to area \"$areaName\" is not a valid SM_module!");
        
        // check for valid area name
        if (empty($this->smTagList[$areaName])) {
            $this->debugLog("area name not found, ignoring request to add ".get_class($module)." to $areaName");        
            return;
        }
        
        // add module to $areaMapping for valid area
        $this->smTagList[$areaName]->addItem($module);
    
    }
    

     /**
      * add a subtemplate to an area in the this template
      * 
      * @param SM_layoutTemplate $template a template object
      * @param string $areaName the arbitrary area name this template (and it's contents) should appear in
      */
    function addTemplate($template, $areaName) {
    
        // error checking
        if (!is_object($template))
            $this->fatalErrorPage("subtemplate added to area \"$areaName\" is not a valid SM_layoutTemplate!");
        
        // check for valid area name
        if (empty($this->smTagList[$areaName])) {
            $this->debugLog("area name not found, ignoring request to add sub-template to $areaName");      
            return;
        }
        
        // add template to $areaMapping for valid area
        $this->smTagList[$areaName]->addItem($template);
    
    }

    /**
     * add text (string) to an area in this template.
     * @param string $output the string to add
     * @param string $areaName the arbitrary area name this template (and it's contents) should appear in
     */
    function addText($output, $areaName) {
    
        // error checking
        if (!is_string($output))
            $this->debugLog("WARNING: data added to area \"$areaName\" is not a valid string!");
        
        // check for valid area name
        if (empty($this->smTagList[$areaName])) {
            $this->debugLog("area name not found, ignoring request to add string to $areaName");        
            return;
        }
        
        // add template to $areaMapping for valid area
        $this->smTagList[$areaName]->addItem($output);
    
    }

    /**
     * resets template to its initial, post-parsed state.  Also resets the tags to their initial states.
     */ 
    function resetTemplate () {

        // clear our output buffer
        // this leaves htmlParsedOutput in place for
        // re-running the tags
        $this->htmlOutput = NULL;
        $this->jsOutput = NULL;

        // step thru tag list and call tagReset on each
        foreach (array_keys($this->smTagList) as $areaName) {
            $this->smTagList[$areaName]->_tagReset();
        }

    }

    /**
     * reset on clone
     */
    function __clone() {
        foreach (array_keys($this->smTagList) as $areaName) {
            $this->smTagList[$areaName] = clone $this->smTagList[$areaName];
        }
        foreach ($this->subTemplateList as $stName => $tptH) {
            $this->subTemplateList[$stName] = clone $this->subTemplateList[$stName];
        }
        $this->resetTemplate();
    }

     /**
     * this method is run before running the tags tagThink method, and gives
     * the user a chance to add directives before running tagConfig
     */
     
    function _prepTags() {

        // for each tag
        foreach (array_keys($this->smTagList) as $areaName) {                       

            // setup directives, if available
            $tagType = strtoupper($this->smTagList[$areaName]->getAttr('TYPE'));
            if (isset($this->directive[$tagType]) && is_array($this->directive[$tagType]))
                $this->smTagList[$areaName]->configure($this->directive[$tagType]);                        

            // call tagConfig
            $this->smTagList[$areaName]->tagConfig();

        }

    }


     /**
     * this private method executes the tags tagThink method
     * and replaces the output into the template
     */
     
    function _runTags() {

        // this array will hold the tags that have delayed execution
        $delayTags = array();

        // set output to begin with post-parse output
        $this->htmlOutput = $this->htmlParsedOutput;

        // sort by priority (reverse sort, so highest is first)
        krsort($this->tagPriorityList, SORT_NUMERIC);

        // for each priority
        foreach ($this->tagPriorityList as $p => $pAreaList) {            

            // for each tag in this priority
            foreach ($pAreaList as $areaName) {

                // get tag type
                $tagType = strtoupper($this->smTagList[$areaName]->getAttr('TYPE'));

                // if validList is set, make sure this tag is in it
                if (sizeof($this->validTagTypeList) > 0) {
                    if (!in_array($tagType, $this->validTagTypeList)) {
                        // wasn't in the valid list
                        $this->debugLog("skipping tag in area $areaName, type $tagType: not in valid tag list");
                        continue;
                    }
                }

                // if excludeList is set, make sure this tag is in it
                if (sizeof($this->excludeTagTypeList) > 0) {
                    if (in_array($tagType, $this->excludeTagTypeList)) {
                        // was in the exclude list
                        $this->debugLog("skipping tag in area $areaName, type $tagType: in exclude tag list");
                        continue;
                    }
                }

                // delayed?
                if ($this->smTagList[$areaName]->delayThink) {                
                    $delayTags[] = $areaName;
                    continue;
                }

                // run the tag
                $this->smTagList[$areaName]->tagThink();

                // grab the tags javascript output
                if (!empty($this->sessionH)) {
                    $this->sessionH->jsOutput .= $this->smTagList[$areaName]->getTagJS();
                }

                // grab and sub the tags output
                $this->htmlOutput = str_replace($this->smTagList[$areaName]->tagLine, $this->smTagList[$areaName]->tagOutput, $this->htmlOutput);

            }

        }

        // now run the delayed tags
        foreach ($delayTags as $areaName) {            

            // run the tag
            $this->smTagList[$areaName]->tagThink();
            
            // grab the tags javascript output
            if (!empty($this->sessionH)) {
                $this->sessionH->jsOutput .= $this->smTagList[$areaName]->getTagJS();
            }

            // grab and sub the tags output
            $this->htmlOutput = str_replace($this->smTagList[$areaName]->tagLine, $this->smTagList[$areaName]->tagOutput, $this->htmlOutput);


        }

    }

    /**
     * run the template, ie run the modules inside this template and sub in 
     * their output to our HTML template
     *
     * @return string complete HTML output from this template
     */
    function run() {
        
        // prep the tags
        $this->_prepTags();

        // execute the tags
        $this->_runTags();

        // return the final template output
        return $this->htmlOutput;

    }


}

?>
