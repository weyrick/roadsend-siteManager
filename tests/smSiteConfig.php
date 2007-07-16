<?

$SM_siteName    = "SiteManager Test Suite";
$SM_siteID      = "TESTSUITE";
$adminDir		= "./";

require_once('siteManager.inc');
require_once('lib/smSiteConfig.inc');

class smSiteConfig_test extends TestCase {

	function smSiteConfig_test ($name) {
		 $this->TestCase($name);
    }

	function testGetVarPass () {
        global $SM_siteManager;
        
        $siteConfig = $SM_siteManager->siteConfig;
        //make sure localConfig is empty
        unset($siteConfig->localConfig);
        //$siteConfig->getVar($section, $name, $mergeGlobal=false, $sectionID='', $attr='VALUE')
        
        //no localconfig, return global
        $results = $siteConfig->getVar('dirs', 'config');
        $this->assert(!empty($results), "no local config");
        
        //load a local config
        $SM_siteManager->loadSite('localConfig-test2.xsm');

        //mergeGlobal is set and we have a local

        //  - global is an array, local is array
        $results = $siteConfig->getVar('dirs', 'libs', true);
        $this->assert((!empty($results)&& sizeof($results) == 4),"global array, local array");

        //finish rest of tests with different config
        unset($siteConfig->localConfig);
        $SM_siteManager->loadSite('localConfig-test.xsm');

        //  - global is single, local is single
        $results = $siteConfig->getVar('dirs', 'config', true);
        $this->assert((!empty($results)&& sizeof($results) == 2),"global single, local single");
        
        
        //  - global is an array, local is single
        $results = $siteConfig->getVar('dirs', 'libs', true);
        $this->assert((!empty($results)&& sizeof($results) == 3), "global array, local single");

        //  - global is single, local is an array
        $results = $siteConfig->getVar('dirs', 'modules', true);

        $this->assert((!empty($results) && sizeof($results) == 3), "global single, local array");
        
        //not set $lVar (mergeGlobal or not)
        
        //not mergeGlobal && is set $lVar
                

    }

	function testFail() {
		$this->assert(1==0, '1 should not equal 0');
	}

}

?>