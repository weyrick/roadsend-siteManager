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
 * global function to reload the current page and keep session variables in tact
 * @param $list (hash) an optional list of key/value pairs to add to the URL
 * @return this function never returns
 */
function SM_reloadPage($list=NULL) {

    global $SM_siteManager;

    // Make sure add is blank (just in case)
    $add = '';

    // Loop thru all the values passsed to us
    if (isset($list) && is_array($list)) {
        foreach($list as $key => $val) {
            $add .= "$key=" . urlencode($val) . '&';
        }
    }

    $SM_siteManager->sessionH->saveSession();
    header("Location: {$SM_siteManager->sessionH->PHP_SELF}?$add".$SM_siteManager->sessionH->getSessionVars());
    exit;

}

/**
 * check the object passed to see if it's a PEAR database error.
 * if it is, call a fatal error page outputting relavent error
 * messages
 * @param PEAR_DB $rh the PEAR DB result handle to check for an error
 * @param mixed $obj the object who called the error
 * @param bool $fatal whether this is a fatal error or not
 */
function SM_dbErrorCheck($rh, $obj=NULL, $fatal=true) {

    global $SM_siteManager, $SM_develState;

    if (isset($SM_siteManager)) {
        return $SM_siteManager->errorHandler->dbErrorCheck($rh, $obj,$fatal);
    }

    if (empty($rh)) {

        $msg = "Database Query Error";

        $lastid = $GLOBALS['SM_LAST_DB_ID'];
        if (empty($lastid))
            $lastid = 'default';
            
        if ($SM_develState && isset($this->dbHL[$lastid])) {
            $edata = $this->dbHL[$lastid]->errorInfo();
            $msg .= $edata[2];
            $msg .= ' <br>Query Was: ['.$this->dbHL[$lastid]->lastQuery.']';
        }

        if ($fatal) {
            SM_fatalErrorPage($msg,$obj);
        }
        else {
            // log error
            SM_debugLog($msg,$obj);
            // return
            return true;
        }

    }

    return false;

}

/**
 * get the value of a variable
 * @param string $varName the variable name to retrieve 
 */
function SM_getVar($varName) {
    global $SM_siteManager;
    return $SM_siteManager->inVarH->getVar($varName);
}


/**
 * return a random integer between 0 and $max 
 * @param int $max maximum random number to return
 * @return int a random number
 */
function SM_randomint($max) {
    static $startseed = 0; 
    if (!$startseed) {
        $startseed = (double)microtime()*getrandmax(); 
        srand($startseed);    
    }
    return(rand()%$max); 
}

/**
 * take a seperated list of items, and return true if $findItem is
 * one of the elements.
 * @param string $findItem the item to find in the list (needle)
 * @param string $cdList the delimited list to search (haystack)
 * @param string $delim the delimiter to use. default is comma
 * @return bool true if needle was found in haystack, else false
 */
function SM_isInSet($findItem, $cdList, $delim=',') {

    $lArray = split($delim, $cdList);
    return in_array($findItem, $lArray);
}

/**
 * return a list suitable for passing as a GET variable list, of the variables in the current script
 * 
 * when using the excludeList, the key for each item of the hash is what's important. it shoud be
 * set to the variable name you wish to have excluded from the list of variables returned. the value
 * for that item in the hash is not important, as long as it's not blank. see the example.
 *
 * example usage:
 *  // create a suitable GET variable list, but don't include sessionID or dontInclude
 *  $transferURL = SM_getScriptVars(array('sessionID'=>'1','dontInclude'=>'1'));
 *  header("Location: $PHP_SELF?$transferURL");
 *  exit;
 *
 * @param array $excludeList a hash of variable that should NOT be included in the lsit, 
 *                      should they be found. key should be the variable name, value should be non-false
 * @return string a list of variables suitable for passing as a GET
 */
