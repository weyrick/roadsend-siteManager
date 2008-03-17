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
 * base class for all SM tag handlers
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_template 
 */
class SM_smTag extends SM_object {

    /**
    * the actual <SM> XML tag as it appears in the template 
    * @var string
    */
    var $tagLine = '';

    /** 
    * the attributes associated with this tag, in key/value hash 
    * @var array
    */
    var $attributes = array();

    /**
    * the data associated with this tag. only found in extended tags
    * like this: <SM TYPE="foo">this is the data</SM>
    * @var string
    */
    var $data = '';

    /** 
    * the javascript output associated with this tag. it will show up in the main javascript section 
    * @var string
    */
    var $jsOutput = '';

    /** 
    * the main output this tag will replace itself with in the final template output 
    * @var string
    */
    var $tagOutput = '';

    /** 
    * setting this to true will cause this tag to be run after all others which are not delayed 
    * @var bool
    */
    var $delayThink = false;

    /** 
    * a value representing the 'priority' in which this tag should be exectuted 
    * @var int
    */
    var $thinkPriority = 0;

    /**
     * the template object that owns this tag
     * @var SM_layoutTemplate
     */
    var $parentTemplate = NULL;

    /**
     * configure base object
     */
    function SM_smTag() {

        $this->_smoConfigure();

    }

    /**
     * set parent template
     */
    function setParent($template) {

        $this->parentTemplate = $template;

    }


    /**
     * a method for configuring the tag upon it's creation. it is called for
     * every tag
     * @param string $tagLine the <SM> XML tag string, as it apprears in the template
     * @param array $attrs a list of key/value pairs that represent the tag attributes
     */
    function setTagData($tagLine, $attrs, $data='') {

        // configure 
        $this->_smoConfigure();
        $this->tagLine = $tagLine;
        $this->attributes = $attrs;
        $this->data = $data;

        // set priority, if there
        if (isset($this->attributes['PRIORITY'])) {
            $this->setPriority($this->attributes['PRIORITY']);
        }
        else {
            $this->setPriority();
        }

    }

    /**
     * set execution priority
     * @param int $priVal priority, 0-n, 0 being the lowest priority
     */
    function setPriority($priVal = 0) {
        $this->thinkPriority = $priVal;
    }

    /**
     * wrapper function for output
     * @param string $output string to add to main tag output
     */
    function say($output) {
        $this->tagOutput .= $output;
    }


    /**
     * wrapper function for javascript output
     * @param string $output string to add to main tag output
     */
    function sayJS($output) {
        $this->jsOutput .= $output;
    }

    /**
     * method for retrieving an attribute from this tag
     * @param string $key the attribute key to retrieve
     * @return string the value of the attribute requested, or NULL
     */
    function getAttr($key) {
        if (isset($this->attributes[$key])) {        
            return $this->attributes[$key];
        }
        else {        
            $this->debugLog("getAttr: requested attribute not found for key [$key], tag: ".$this->attributes['TYPE'], 6);
            return NULL;
        }
    }

    /**
     * the main "think" method for the tag. it should run whatever processing
     * code is necesary to return the final tag output. this should be overridden
     */
    function tagThink() {
        // virtual
    }

    /**
     * the main "config" method for the tag. 
     * it has access to directives set through the template by using a key of
     * the tags name. for example, setting $template->addDirective('HTMLTITLE',array('htmlTitle','defaultTitle'))
     * will give access to $this->directive['htmlTitle'] in tagConfig() and tagThink()
     */
    function tagConfig() {
        // virtual
    }

    /**
     * called by templates to reset the tag to its initial state. this should be overridden
     */ 
    function _tagReset() {

        // derived classes should call me with parent::tagReset();
        $this->tagOutput = NULL;
        $this->jsOutput = NULL;

        if (method_exists($this, "tagReset")) {
            $this->tagReset();
        }

    }

    /**
     * return the current javascript output
     * @return string javascript output that should appear in the main javascript section
     */
    function getTagJS() {
        return $this->jsOutput;
    }

}


/**
 * load (include) a template tag handler
 * @param string $iName the input entity to load
 * @returns bool if false, couldn't find it. true means successful load
 */
function SM_loadTag($tagName) {
    
    // if already here, don't reload    
    if (class_exists('SM_smTag_'.$tagName))
        return true;

    // globalize
    global $SM_siteManager;

    // find file
    $tagName = $SM_siteManager->findSMfile($tagName.'_tag', 'smTags', 'inc', false);

    // if it wasn't found, return an error
    if (empty($tagName))
        return false;

    // load it up
    // include_once will not let duplicate modules be loaded
    include_once($tagName);   
    
    // all good and loaded, return
    return true;

}

?>
