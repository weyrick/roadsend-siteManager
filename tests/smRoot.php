<?
$SM_siteName    = "SiteManager Test Suite";
$SM_siteID      = "TESTSUITE";
$adminDir		= "./";

require_once('siteManager.inc');

require_once('lib/smRoot.inc');

class smRoot_test extends TestCase {

	function smRoot_test ($name) {
		$this->TestCase($name);
    }


	function test_findSMfile_diskPass () {
		global $SM_siteManager;

		$smBase = '/usr/local/lib/php/siteManager/';
		//Test with path included, file exists
		$results = $SM_siteManager->_findSMfile_disk($smBase . 'contrib/modules/dbGui','modules','mod',false);
        $this->assert(($results !== false && !empty($results)), "with path included, file exists");

		//Test with path included, file doen't exist
		$results = $SM_siteManager->_findSMfile_disk($smBase . 'contrib/modules/nonExist','modules','mod',false);
        $this->assert(($results === false), "with path included, file doesn't exist");

		//Test w/o path, file exists, one path element
		$results = $SM_siteManager->_findSMfile_disk('dbGui','modules','mod',false);
        $this->assert(($results !== false && !empty($results)),"file exists, one path element");

		//Test w/o path, file doesn't exist, one path element
		$results = $SM_siteManager->_findSMfile_disk('nonExist','modules','mod',false);
        $this->assert(($results === false), "file doesn't exist, one path element");

		//load a local config, so there is an array of elements
		$SM_siteManager->loadSite('localConfig-test.xsm');

		//Test w/o path, file exists, more then one path element
		$results = $SM_siteManager->_findSMfile_disk('dbGui','modules','mod',false);
        $this->assert(($results !== false && !empty($results)), "file exists, multiple paths");

		//Test w/o path, file doesn't exist, more then one path element
		$results = $SM_siteManager->_findSMfile_disk('nonExist','modules','mod',false);
        $this->assert(($results === false), "file doesn't exist, multiple paths");

    }


}

?>