function SM_getScriptVars($excludeList=NULL) {

    // gather vars
    $varList = '';
    while (list($key, $val) = each($_REQUEST)) {
        if (is_array($excludeList)) {
            if ((!isset($excludeList[$key]))||($excludeList[$key] == ''))
                $varList .= "&$key=".urlencode($val);
        }
        else {
                $varList .= "&$key=".urlencode($val);
        }

    }

    $varList = substr($varList, 1);
    return $varList;        

}


/**
 * reload (via Location header) the current script with all variables in tact,
 * minus a hash of variables not to include and plus a hash of variabless to be included
 * 
 * when using the excludeList and includeList, the key for each item of the hash is what's important. it shoud be
 * set to the variable name you wish to have excluded/included from the list of variables used. the value
 * for that item in the hash is not important, as long as it's not blank.
 *
 * @param array $exludeList a hash of variables to be excluded from the reload
 * @param array $includeList a hash of variables to be included into the reload
 * @see SM_getScriptVars
 */
function SM_reloadScript($excludeList=NULL,$includeList=NULL) {

    global $PHP_SELF;
    
    $varList = SM_getScriptVars($excludeList);

    // new vars to add
    if (is_array($includeList)) {
        
        while (list($key, $val) = each($includeList)) {
            $varList .= "&$key=".urlencode($val);
        }
    
    }

    $newLoc = $PHP_SELF.'?'.$varList;

    header("Location: $newLoc");
    exit();

 }


/**
 * return a "pretty name". ie, split on studly caps
 * this will take a string such as "thisIsAStringTest1" and
 * return "This Is A String Test 1"
 *
 * @param string $name the string to convert
 * @return string the newly formatted string
 */
function SM_prettyName($name) {

    // firstName = First Name
    if (preg_match("/[a-z][A-Z]/",$name)) {
       $name = preg_replace("/([a-z])([A-Z])/","\\1%\\2",$name);
       $wordList = explode('%',$name);
       $name = '';
       foreach ($wordList as $word) {
            $name .= ucfirst($word).' ';
       }
       chop($name);
    }
    // FirstName = First Name
    elseif (preg_match("/[A-Z][a-z]/",$name)) {     
       $name = preg_replace("/([A-Z])([a-z])/","\\1%\\2",$name);
       
       $wordList = explode('%',$name);
       $name = '';
       foreach ($wordList as $word) {
            $name .= ucfirst($word).' ';
       }
    }
    // address1 = Address 1
    // address2 = Address 2
    elseif (preg_match("/^(\w+)(\d)\$/",$name,$m)) {
        $name = ucfirst($m[1]).' '.$m[2];
    }
    // sometext = Sometext
    else {
       $name = ucfirst($name);
    }
    
    // strip a trailing _idxNum
    if (preg_match("/^(.+)_idx\s*Num\s*\$/",$name,$m)) {
       $name = $m[1];
    }           
    
    return trim($name);

}

/** 
* debug function used in array_walk (smObject.inc)
*/
 
function _ddPrint($val, $key) {

    global $_dOp;

    if (is_array($val)) {
        $_dOp .= "$key<ul><br />\n";
        array_walk($val, '_ddPrint');
        $_dOp .= "</ul>";
        return;
    }

    $tVal = $val;

    // cleanup
    if (isset($val) && $val == '') { $tVal = '[blank string]'; }
    if ($val === false ) { $tVal = 'false'; }
    if ($val === true ) { $tVal = 'true'; }
    if ($val === NULL) { $tVal = '[NULL]'; }
    
    if (is_object($val)) {
        $classVars = get_class_vars(get_class($val));
        $obj = $val;
        $tVal = "<ul><br />\n";
        foreach ($classVars as $n => $v) {        
            $v = $obj->$n;
            if ($v === false ) { $v = 'false'; }
            if ($v === true ) { $v = 'true'; }
            if ($v === NULL) { $v = '[NULL]'; }
            if (isset($v) && $v == '') { $v = '[blank string]'; }
            $tVal .= "$n -> $v<br />\n";
        }
        $tVal .= "</ul>";
    }

    $_dOp .= "$key => $tVal<br />\n";

}

?>
