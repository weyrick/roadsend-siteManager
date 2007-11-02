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
 * a class for handling runtime errors
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 */
class SM_errorHandler extends SM_object {

    /**
     * constructor
     */
     
     
    function SM_errorHandler() {
        // SM_object configure
        $this->_smoConfigure();
        // show load notice
        SM_debugLog("error handler loaded",$this,1);
    }

    /**
    * invoke a fatal error page
    * the base method simply calls SM_fatalErrorPage
    */
     
    function fatalErrorPage($msg, $obj=NULL) {
        SM_fatalErrorPage($msg, $obj);
    }

    /**
     * add a message to the debug log
     * the base method simply calls SM_debugLog
     * @param string $msg message to log
     * @param SM_errorHandler $obj object calling the error handler
     * @param int $verbosity level of verbosity this message is assigned to (higher=more verbose)
     */
    function debugLog($msg, $obj=NULL, $verbosity=0) { 
        SM_debugLog($msg, $obj, $verbosity);
    }

    /**
     * check for a PEAR database error
     * @param result handle $rh the PEAR db result handle to check
     * @param SM_errorHandler $obj calling object
     * @param bool $fatal if true, and $rh is a PEAR error, we issue a fatalErrorPage()
     */
    function dbErrorCheck($rh, $obj, $fatal=true) {

        global $SM_develState;

        if (empty($rh)) {

            $msg = '';
            $lastid = $GLOBALS['SM_LAST_DB_ID'];
            if (empty($lastid))
                $lastid = 'default';
            
            if ($SM_develState && isset($this->dbHL[$lastid])) {
                $edata = $this->dbHL[$lastid]->errorInfo();
                $msg .= $edata[2];
                $msg .= ' <br>Query Was: ['.$this->dbHL[$lastid]->lastQuery.']';
            }

            if ($fatal) {
                $this->fatalErrorPage($msg,$obj);
            }
            else {
                // log error
                $this->debugLog($msg,$obj);
                // return
                return true;
            }

        }

        return false;

    }

}


?>