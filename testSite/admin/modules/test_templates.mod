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

Templates
    SM Tags
        QuickTags
        Extended Tags
        Priority
        Configuring
        Resetting
    Template Load Types
        From Disk
        Set From Code
    Area Manipulation
        Set Priority
        Get Area List
        Get Area Attributes
        Add Data To Areas
            Modules
            Templates
            CodePlates
            Text
    Reset Template
    Load Cacheing
    
*/

// make sure base is loaded
global $SM_siteManager;
$SM_siteManager->includeModule('testBase');

/**
 * extend testBase for testing sitemanager functionality
 */
class test_templates extends testBase {
     
    /** title output at top of test module */
    var $testTitle = 'Template Functionality Test';

    /** description */
    var $testDesc  = 'Test the functionality of the Template System.';

     /**
      * run by base class after base runs moduleConfig()
      */
    function T_moduleConfig() {

        // module config stuff here


    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
    

        // TEST
        $tpt = $this->loadTemplate('test');
        $tpt->addText('(sample text added to area)','areaOne');
        $actual = $tpt->run();
        $expect = '<b>Sample Template</b><br>
(sample text added to area)
<br>
Template Test Complete
';
        $this->addTest('loadTemplate()', 
                       'Load, parse and run template from file',
                       $expect,
                       $actual);

        // TEST
        $tpt = new SM_layoutTemplate(); 
        $text = '<b>Template From Code</b><Br><SM class="sample" type="area" name="areaTwo"><br>Template Test Complete';
        $tpt->setTemplateData($text);
        $tpt->addText('(sample text added to area)','areaTwo');
        $actual = $tpt->run();
        $expect = '<b>Template From Code</b><Br><span class="sample">'."\n".'(sample text added to area)'."\n".'</span><br>Template Test Complete';
        $this->addTest('setTemplateData', 
                       'Create template, use template from code.',
                       $expect,
                       $actual);

        // TEST
        $tpt = new SM_layoutTemplate();
        $text = '<b>Extended Tags</b><Br><SM type="area" name="areaTwo">this is some data</SM><br>Template Test Complete';
        $tpt->setTemplateData($text);
        $tpt->addText('(sample text added to area in extended tag)','areaTwo');
        $tList = $tpt->getTagsByType('AREA');
        foreach ($tList as $tag) {
            if ($tag->data != '')
                $tpt->addText($tag->data, 'areaTwo');
        }
        $actual = $tpt->run();
        $expect = '<b>Extended Tags</b><Br>(sample text added to area in extended tag)this is some data<br>Template Test Complete';
        $this->addTest('extended tags',
                       'Test extended SM tags with data',
                       $expect,
                       $actual);

        // TEST
        $tpt = new SM_layoutTemplate(); 
        $text = '<b>Sub Template Test</b><br><SM type="area" name="subArea"/><br>Bottom Of First Template';        
        $tpt->setTemplateData($text);
        $tpt2 = new SM_layoutTemplate();
        $text2 = '<b><sm type="area" name="subMain"></b>Bottom of Sub Template';
        $tpt2->setTemplateData($text2);
        $tpt2->addText('Sub Template Area Text<br>', 'subMain');
        $tpt->addTemplate($tpt2, 'subArea');
        $actual = $tpt->run();
        $expect = '<b>Sub Template Test</b><br><b>Sub Template Area Text<br></b>Bottom of Sub Template<br>Bottom Of First Template';
        $this->addTest('Sub Templates', 
                       'Create a template, add another template to an area in the first (subtemplate)',
                       $expect,
                       $actual);

        // TEST
        $cpt = $this->loadCodePlate("testCodePlate");
        $tpt2 = new SM_layoutTemplate();
        $text2 = '<b><sm type="area" name="subMain"></b>Bottom of Sub Template';
        $tpt2->setTemplateData($text2);
        $tpt2->addText('Sub Template Area Text<br>', 'subMain');
        $cpt->addText('<b>Codeplate Test</b><br>', 'areaOne');
        $cpt->addTemplate($tpt2, 'areaOne');
        $actual = $cpt->run();
        $expect = '<b>Sample Template</b><br>
<b>Codeplate Test</b><br><b>Sub Template Area Text<br></b>Bottom of Sub Template<span id="SM4">
<table width="100%" border="0" cellspacing="0" cellpadding="0"><tbody>
<tr>
<td><B>Hello World</B></td></tr></tbody>
</table>
</span>

<br>
Template Test Complete
';
        $this->addTest('Template in Codeplate', 
                       'Load a CodePlate, add some text and another template to an area in the CodePlate',
                       $expect,
                       $actual);
        

        // custom SM tag
        $tpt = $this->loadTemplate('smTagTest');
        $actual = $tpt->run();
        $expect = '<b>Sample Template SM TAG tester</b><br>
EXAMPLE tag: babble
<br>
SM TAG Template Test Complete

';
        $this->addTest('Custom SM Tag', 
                       'Load a template that makes use of a custom template tag EXAMPLE',
                       $expect,
                       $actual);

    }
    
}


?>
