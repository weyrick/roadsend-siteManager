<?php

/*********************************************************************
*  Roadsend SiteManager
*  Copyright (c) 2001-2009 Roadsend, Inc.(http://www.roadsend.com)
**********************************************************************
*
* This source file is subject to version 1.0 of the Roadsend Public
* License, that is bundled with this package in the file 
* LICENSE, and is available through the world wide web at 
* http://www.roadsend.com/license/rpl1.txt
*
**********************************************************************
* Author(s): Shannon Weyrick (weyrick@roadsend.com)
*
*/

// -----------------------------------------------------------------------------

// if SiteManager is being used on a system where you
// are unable to place the main SiteManager root directory
// into the php.ini include_path, set the following variable
// to the main root siteManager directory
// for example:

// $SM_rootDir = '/var/www/user/siteManager/';

$SM_rootDir = '';   // when set, make sure it ends with a slash!

// -----------------------------------------------------------------------------
// don't touch below here
// -----------------------------------------------------------------------------

// Version
$SM_versionTag = "6.0.0";
define('SM_VERSION', '6.0.0');

// main global config file
define('SM_GLOBAL_CONFIG_FILE','config/globalConfig.xsm');
define('SM_SMCONFIG_READER_FILE','lib/configReaders/SMCONFIG_reader.inc');

// turn on all warnings and notices
error_reporting(E_ALL);

// start out with debugLevel on high, it will be altered by GCS
// this will catch errors on startup
$SM_develState = true;
$SM_debugLevel = 10;

// catch all output
ob_start();

// script start time
$microtime = explode(' ', microtime());
$SM_scriptStartTime = $microtime[1].substr( $microtime[0], 1);

require($SM_rootDir."lib/smLoader.inc");
require($SM_rootDir."lib/smComponents.inc");

require($SM_rootDir."lib/smErrors.inc");
require($SM_rootDir."lib/smSupport.inc");
require($SM_rootDir."lib/smObject.inc");
require($SM_rootDir."lib/smErrorHandler.inc");
require($SM_rootDir."lib/smConfigReader.inc");
require($SM_rootDir."lib/smDbWrapper.inc");

require($SM_rootDir."lib/smConfig.inc");
require($SM_rootDir."lib/smSiteConfig.inc");
require($SM_rootDir."lib/smInVarManager.inc");
require($SM_rootDir."lib/smRoot.inc");
require($SM_rootDir."lib/smLayoutTemplate.inc");
require($SM_rootDir."lib/smModules.inc");
require($SM_rootDir."lib/smSessionContainer.inc");
require($SM_rootDir."lib/smSessions.inc");

require($SM_rootDir."lib/dataManip.inc");
require($SM_rootDir."lib/auth.inc");


// ROOT SITEMANAGER CLASS
$SM_siteManager = new SM_siteManagerRoot();
$SM_siteManager->boot();

if ($SM_develState) {
    SM_debugLog("Initializing SiteManager...");
}


?>