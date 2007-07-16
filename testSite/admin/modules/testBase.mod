<?php

/*********************************************************************
*  Roadsend SiteManager
*  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
**********************************************************************
*
* This source file is subject to version 1.0 of the Roadsend Public
* License, that is bundled with this package in the file 
* LICENSE, and is available through the world wide web at 
* http://www.roadsend.com/license/rpl1.txt
*
**********************************************************************
* Author(s): weyrick
*
*/

/*

        // TEST
        $actual = '';
        $expect = '';
        $this->addTest('title', 
                       'desc',
                       $expect,
                       $actual);
                       
*/

/**
 * simple data structure class
 */
class testEntry {
    var $title;
    var $desc;
    var $expectedResult;
    var $actualResult;
}

/**
 * base module for test modules
 *
 * this provides common testing support methods
 *
 */
class testBase extends SM_module {
     
    /**
     * common variables
     */

    /** title output at top of test module */
    var $testTitle = 'Unknown Test Module';

    var $testDesc  = 'Unknown description for this test module';

    /** array to hold tests */
    var $testList = array();

    function moduleConfig() {

        // run child class T_moduleConfig, if it exists
        if (method_exists($this, 'T_moduleConfig')) {
            $this->T_moduleConfig();
        }

        // setup pre and post ThinkList
        $this->preThinkList[] = 'testPreThink';
        $this->postThinkList[] = 'showResults';

    }


    /** 
     * add a test entry to the test table 
     */
    function addTest($title, $desc, $expectedResult, $actualResult) {
       
        $newTest = new testEntry();
        $newTest->title             = $title;
        $newTest->desc              = $desc;
        $newTest->expectedResult    = $expectedResult;
        $newTest->actualResult      = $actualResult;

        if (is_array($newTest->expectedResult)) {
            ob_start();
            print_r($newTest->expectedResult);
            $newTest->expectedResult = nl2br(ob_get_contents());
            ob_end_flush();
        }
        if (is_array($newTest->actualResult)) {
            ob_start();
            print_r($newTest->actualResult);
            $newTest->actualResult = nl2br(ob_get_contents());
            ob_end_flush();
        }

        $this->testList[] = $newTest;

    }

    /**
     * method run before think method
     */
    function testPreThink() {

        // output title
        $this->saybr("<center><b>$this->testTitle</b></center>");
        $this->saybr("$this->testDesc<br>");

    }


    /**
     * run the tests and display the output
     */
    function showResults() {

        // no tests, don't show
        if (sizeof($this->testList) == 0) {
            return;
        }

        $this->say('<table width="100%" border=1>');

        // column headers
        $this->say("<tr><td align='center'><b>Test Title</td><td align='center'><b>Description</td><td align='center'><b>Expect</td><td align='center'><b>Actual</td><td align='center'><b>Result</td></tr>");

        foreach ($this->testList as $testItem) {

            $this->say("\n<tr>");
                
            // stats
            $this->say("<td>$testItem->title</td><td>$testItem->desc</td><td>$testItem->expectedResult</td><td>$testItem->actualResult</td>");

            // result..
            if (strcmp($testItem->expectedResult,$testItem->actualResult)) {
                for ($i=0; $i<strlen($testItem->expectedResult); $i++) {
                    if ($i >= strlen($testItem->actualResult)) {
                        $this->debugLog("in expected but not actual $i: ".$testItem->expectedResult{$i});
                    }
                    elseif ($testItem->expectedResult{$i} != $testItem->actualResult{$i}) {
                        $this->debugLog("$i: ".$testItem->expectedResult{$i}.'!='.$testItem->actualResult{$i});
                    }
                }
                $this->debugLog("FAILED TEST");
                $this->debugLog("expect: [$testItem->expectedResult]");
                $this->debugLog("actual: [$testItem->actualResult]");
                $this->say('<td align="center" bgcolor="red"><b>FAIL</td>');
            }
            else {
                $this->say('<td align="center" bgcolor="green"><b>PASS</td>');
            }

            $this->say('</tr>');

        }

        $this->say('</table>');

    }

}


?>
