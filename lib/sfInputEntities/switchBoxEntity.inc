<?php

/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
 *******************************************************************
 *
 * This source file is subject to version 1.0 of the Roadsend Public
 * License, that is bundled with this package in the file 
 * LICENSE, and is available through the world wide web at 
 * http://www.roadsend.com/license/rpl1.txt
 *
 *******************************************************************
 * Author(s): Shannon Weyrick (weyrick@roadsend.com)
 *
 */

/**
 *
 * a group of checkboxes inside one entity
 *  
 *  directives:
 *  
 *      returnVal   - the default value each box will return if it's on
 *                  default "ON"
 *      numPerRow   - how many boxes to add per row
 *
 * 
 * changes:
 *      
 *     entity titles are now wrapped in entity class spans by default 
 *
 * @author Shannon Weyrick <weyrick@roadsend.com> 
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class switchBoxEntity extends SM_inputTypeEntity {

    /** 
    * a hash of name => title for each checkbox in the box 
    * @var array
    */
    var $boxList;

    /** 
    * which check boxes are "on" (checked) 
    * @var array
    */
    var $selected;

    /**
     * template for output, optional
     * @var array SM_layoutTemplate
     */
    var $tpt = NULL;

    /**
     * list of templates to use for use by check boxes in specific columns
     * @var array string
     */
    var $colTptList = NULL;
    
   /**
    * setup entity 
    */
    function entityConfig() {
    
        // setup some defaults
        $this->directive['returnVal']       = 'ON';
        $this->directive['numPerRow']       = 3;
        /**
         * default templates to use for use by check boxes in all columns not specified
         * @var array string
         */
        $this->directive['defaultColTpt']   = NULL;
        
    }


    /** 
     * set a template to use for output of a checkbox in a single column
     * expects two areas, one named 'title', one named 'checkbox'
     * if a column template isn't specified, it will use defaultColTpt if set, or create a standard one otherwise
     * @param string $areaName area name of the column from the main template
     * @param string $templateName template file name to use for checkbox in this column
     */
    function setColumnTemplate($areaName, $templateName) {

        $this->colTptList[$areaName] = $templateName;

    }

    /**    
     * use this function to add a checkbox to the list in the box
     * $data will be the VALUE="", $title will be displayed
     * to the user.
     * @param string $name varname of checkbox
     * @param string $title title for this checkbox
     * @param bool $checked when true, the checkbox will be on by default
     * @param string $areaName area name to place check box, if using a template
     *
     */
    function addBox($name, $title, $checked=false, $areaName=NULL) {
    
        global $SM_siteManager;

        $this->boxList[$name] = $title;
        
        $p = NULL;
        if (isset($_POST[$name])) {
            $p = $_POST[$name];
        }

        if ($this->parentFormEntity->parentSmartForm->ftl) {
            if ($checked) 
                $this->selected[$name] = 'checked="checked"';
        }
        else {
            if ($p == $this->directive['returnVal']) {
                $this->selected[$name] = 'checked="checked"';
            }
        }

        // add to smartForm internal var list
        $this->parentFormEntity->parentSmartForm->addInternalVar($name);

        // if using a template, add to area
        if ($this->tpt != NULL && !empty($areaName)) {

            $attr = $this->getCommonAttr();

            (isset($this->selected[$name])) ? $sel = $this->selected[$name] : $sel = '';

            if (empty($this->directive['entityClassTag'])) 
                $tT = $title."\n";
            else
                $tT = "<span class=\"{$this->directive['entityClassTag']}\">$title</span>\n";

            $cB = "<input $attr type=\"checkbox\" name=\"{$name}\" value=\"{$this->directive['returnVal']}\" $sel />";

            if (isset($this->colTptList[$areaName])) {
                // column template specific to this area
                $ct = $SM_siteManager->loadTemplate($this->colTptList[$areaName]);
                $ct->addText($cB, 'checkbox');
                $ct->addText($tT, 'title');
                $this->tpt->addTemplate($ct, $areaName);
            }
            elseif (!empty($this->directive['defaultColTpt'])) {
                // default column template
                $ct = $SM_siteManager->loadTemplate($this->directive['defaultColTpt']);
                $ct->addText($cB, 'checkbox');
                $ct->addText($tT, 'title');
                $this->tpt->addTemplate($ct, $areaName);
            }
            // fall through - no template, make our own
            else {
                // entity class tag sets the css style of the title output for the entity
                $this->tpt->addText($cB.$tT, $areaName);
            }

        }

    }
    
    /**
     * wrapper for consistency with other addOption type entities
     * @param string $name varname of checkbox
     * @param string $title title for this checkbox
     * @param bool $checked when true, the checkbox will be on by default
     * @param string $areaName area name to place check box, if using a template
     *
     */
    function addOption($name, $title, $checked=false, $areaName=NULL) {
        $this->addBox($name, $title, $checked, $areaName);
    }
                           

    /**
     * load a template for output
     * @param string $templateName name of template file to load
     * @return SM_layoutTemplate
     */
    function loadTemplate($templateName) {

        global $SM_siteManager;

        $this->tpt = $SM_siteManager->loadTemplate($templateName);
        return $this->tpt;

    }


    /**
     * build a standard table, no template
     *
     */
    function _doStandardTable() {

        $this->say("<table width='100%'>\n");

        $numRowsInCol = ceil(sizeof($this->boxList) / $this->directive['numPerRow']);
        $classTag = $this->directive['entityClassTag'];

        $attr = $this->getCommonAttr();

        $col = 1;
        $row = 1;
        foreach ($this->boxList as $title => $name) {
            $colList[$col][$row++][$title] = $name;
            if ($row > $numRowsInCol) {
                $col++;
                $row = 1;
            }
        }

        for ($r=1; $r <= $numRowsInCol; $r++) {
            $this->say("<tr>");
            for ($c=1; $c <= $this->directive['numPerRow']; $c++) {
    
                
                $title = current($colList[$c][$r]);
                $name  = key($colList[$c][$r]);

                if (empty($title) && empty($name))
                    continue;

                (isset($this->selected[$name])) ? $sel = 'CHECKED' : $sel = '';
                // entity class tag sets the css style of the title output for the entity
                if (empty($classTag))
                    $this->say("<td valign='top'><input $attr type=\"checkbox\" name=\"{$name}\" value=\"{$this->directive['returnVal']}\" $sel />$title</td>\n");
                else
                    $this->say("<td valign='top'><input $attr type=\"checkbox\" name=\"{$name}\" value=\"{$this->directive['returnVal']}\" $sel /><span class=\"$classTag\">$title</span></td>\n");
                
                
                
            }
            $this->say("</tr>\n");
        }
        
        $this->say("</table>\n");
    }

    /**
    * output 
    */
    function entityThink() {

        // go ahead and return if there's no options
        if (!is_array($this->boxList)) {
            $this->debugLog("output: no options for switchBox to output");
            return 'N/A';
        }
        
        if ($this->tpt != NULL) {
            $this->say($this->tpt->run());
        }
        else {
            $this->_doStandardTable();
        }
        
    }

}

?>

