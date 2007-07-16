<?php
require_once("phpunit.php");

$suite = new TestSuite;

//smRoot tests
require_once('smRoot.php');
$suite->addTest(new testSuite("smRoot_test"));

//smSiteConfig tests
require_once('smSiteConfig.php');
$suite->addTest(new testSuite("smSiteConfig_test"));

?>