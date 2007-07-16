<?php

/*

    Project: 
    using code from Roadsend PHP SiteManager (www.roadsend.com)
    
    description   :

    change history:
            
                xx/xx/xx - script created by xxxx
                    
*/

// include site configuration file, which includes siteManager libs
require('../admin/common.inc');

// create a new module. notice, returns a reference!!
$mod1 = $SM_siteManager->loadModule('basic');

// configure the module
$mod1->addDirective('testDirective','The module received a directive');

// create root template. notice, returns a reference!!
$layout1 = $SM_siteManager->rootTemplate("basicTemplate");

// add our module to area 'areaOne'
$layout1->addModule($mod1, "areaOne");

// finish display
$SM_siteManager->completePage();


?>