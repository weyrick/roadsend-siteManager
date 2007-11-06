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
 *  input entity implementing an entity grid
 *  
 *  this is to be used when a grid of entities is needed where 
 *  there is a set number of columns, each column having a set
 *  entity type, and a dynamic number of rows. this is used in
 *  a shopping cart or database editor, for example
 *
 *
 */
/** 
 * a data structure for holding column information 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class eg_column {
    
    /**
    * header displayed at the top of this column 
    * @var string
    */
    var $header = '';

    /** 
    * a list of filters to assign to entities in this column, keyed by filterType 
    * @var array
    */
    var $filterList = array();

    /** 
    * the entity type all entities in this column will use 
    * @var string
    */
    var $entityType = '';

    /** 
    * a list of directives to assign to entities in this column 
    * @var array
    */
    var $entityDirectives = array();

    /**
     * single dimension flag will cause entities in this column to have
     * a common form name. useful for radio buttons to select from
     * mutliple rows
     */
    var $singleDimension = false;    

}

/** 
 * a class for holding row information 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class eg_row {

    /** 
    * a list of entities in this row,  keyed by colID (varName) 
    * @var array
    */
    var $entityList;
    var $rowTemplateData = array();


    /** 
    * a list of template VAR data fields for custom row templates 
    * @var array
    */
    function setRowTemplateData($data){
        $this->rowTemplateData = $data;
    }

    function getRowTemplateData() {
        return $this->rowTemplateData;
    }



    /**
    * loop through entities and gather their output, placing each entities output in a <TD></TD> column
    */
     
    function output($colID) {

        if (!isset($colID)) {
            SM_fatalErrorPage("output: invalid column: $colID",$this);
        }

        if (!isset($this->entityList[$colID])) {
            SM_debugLog("output: entity is NULL for this column",$this);
            return '<br />';
        }

        $output = '';
        $entity = $this->entityList[$colID];
        $output .= $entity->output();
        if (!$entity->inputEntity->isValid) {
            $output .= $entity->getTitle();
        }

        return $output;
    }

     /**
      * add a pre-created entity to this row 
      * @param string $colID the column to put this entity into
      * @param SM_formEntity an SM_formEntity object
      */
    function addEntity($colID, $entity) {
        if ($entity == NULL) {
            SM_fatalErrorPage("entity was null for colID $colID",$this);
        }
        $this->entityList[$colID] = $entity;
    }

    /**
     * apply all filters on entities in this row
     * @return bool true if all filters passed, false if any failed
     */
    function applyFilters() {
        $isValid = true;
        foreach ($this->entityList as $entity) {            
            $entity->applyFilters();
            // if the filter wasn't satisfied, then neither are we
            if (!$entity->inputEntity->isValid) {
                $isValid = false;
            }
        }
        return $isValid;
    }

}

