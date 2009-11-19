<?php

/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001-2009 Roadsend, Inc.(http://www.roadsend.com)
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
 *  override the PHP error handler
 */
set_error_handler("SM_errorHandler");

/**
 * submit a message to the debug log. it will only show up when viewing
 * debug output
 *
 * @param string $msg message to log
 * @param SM_errorHandler $obj the caller object. specify to display which object called
 *                                 the debug function
 * @param int $verbosity verbosity of this message, for filtering out unwanted debug messages later
 */
function SM_debugLog($msg, $obj=NULL, $verbosity=0) {

    global $SM_debugOutput, $SM_develState, $SM_filePrefixRemove, $SM_siteManager;

    // if not in develState, not debug log
    if (!$SM_develState) {
        return;
    }

    if (is_object($obj)) {
        $msg = '<span class="caller">'.get_class($obj)."::</span> ".$msg;
    }

    // add file/line
    $bt = debug_backtrace(false);
    $last = $bt[2];
    $msg .= ' on line '.$last['line'].' in '.$SM_siteManager->maybeRemovePrefix($last['file']);

    $SM_debugOutput[] = array('msg'=>$msg,'verbosity'=>$verbosity);

}

/**
 * format output from debug_backtrace PHP function (php 4.3+)
 */
function SM_getDebugBackTrace($stripTags = false, $exception=NULL) {

    global $SM_siteManager;

    $text = '<br /><font color="blue">';
    if (!empty($exception)) {
        $bt = $exception->getTrace();
    }
    else {
        $bt = debug_backtrace();
    }
    array_shift($bt); // get rid of SM_getDebugBackTrace()
    foreach ($bt as $val) {
        //$args = '('.join(',',$val['args']).')';
        $args = '(';
        foreach ($val['args'] as $a) {
            $aType = gettype($a);
            switch ($aType) {
            case 'string':
                if (strlen($a) > 20)
                    $args .= "<font color='darkgreen'>'".substr($a,0,20)."...'</font>,";
                else
                    $args .= "<font color='darkgreen'>'$a'</font>,";
                break;
            case 'array':
                $args .= '<font color="darkblue">[array/'.sizeof($a).']</font>,';
                break;
            case 'object':
                $args .= '<font color="darkblue">[object: '.get_class($a).']</font>,';
                break;
            case 'resource':
                $args .= '<font color="darkblue">[resource]</font>,';
                break;
            case 'boolean':
            case 'integer':
            case 'double':
            case 'NULL':
            default:
                $args .= '<font color="brown">'.$a.'</font>,';
                break;
            }
        }
        if (sizeof($val['args'])) {
            $args = substr($args, 0, -1);
        }
        $args .= ')';
        if (!empty($val['class']))
            $caller = $val['class'].'::'.$val['function'];
        else
            $caller = $val['function'];
        $fileM = $SM_siteManager->maybeRemovePrefix($val['file']);
        $file = dirname($fileM).'/<font color="brown">'.basename($fileM).'</font>';
        $text .= $file.' (<font color="darkred">'.$val['line'].'</font>): <font color="darkorange">'.$caller.'</font>'.$args."\n";
    }

    $text .= "</font>";

    if ((PHP_SAPI == 'cli') || ($stripTags))
        $text = strip_tags($text);
        
    return $text;


}

/** 
 *  a fatal error has occured. last ditch effort to output an error message.
 *
 *  NOTE that this function checks to make sure it is not already defined. this allows
 *  you to override SM_fatalErrorPage() by defining it before the call to include SiteManager.
 *
 *  @param string $msg error message to display 
 *  @param object $obj the caller object. specify to display which object called the error routine
 *  
 */
if (!function_exists('SM_fatalErrorPage')) {
function SM_fatalErrorPage($msg='', $obj=NULL) {
    
    global $SM_outputBuffer, $SM_develState, $SM_shortFatalError, $SM_debugOutput, $SM_noticeLog;

    // SM_shortError means we don't display any notice or previous debugLog
    // we just show the current error and debug trace
    if ($SM_shortFatalError) {
        $dcnt = sizeof($SM_debugOutput);
        $SM_debugOutput = array();
        $SM_noticeLog = array();
        SM_debugLog("** NOTE: SM_shortFatalError is ON: turn off to see $dcnt more lines of debug output");
    }

    // add backtrace, if available (php 4.3+)
    if ($SM_develState) {
        ($obj instanceof Exception) ? $e = $obj : $e = NULL;
        SM_debugLog(SM_getDebugBackTrace(false, $e));
    }

    // if msg is an array, dump it
    if (is_array($msg))
        var_dump($msg);

    SM_debugLog("<font color=\"red\">$msg</font>",$obj);

    if (ob_get_level() > 0) {
        $SM_outputBuffer = ob_get_contents();
        ob_end_clean();
    }
    
    echo SM_makeErrorPage();
    exit;
    
}
}

/**
 * create the HTML for the error page
 * doesn't echo anything, it returns it's output
 * @return string complete HTML page output
 */
function SM_makeErrorPage() {

    global $SM_siteName, $SM_siteID, $PHP_SELF, $SM_debugOutput, $SM_outputBuffer, $SM_noticeLog, $SM_siteManager, $SM_debugLevel;
    
    if ($SM_siteName == '')
        $SM_siteName = 'UNKNOWN SITENAME';
    
    if ($SM_siteID == '')
        $SM_siteID = 'UNKNOWN SITEID';
    
    $output = "<html><head>
               <style type=\"text/css\">
               .sm_error_wrap {
                 color: black;
                 background-color: white;
                 font-family: monospace;
                 font-size: 9pt;
               }
               h2 {
                 color: red;
               }
               .caller {
                 font-weight: bold;
               }
               .row0 {
                 background-color: #eeeeee;
                 white-space: pre;
               }
               .row1 {
                 background-color: white;
                 white-space: pre;
               }
               #dbBenchmark {
                 display: inline;
               }
               .sm_sqlstats {
                 background-color: #eeeeee;
                 white-space: pre;
               }
               .sm_sqlbt {
                 background-color: white;
                 white-space: pre;
               }
               .sm_sql {
                 background-color: white;
                 white-space: pre;
               }
               .sm_querybox {
                 border: solid 1px;
                 margin: 2px;
               }
               </style>
               </head><body>
               <div class='sm_error_wrap'>
               <br /><h2>\n";
    $output .= "$SM_siteName ($SM_siteID) :: ".$_SERVER['REQUEST_URI']."</h2><hr />\n";
    
    /*
    if (!$SM_siteManager->dbH->benchMarking) {
        $chr = (strstr($_SERVER['REQUEST_URI'],'?')) ? '&' : '?';
        $output .= '<a href="'.$_SERVER['REQUEST_URI'].$chr.'SM_dbBenchmark=1">Reload With DB Benchmarking</a><br>';
    }
    */

    if (!empty($SM_noticeLog)) {

        if (isset($SM_siteManager) &&
            (!isset($SM_siteManager->siteConfig) ||
             (isset($SM_siteManager->siteConfig) && // this would only happen if it dies in config reader
              $SM_siteManager->siteConfig->getVar('debug','showNotices')))) {        

            $output .= "<b>NOTICE LOG:</b><br />\n";
            foreach ($SM_noticeLog as $line => $_d) {
                $output .= "$line<br />\n";
            }

        }
    }

    $skipped = 0;
    if (is_array($SM_debugOutput)) {
        foreach ($SM_debugOutput as $log) {
            if ($log['verbosity'] > $SM_debugLevel) {
                $skipped++;
                continue;
            }
            ($log['verbosity'] > 0) ? $v = '['.$log['verbosity'].']' : $v = '';
            $rNum = ($rNum) ? 0 : 1;
            $dbLogOutput .= "<div class='row{$rNum}'>$v {$log['msg']}</div>\n";
        }
    }

    $output .= "<h3>Showing Debug Level: ".$SM_debugLevel." ({$skipped} msgs hidden)</h3>";
    $output .= $dbLogOutput;

    if (trim($SM_outputBuffer) != '') {
        $output .= "<h3>Output Buffer:</h3><div class='row0'>".$SM_outputBuffer."</div>\n";
    }

    /*
    if ($SM_siteManager->dbH->benchMarking) {
        $output .= '<h3>DB Benchmarks:</h3><span id="dbBenchmark">'.$SM_siteManager->dbH->getProfileReport().'</span>';
    }
    */

    if (defined("ROADSEND_PHPC"))
        $verLine = "(".RAVEN_VERSION_TAG.'/'.PHP_OS.')';
    else
        $verLine = "(PHP v".PHP_VERSION.'/'.PHP_OS.')';
    
    if (PHP_SAPI != 'cli')
        $output .= "<hr /><b>Roadsend SiteManager v".SM_VERSION." (<a href=\"http://www.roadsend.com/siteManager\">http://www.roadsend.com/siteManager</a>) ".$verLine;
    $output .= "</b><br />";

    $output .= "</div></body></html>\n";

    if (PHP_SAPI == 'cli')
        $output = strip_tags($output);

    return $output;

}

