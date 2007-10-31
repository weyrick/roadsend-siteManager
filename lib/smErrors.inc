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

    global $SM_debugOutput, $SM_develState;

    // if not in develState, not debug log
    if (!$SM_develState) {
        return;
    }

    if (is_object($obj)) {
        $msg = get_class($obj).":: ".$msg;
    }

    $SM_debugOutput[] = array('msg'=>$msg,'verbosity'=>$verbosity);

}

/**
 * format output from debug_backtrace PHP function (php 4.3+)
 */
function SM_getDebugBackTrace($stripTags = false, $exception=NULL) {

    if (!function_exists('debug_backtrace'))
        return '(no backtrace available)';
    
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
        $file = dirname($val['file']).'/<font color="brown">'.basename($val['file']).'</font>';
        $text .= $file.' (<font color="darkred">'.$val['line'].'</font>): <font color="darkorange">'.$caller.'</font>'.$args."<br />\n";
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
    
    global $SM_outputBuffer, $SM_develState;

    // add backtrace, if available (php 4.3+)
    if ($SM_develState) {
        ($obj instanceof Exception) ? $e = $obj : $e = NULL;
        SM_debugLog(SM_getDebugBackTrace(false, $e));
    }

    // if msg is an array, dump it
    if (is_array($msg))
        var_dump($msg);

    SM_debugLog("<font color=\"red\">$msg</font>",$obj);

    $SM_outputBuffer = ob_get_contents();
    ob_end_clean();

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
    
    $output = "<html><body bgcolor=ffffff><br /><b><font color=\"red\">\n";
    $output .= "$SM_siteName ($SM_siteID) :: ".$_SERVER['PHP_SELF']."<br /></font><hr />\n";

    if (!empty($SM_noticeLog)) {

        if (isset($SM_siteManager) &&
            (!isset($SM_siteManager->siteConfig) ||
             (isset($SM_siteManager->siteConfig) && // this would only happen if it dies in config reader
              $SM_siteManager->siteConfig->getVar('debug','showNotices')))) {        

            $output .= "<b>NOTICE LOG:</b><br />\n";
            foreach ($SM_noticeLog as $line) {
                $output .= "$line<br />\n";
            }

        }
    }

    if (is_array($SM_debugOutput)) {
        foreach ($SM_debugOutput as $log) {
            if ($log['verbosity'] > $SM_debugLevel)
                continue;
            ($log['verbosity'] > 0) ? $v = '['.$log['verbosity'].']' : $v = '';
            $output .= "$v {$log['msg']}<br />\n";
        }
    }

    if (trim($SM_outputBuffer) != '') {
        $output .= "Output Buffer:<br /><pre>".$SM_outputBuffer."</pre>\n";
    }

    if (defined("ROADSEND_PHPC"))
        $verLine = "(".RAVEN_VERSION_TAG.'/'.PHP_OS.')';
    else
        $verLine = "(PHP v".PHP_VERSION.'/'.PHP_OS.')';
    
    $output .= "<hr /><b>Roadsend SiteManager v".SM_VERSION." (<a href=\"http://www.roadsend.com/siteManager\">http://www.roadsend.com/siteManager</a>) ".$verLine;
    $output .= "</b><br /></body></html>\n";

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

    global $SM_noticeLog;

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
        $msg = "<b>SiteManager PHP WARNING:</b> $errstr on line $errline in $errfile<br />\n";
        SM_debugLog($msg);
        break;
    case E_STRICT:
        $msg = "<b>STRICT:</b> $errstr on line $errline in $errfile<br />\n";
        SM_debugLog($msg);
        break;
    case E_NOTICE:
    case E_USER_NOTICE:
        $SM_noticeLog[] = '<b>SiteManager PHP NOTICE:</b> '.$errstr." on line $errline in $errfile\n";
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