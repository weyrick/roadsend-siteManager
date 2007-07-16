<?php

/*

    Project: 
    using code from Roadsend PHP SiteManager (www.roadsend.com)
    
    description   : directive script

    change history:
            
                    
*/

// include site configuration file, which includes siteManager libs
require('../admin/common.inc');

// create root template. notice, returns a reference!!
$layout1 = $SM_siteManager->rootTemplate("main.cpt");

$layout1->configureTag('HTMLTITLE',array('htmlTitle'=>'Template System'));

// load requested module
$mod = $SM_siteManager->loadModule('test_templates');

// add the module to the codePlate
$layout1->addModule($mod, 'main');

// finish display
$SM_siteManager->completePage();

?>