/**
 * this debug function will dump all variables that were
 * passed to the script, including GET, POST and cookies
 * will traverse arrays
 */
function SM_dumpVars() {
    
    while (list($key, $val) = each ($_REQUEST)) {
        if (!is_array($val))
            echo "$key => $val<br />";
        else {
            echo "$key (array):<br />";
            foreach($val as $k => $v) {
                echo " ..... $k -> $v<br />";
            }
        }
    }

}   


/**
*  override PHP's main error handler function
*  this should NOT be called directly
*/
 
function SM_errorHandler($errno, $errstr, $errfile, $errline) {

    global $SM_noticeLog, $SM_develState, $SM_siteManager;

    $errfile = $SM_siteManager->maybeRemovePrefix($errfile);

    // if not in devel mode we ignore
    if (!$SM_develState)
        return;

    switch ($errno) {
    case E_PARSE:
    case E_USER_ERROR:
    case E_COMPILE_ERROR:
    case E_CORE_ERROR:
    case E_ERROR:
        $msg = "<b>FATAL</b> [$errno] $errstr<br />\n";
        $msg .= "  Fatal error in line $errline of file ".$errfile;
        $msg .= ", SiteManager ".SM_VERSION.", PHP ".PHP_VERSION." (".PHP_OS.")<br />\n";
        SM_fatalErrorPage($msg);
        break;
    case E_USER_WARNING:
    case E_COMPILE_WARNING:
    case E_CORE_WARNING:
    case E_WARNING:
    case E_RECOVERABLE_ERROR:
        $msg = "<b>PHP WARNING:</b> $errstr on line $errline in $errfile<br />\n";
        SM_debugLog($msg);
        break;
    case E_STRICT:
        $msg = "<b>PHP STRICT:</b> $errstr on line $errline in $errfile<br />\n";
        $SM_noticeLog[$msg] = true;
        break;
    case E_NOTICE:
    case E_USER_NOTICE:
        // only unique notices
        $msg = '<b>PHP NOTICE:</b> '.$errstr." on line $errline in $errfile\n";
        $SM_noticeLog[$msg] = true; 
        break;
    default:
        SM_fatalErrorPage("Unkown error type: [$errno] $errstr on line $errline in $errfile<br />\n");
        break;
    }

}


/**
 * default exception handler
 *
 * @param Exception $exception the uncaught exception
 */
function sm_default_exception_handler($exception) {
    if ($exception instanceof Exception) {
        SM_fatalErrorPage($exception->getMessage(), $exception);
    }
    else {
        SM_fatalErrorPage('Unknown exception: '.$exception);
    }
}

// use the default SiteManager exception handler
set_exception_handler('sm_default_exception_handler');

?>