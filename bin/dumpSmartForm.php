#!/usr/local/bin/php
<?php

if ($argc < 5) {
    die("usage: {$argv[0]} <username> <password> <database> <table> [form/tpt]\n");
}

$SM_siteName    = "Form Dumper";
$SM_siteID      = "FORMDUMP";
require('siteManager.inc');
require('contrib/lib/smSaveForm.inc');

$dbSettings = array('DSN' => "mysql:host=localhost;dbname={$argv[3]}",
                    'userName' => $argv[1],
                    'passWord' => $argv[2]
                   );
$SM_siteManager->defineDBconnection('default',$dbSettings);

$dS = array('tableName' => $argv[4]);
$m = $SM_siteManager->loadModule('dbGui');
$m->configure($dS);
$m->run();

$form = $m->getResource('myForm');
if ($argv[5] == 'tpt') {
    echo join("\n",$form->template->htmlTemplate);
}
else {
    echo SM_saveForm($form);
}

?>