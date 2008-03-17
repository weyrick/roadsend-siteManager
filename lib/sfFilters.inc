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
 * @package roadsend_siteManager
 */

/**
 * filter base class            
 * all filters should extend this class 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class SM_entityFilter extends SM_object {

    /**
    * the data we'll be looking at 
    * @var string  
    */
    var $varName = '';  

    /** 
    * the current contents of $varName
    * @var mixed  
    */
    var $data = NULL;  

    /** 
    * does this filter REQUIRE javascript to work?
    * @var bool  
    */
    var $requiresJS = false; 

    /** 
     * javascript output. this is the stuff that needs to
     * go at the top, not the inline stuff
     * @var string
     */
    var $jsOutput = '';

    /** 
    * use javascript or not
    * @var bool
    */
    var $useJS = false;
                        
    /** 
    * my parent input entity
    * @var SM_Object
    */
    var $parentInputEntity = NULL;

    /**  
     * constructor setups up class. this should NOT be overriden!
     * use filterConfig() for class initialization.     
     * @param SM_Object $pE  parent input entity of this filter
     */
    function SM_entityFilter($pE) {
            
        // setup handlers
        $this->_smoConfigure();
        
        // parent input entity
        $this->parentInputEntity    = $pE;
        
        // default bad message
        $this->addDirective('badMessage','');

    }
        
    /**
     * set info on this filter
     * @param string $vN  variable name that contains data we'll be looking at
     * @param string $msg string to display if filter fails
     * @param bool $uJS  use JavaScript in this filter or not
     */
    function setInfo($vN, $msg='', $uJS) {
        
        // use javascript?
        $this->useJS = $uJS;
        
        // if we're using javascript, load it up
        if ($uJS)
            $this->loadJS();
            
        // if the filter fails, this message will be displayed to the user
        if ($msg != '')
            $this->directive['badMessage'] = $msg;
        else
            $this->directive['badMessage'] = "This field appears to be invalid";  /* default */
    
        // the data the filter will operate on
        $this->varName = $vN;

        // set data based on $varName
        // Added check to $_FILES (gollum 07/22/2004)
        if (isset($_POST[$this->varName]))
            $this->data = $_POST[$this->varName];
        elseif (isset($_FILES[$this->varName]))
            $this->data = $_FILES[$this->varName]['name'];
        else
            $this->data = '';

        // run filterConfig method if it exists
        if (method_exists($this, 'filterConfig')) {
            $this->filterConfig();
        }
    
    }

    /**
     * apply this filter. if it fails, return the false
     *
     * this should be overridden by filters when they subclass this class
     * @return bool whether filter passed or not
     */
    function filterThink() { return true; }
   
    /**
     * return javscript code we might have
     * check global list to make sure we haven't already been added to this script's js code
     * @return string JavaScript output, if any
     */
    function getJS() {
        
        global $filterDefList;

        $cName = get_class($this);
        
        if (isset($filterDefList[$cName]))
            return;
        else {
            $filterDefList[$cName] = true;
            return $this->jsOutput;
        }
        
    }
    
    /**
     * attempt load javascript code for this filter. do this my trying to load
     * our filter name +.js in the php path. use it as javscript output
     * FIXME: security hazards??
     */
    function loadJS() {
        
        $fName = get_class($this).'.js';
        $fName = $this->siteConfig->getVar('dirs','sfFilters').'/'.str_replace("filter","Filter",$fName); // capitlize Filter, as the class won't for us
    
        // load javascript, check include path.
        // ignore errors
        if (file_exists($fName))
            $jsO = file($fName,1);
        
        if ((isset($jsO))&&(is_array($jsO))) {
            $this->jsOutput = implode('',$jsO);
        }
    
    }
    
}



/**
 * load (include) a filter module
 * if it's already loaded, ignore request
 * @param string $fName the input entity to load
 * @return bool if false, couldn't find it. true means successful load
 */
function SM_sfLoadFilter($fName) {
    
    // if already here, don't reload    
    if (class_exists($fName.'Filter'))
        return true;    
    
    // globalize
    global $SM_siteManager;

    // find file
    $fName = $SM_siteManager->findSMfile($fName.'Filter', 'sfFilters', 'inc', true);

    // load it up
    // include_once will not let duplicate modules be loaded
    include_once($fName);   
    
    // all good and loaded, return
    return true;

}

?>