/**
 * the actual entityGridEntity class
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class entityGridEntity extends SM_inputTypeEntity {

    /** 
    * a list of columns in this entity, keyed by colID (varName) 
    * @var array
    */
    var $colList = array();

    /** 
    * a list of rows in this entity, keyed by rowID 
    * @var array
    */
    var $rowList = array();

    /** 
    * final output template 
    * @var SM_layoutTemplate
    */
    var $outputTemplate = NULL;

    /** 
    * a row template filename or SM_layoutTemplate object
    * @var string or SM_layoutTemplate
    */
    var $rowTemplate = NULL;

    /** 
    * a row template filename or SM_layoutTemplate object
    * @var string or SM_layoutTemplate
    */
    var $altRowTemplate = NULL;

	var $columnHeaderRow = NULL;
	
	var $useAltRowTemplate = false;
    /**
    * configure the entity
    */

    function entityConfig() {

        // clean layout up, we'll do our own
        $this->parentFormEntity->layout = SF_LAYOUT_SINGLENOBR;
        
        // force filters on
        $this->parentFormEntity->forceFilter = true;

        // table settings        
        $this->directive['tableBorder']         = 0;
        $this->directive['tableClass']           = "gridEntityClass";
        //$this->directive['tableWidth']          = "100%";
        $this->directive['tableCellPadding']    = "3";
        $this->directive['tableCellSpacing']    = "3";
        $this->directive['tableBgColor']        = "";
        // if this is true, the entity grid will only apply filters to
        // the row that had a submit button pressed. if a submit button wasn't
        // added as a column on the grid, this has no effect
        $this->directive['filterSubmitRowOnly'] = false;
        $this->directive['haveSubmit']          = false;

        $this->directive['showColumnHeaders']   = true;
        $this->directive['columnHeadersBgColor'] =  "";


    }

    /**
     * add a column definition to the grid
     * @param string $colID the identified for this column, used for variable names in this grid. all entities added
     *                        to this column will have the variable name $colID_$rowID
     * @param string $header text string to describe this column, placed at the top of the table above this column
     * @param string $entityType a valid SmartForm Input Entity type
     * @param array $entityDirectives a list of directives to assign to entities added to this column
     * @param bool $singleDimension when true, the name of the column will not include the rowID, only colID. useful for radio boxes.
     */
    function addColumn($colID, $header, $entityType, $entityDirectives=NULL, $singleDimension=false) {

        if (isset($this->colList[$colID])) {
            $this->debugLog("Column $colID already defined");
            return;
        }

        SM_sfLoadEntity($entityType);

        // set info
        $this->colList[$colID] = new eg_column();
        $this->colList[$colID]->header = $header;
        $this->colList[$colID]->entityType = $entityType;
        $this->colList[$colID]->singleDimension = $singleDimension;

        // add directives
        if ($entityDirectives != NULL) {
            $this->colList[$colID]->entityDirectives = $entityDirectives;
        }

        // remember if this is a submit
        if ($entityType == 'submit') {
            $this->directive['haveSubmit'] = true;
        }

    }

    /**
     * add a filter to be added to entities added to a column
     * @param string $colID column to add this filter assignment to
     * @param string $filterType a valid SmartForm Filter type
     * @param string $bMsg text string displayed when filter fails
     * @param array $filterDirectives a list of directives assigned to the filter when assigned to the entity
     */
    function addColumnFilter($colID, $filterType, $bMsg='Invalid Input', $filterDirectives=NULL) {

        if (empty($this->colList[$colID])) {
            $this->debugLog("addColumnFilter: Column $colID doesn't exist");
            return;
        }        

        if ($filterDirectives == NULL) {
            $filterDirectives = array();
        }

        $this->colList[$colID]->filterList[$filterType]['directives'] = $filterDirectives;
        $this->colList[$colID]->filterList[$filterType]['bMsg'] = $bMsg;

    }
             
    /**
     * add a row definition. this method is optional, as rows will be created on the fly by
     * addRowEntity()     
     * @param string $rowID row identifier, used in the variable name ($colID_$rowID)
     * @return bool true on success, false otherwise
     */
    function addRow($rowID) {
        if (isset($this->rowList[$rowID])) {
            $this->debugLog("addRow: row [$rowID] already existed");
            return false;
        }
        $this->rowList[$rowID] = new eg_row();
        return true;
    }
    
    /**
     * add a row data set to configure a custom row template
     * this method is optional.
     * @param string $rowID row identifier, used in the variable name ($colID_$rowID)
     * @param array $data hash to configure the VAR field for a row template
     * @return bool true on success, false otherwise
     */
    function addRowTemplateData($rowID,$data) {
        
        // If row doesn't exist - add it now
        $this->addRow($rowID);    
        
        // Set custom template data
        $this->rowList[$rowID]->setRowTemplateData($data);

    }
    
    /**
     * add an entity to a predefined row
     * @param string $rowID row identifier, used in the variable name ($colID_$rowID)
     * @param string $colID column to add this entity to, in this row (must have been defined already with addColumn)
     * @param string $defaultVal default value shown for this entity
     * @param bool $required whether this entity receives the required filter by default
     * @param string $entityTypeOverride setting an entity type here overrides the column entity type for this location
     * @return SM_entityGridEntity a reference to the newly created entity, for further customization
     */
    function addRowEntity($rowID, $colID, $defaultVal='', $required=false, $entityTypeOverride=NULL) {

        // need column
        if (empty($this->colList[$colID])) {
            $this->debugLog("addColumnFilter: Column $colID doesn't exist");
            return;
        }        

        // create on the fly if necessary
        if (empty($this->rowList[$rowID])) {
            $this->rowList[$rowID] = new eg_row();
        }       

        // get info
        if ($entityTypeOverride == NULL) {
            $entityType = $this->colList[$colID]->entityType;
        }
        else {
            // make sure overridden entity type is loaded
            SM_sfLoadEntity($entityTypeOverride);
            $entityType = $entityTypeOverride;
            if ($entityType == 'submit')
                $this->directive['haveSubmit'] = true;
        }

        if ($this->colList[$colID]->singleDimension) {
            $varName = $colID;
        }
        else {
            $varName = $colID.'_'.$rowID;
        }

        $filterList = $this->colList[$colID]->filterList;
        $dirs = $this->colList[$colID]->entityDirectives;

        // create and configure
        $newEntity = new SM_formEntity($varName, ' ', $entityType, $required, $this->parentFormEntity->parentSmartForm, 0, $defaultVal);
        $newEntity->inputEntity->configure($dirs);

        // add filters?
        if (is_array($filterList)) {
            foreach ($filterList as $filterType => $filterData) {
                $filter = $newEntity->inputEntity->addFilter($filterType, $filterData['bMsg'], $varName);
                $filter->configure($filterData['directives']);
            }
        }

        // add entity to row
        $this->rowList[$rowID]->addEntity($colID, $newEntity);

        // return entity for further processing by user
        return $newEntity->inputEntity;
    }

    /**
     * return a list of variable names used in the grid (colID's). this does not return all variable names as used
     * in the final output, only the column variable names used to build them.
     * @return array list of variable names
     */
    function getVarList() {
        return array_keys($this->colList);
    }

    /**
     * identical to getVarList()
     * @return array a list of col ID's     
     */
    function getColIDList() {
        return array_keys($this->colList);
    }

    /**
     * @return array a list of row ID's     
     */
    function getRowIDList() {
        return array_keys($this->rowList);
    }

    /**
     * return a hash of information, keyed by column name, of all information
     * in the requested row
     * @param string $rowID row to retrieve posted variables for
     */
    function getRowByID($rowID) {
        $rowData = array();
        foreach (array_keys($this->colList) as $colID) {
            if (isset($_POST[$colID.'_'.$rowID])) {
                $rowData[$colID] = $_POST[$colID.'_'.$rowID];
            }
        }
        return $rowData;
    }

    /**
     * return a hash of information, keyed by row ID,, of all information
     * in the requested column
     * @param string $colID col to retrieve posted variables for
     */
    function getColByID($colID, $postFix='') {
        $colData = array();
        foreach (array_keys($this->rowList) as $rowID) {
            if (isset($_POST[$colID.'_'.$rowID.$postFix])) {
                $colData[$rowID] = $_POST[$colID.'_'.$rowID.$postFix];
            }
        }
        return $colData;
    }

    /**
     * return rowID and value of submit button that was pressed in the grid,
     * or NULL if none
     */
    function getSubmitButton() {
        foreach ($this->colList as $colID => $colObj) {
            if ($colObj->entityType == 'submit') {
                $cData = $this->getColByID($colID);
                if (sizeof($cData)) {
                    list ($rowID) = array_keys($cData);
                    $sB = array('rowID'=>$rowID, 'value'=>$cData[$rowID], 'colID' => $colID);
                    return $sB;
                }
                else {
                    // check for _x version, for submit images
                    $cData = $this->getColByID($colID,'_x');
                    if (sizeof($cData)) {
                        list ($rowID) = array_keys($cData);
                        $sB = array('rowID'=>$rowID, 'value'=>$cData[$rowID], 'colID' => $colID);
                        return $sB;
                    }
                }
            }
        }
        // not found
        return NULL;
    }

    /**
     * get the value of a variable located at row rowID, in column varName (colID)
     * @param string $rowID the row to retrieve variable from
     * @param string $varName the variable to retrieve (colID)
     * @return mixed value posted to form
     */
    function getVar($rowID, $varName) {
        if (isset($_POST[$varName.'_'.$rowID]))
            return $_POST[$varName.'_'.$rowID];
        else
            return NULL;
    }

    /**
     * override SM_inputEntity::applyFilters so we can run the filters of our internal entities
     *
     */
    function applyFilters() {

        // if we should only apply filters to the submit row, check that here
        if ($this->directive['filterSubmitRowOnly'] && $this->directive['haveSubmit']) {
            $sBut = $this->getSubmitButton();
            $submitRow = $sBut['rowID'];
        }
        else {
            $submitRow = NULL;
        }

        // apply filters for my entities
        foreach ($this->rowList as $rowID => $row) {

            // skip this row if we're only checking submit rows, and this isn't it
            if ($submitRow != NULL && $rowID != $submitRow) {
                continue;
            }

            // if the filter wasn't satisfied, then neither are we
            if (!$row->applyFilters()) {
                $this->isValid = false;
            }

        }

        // if we're invalid, stop here
        if (!$this->isValid) {
            return;
        }

        // call parent
        parent::applyFilters();

    }

    /**
     * tell the smartform it should use the given SM_layoutTemplate template
     * for layout
     * @param SM_layoutTemplate $templateObj the template the form should use for output
     */
    function useAltRowTemplate($templateObj) {
    	$this->addDirective('showColumnHeaders',false);
    	$this->useAltRowTemplate = true;
        $this->altRowTemplate = $templateObj;
    }

    /**
     * load a row template
     * @param string $fileName siteManager .tpt file
     */
    function setAltRowTemplate($fileName) {
        $this->addDirective('showColumnHeaders',false);
        $this->useAltRowTemplate = true;
        $this->altRowTemplate = $fileName;
    }


    /**
     * tell the smartform it should use the given SM_layoutTemplate template
     * for layout
     * @param SM_layoutTemplate $templateObj the template the form should use for output
     */
    function useRowTemplate($templateObj) {
    	$this->addDirective('showColumnHeaders',false);
        $this->rowTemplate = $templateObj;
    }

    /**
     * load a row template
     * @param string $fileName siteManager .tpt file
     */
    function setRowTemplate($fileName) {
        $this->addDirective('showColumnHeaders',false);
        $this->rowTemplate = $fileName;
    }
    
    function setColumnHeaderRow($row) {
        $this->addDirective('showColumnHeaders',false);
        $this->columnHeaderRow = $row;
    }
    
    /**
     * make a template for output
     */
    function makeTemplate() {

        global $SM_siteManager;

        $template = "<table class=\"{$this->directive['tableClass']}\" width=\"{$this->directive['tableWidth']}\" border=\"{$this->directive['tableBorder']}\"";

        if ($this->directive['tableBgColor'] != '')
            $template .= " bgcolor=\"{$this->directive['tableBgColor']}\" ";

        $template .= " cellpadding=\"{$this->directive['tableCellPadding']}\" cellspacing=\"{$this->directive['tableCellSpacing']}\">";


        // headers for columns
        if(!empty($this->columnHeaderRow)) {
        	$template .= $this->columnHeaderRow;
        } else if ($this->directive['showColumnHeaders']) {
            $template .= '<tr class="columnHeader" bgcolor="'.$this->directive['columnHeadersBgColor'].'">';
            foreach ($this->colList as $colID => $colObj) {
                $template .= "<td align=\"center\"><b>$colObj->header</b></td>\n";
            }
            $template .= '</tr>';
        }

        // if we don't have a custom row template...
        if(empty($this->rowTemplate)) {
            $rowTemplate = "<tr class='columnRow'>";
            foreach ($this->colList as $colID => $colObj) {
                $rowTemplate .= "<td><SM TYPE='area' NAME='$colID'></td>";
            }
            $rowTemplate .= "</tr>\n";	
            $sfTemplate = new SM_layoutTemplate;
            $sfTemplate->setTemplateData($rowTemplate);
            $this->rowTemplate = $sfTemplate;		
        }

        // rows
        $altRow = false;
        foreach ($this->rowList as $rowID => $rowObj) {

            if ($altRow) {

                if ($this->altRowTemplate instanceof SM_layoutTemplate) {
                    $rT = clone $this->altRowTemplate;
                } else {
                    // loadTemplate will return clone
                    $rT = $SM_siteManager->loadTemplate($this->altRowTemplate);
                }

            } else {

                if ($this->rowTemplate instanceof SM_layoutTemplate) {
                    $rT = clone $this->rowTemplate;
                } else {
                    // loadTemplate will return clone
                    $rT = $SM_siteManager->loadTemplate($this->rowTemplate);
                }
            }
            
            foreach ($this->colList as $colID => $colObj) {
                $areaName = $rowID.'_'.$colID;
                $rT->addText("<SM TYPE='area' NAME='$areaName'>",$colID);
            }
            
            // Configure Data for row template 
            $rT->configureTag('VAR',array('data'=>$this->rowList[$rowID]->getRowTemplateData() ));
            $template .= $rT->run()."\n";

            if($this->useAltRowTemplate) $altRow = !$altRow;
        }

        $template .= "</table>\n";
        
        // make template, set data
        $this->outputTemplate = new SM_layoutTemplate();
        $this->outputTemplate->setTemplateData($template);

    }


    /**
    * output 
    */

    function entityThink() {
        
        // make template
        $this->makeTemplate();
        
        // rows
        foreach ($this->rowList as $rowID => $rowObj) {
            foreach ($this->colList as $colID => $colObj) {
                $areaName = $rowID.'_'.$colID;
                $rowOP = $rowObj->output($colID);
                $this->outputTemplate->addText($rowOP, $areaName);
            }
        }

        $this->say($this->outputTemplate->run());

    }


}

?>
