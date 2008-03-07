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

$intro  = '<B>SiteManager Test Suite</b><br><br>Click on the items in the left to test SiteManager functionality.<br><br>';
$intro .= 'If all tests do not pass, check your PHP version (Zend PHP 5+ or Roadsend PHP 2.9.3+) and include file setup. If you need more help, head over to the
           <a href="http://code.roadsend.com" target="_new">Roadsend Community Site</a>.';

if (defined('ROADSEND_PCC')) {
    $stats = re_memo_stats();
    $intro .= "<br><br>Roadsend AST Cache Stats:<br>Hits: {$stats['hits']}<br>Misses: {$stats['misses']}<br>Resets: {$stats['resets']}<br>";
}

// add intro
$layout1->addText($intro, 'main');

// finish display
$SM_siteManager->completePage();

?>