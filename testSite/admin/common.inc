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
            
                05/15/01 - script created by weyrick
                    
*/

// site name and ID. used for debug output and configuration
$SM_siteName    = "SiteManager Test Site";
$SM_siteID      = "TESTSITE";

// Roadsend SiteManager Functionality
require("siteManager.inc");

// set base admin directory
$adminDir = "/var/www/testSite/admin/";

// this is a global defined to test teh GCS
$testGlobal = 'FU';

// load in site settings through SM_siteManager
$SM_siteManager->loadSite($adminDir.'config/localConfig.xsm');

// connect to database, using settings from localConfig.xsm
$SM_siteManager->dbConnect();

if ($SM_develState) {
    if ((isset($_GET['SM_queryTrace']))&&($_GET['SM_queryTrace'] == 1)) {
        $SM_siteManager->dbH->startQueryTrace();
    }
}

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
    $SM_siteManager->addPersistent('SM_queryTrace');
    if ((isset($_GET['SM_queryTrace']))&&($_GET['SM_queryTrace'] == 1)) {
        $SM_siteManager->sessionH->setPersistent('SM_debug',1);
    }
}


// hook for grabbing extra member information
// this is run when the member system loads the member information from the main table
// (or from a persistent variable)
function testMemberLoad(&$memberData) {

    // alter the main users memberData for other modules and code to see
    $memberData['testMemberLoad'] = 'ran testMemberLoad';

    // could be looking up more info in a database, or whatever else...
    // to access the SiteManager database settings, use

    // $SM_siteManager->dbH

    // after declaring $SM_siteManager as a global

}


?>