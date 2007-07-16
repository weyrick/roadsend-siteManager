<?php

/*

    SITE CONFIG FILE

    Project: 
    using code from RoadsEnd PHP SiteManager
    
    -------------------------------------------------------------------

    SECURITY NOTICE: This file should be secured against direct requests.
    The SiteManager system suggests blocking all direct access to files
    with the extension .inc
    
    In apache, this is done by adding the following lines to httpd.conf:
    
    # dont allow access to SiteManager files
    <FilesMatch "\.(inc|tpt|stp|mod|xsm|cpt)$">
        Order allow,deny
        Deny from all
    </FilesMatch>
      
    --------------------------------------------------------------------

    description   : site include file. sets up information for this site.

    change history:
            
                xx/xx/xx - script created by xxx
                    
*/

// site name and ID. used for debug output and configuration
$SM_siteName    = "Project Name";
$SM_siteID      = "PROJ1";

// Roadsend SiteManager Functionality
require("siteManager.inc");

// set base admin directory
if(substr(PHP_OS, 0, 3) == "WIN") {
    // windows
    $adminDir = "c:/project/admin/";
}
else {
    // unix
    $adminDir = "/var/www/html/project/admin/";
}

// load in site settings through SM_siteManager
$SM_siteManager->loadSite($adminDir.'config/localConfig.xsm');

// connect to database, using settings from localConfig.xsm
//$SM_siteManager->dbConnect();

// use sessions
$SM_siteManager->startSessions();

// setup debug session?
if ($SM_develState) {
    $SM_siteManager->addPersistent('SM_debug');
    if ((isset($_GET['SM_debug']))&&($_GET['SM_debug'] == 1)) {
        $SM_siteManager->sessionH->setPersistent('SM_debug',1);
    }
    else if (isset($_GET['SM_debug'])) {
        $SM_siteManager->sessionH->setPersistent('SM_debug',0);
    }
}

// add a variable to keep persistent
//$SM_siteManager->addPersistent("varName");

// authorization list for SM_basicAuth (auth.inc)
//$authList   = array(
//    "username1" => "password1",
//    "username2" => "password2"
//);

// other included libraries
//require($adminDir."functions.inc");

?>