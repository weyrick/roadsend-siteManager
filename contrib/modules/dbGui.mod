<?php

/*********************************************************************
 *  Roadsend SiteManager
 *  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
 *********************************************************************
 *
 * This source file is subject to version 1.0 of the Roadsend Public
 * License, that is bundled with this package in the file 
 * LICENSE, and is available through the world wide web at 
 * http://www.roadsend.com/license/rpl1.txt
 * see dbRecordSelector.mod
 *
 *********************************************************************
 * Author(s): gollum, weyrick
 *
 * TODO:
 *
 *   - go through all code and check use of references
 *   - determine effeciency of get/set methods?
 *
 * Change Log:
 *
 *      10/25/2001 - created. loosely based off of the "mySQLdbGUI" module.
 *
 *      11/20/2001 - columnDefinition class removed from this file and 
 *                   loaded using the new 'includeLib' method.
 *                  
 *                   added 'runTableFormat' too keep track of weather or not
 *                   getTableFormat() should run (gollum)
 *
 *      11/28/2001 - Modified setFilter to reflect changes in the 
 *                   columnDefinition class (gollum)
 *
 *      12/12/2001 - Modified to use the useSmartForm and getResource
 *                   functions insted of keeping a class var for the 
 *                   smartform. (gollum)
 *
 *      12/18/2001 - Code that actually gets the values for the form elements
 *                   is now handeled inside of abstractDbEditor. (gollum)
 *
 */

global $SM_siteManager;
$SM_siteManager->includeModule('abstractDbEditor');

/**
 *
 * Extra Directives:
 *      dbHook - The name of the dbGuiBaseHook class that was over ridden
 *      hiddens - list of columns to be hidden
 *      orderBy - order by clause for database queries 
 *      smartFormTemplate - The name of the smart form template to use
 *      whereClause - where clause for database queries
 *
 */

class SM_dbGui extends SM_abstractDbEditor {

    /** column objects hash **/
    var $tableDef = array();

    /** Track weather or not 'getTableFormat()' should be run **/
    var $runTableFormat=true;

    /** the row of data we are working on, as pulled from the database, if available */
    var $rowData = array();
    
    /**
     * configure the module. run before moduleThink
     */        
    function moduleConfig() {        
        // directives       
        $this->directive['maxVisibleSize']  = 30;                  // max size of a form element (default)     
        $this->directive['displayAsXML']    = false;               // display the form as XML     
        $this->directive['dumpTemplate']    = false;               // display the forms as a template
        $this->directive['dbOverRide']      = NULL;                // display the forms as a template    
        $this->directive['autoFilter']      = true;                // auto filters
        $this->directive['columnClass']     = 'columnDefinition';  // which cloumn class to use. Should extend 'columnDefinition'
        $this->directive['dbHook']          = NULL;                // dbHook class name
        $this->directive['dbHookDirectory'] = 'modules';           // where to look for the dbHook class file
        

        // parents
        parent::moduleConfig();
    }   
    
    /**
     *  This function will configure fields within the form
     *  based on user input from directive scripts
     */
    function configureForm() {
        // loop through each field and see if we have any config directives for it
        foreach($this->tableDef as $field) {
            $fName = $field->getName();
            
            // if we have any, pass along any column specific directives
            if (empty($this->directive[$fName]))
                continue;

            $field->setDirectives($this->directive[$fName]);
        }
    }

    /**
     * called before moduleThink() - MUST be defined!
     *
     */
    function defineForm() {

        // get the table format
        $this->getTableFormat();

        // configure the fields
        $this->configureForm();

        $myForm = $this->getResource('myForm');

        if(isset($this->directive['smartFormTemplate']))
            $myForm->setTemplate($this->directive['smartFormTemplate']);

        // handle file uploads
        if (!empty($this->directive['fileUpload'])) {
            $myForm->addDirective("enctype","multipart/form-data");
        }

        // if dbu flag is set, notify the user of a database update
        if ($this->getVar('dbu') == '1') {       
            $myForm->setHeader("THE DATABASE HAS BEEN UPDATED");
        }
        else {
            // it wasn't set, so no database write. but, if it's not a first time load,
            // tell them there was a problem with the form
            if (!$myForm->ftl)
                $myForm->setHeader("THE DATABASE WAS NOT UPDATED -- THERE IS A PROBLEM WITH THE INPUT");
        }

        // alt colors
        $myForm->addDirective('rowColorAlt1','#EEEEEE');
        $myForm->addDirective('rowColorAlt2','#FFFFFF');

        // add hiddens
        if (isset($this->directive['hiddens']) && sizeof($this->directive['hiddens']))
            $myForm->addDirective('hiddens',$this->directive['hiddens']);

        // Add smartForm level directives         
        if (is_array($this->directive['smartFormDirectives'])) {
        
            foreach ($this->directive['smartFormDirectives'] as $sfDir=>$sfVal){
                $myForm->addDirective($sfDir,$sfVal);
            }
        }

        // loop through each field we found
        foreach($this->tableDef as $field) {
            $field->addToSmartForm($myForm, $this->sType, $this->directive['dataBaseID']);
        }               


        // include an xml format of the form.
        if ($this->directive['displayAsXML'] == true) {

            if (!function_exists('SM_saveForm')) {
                require('contrib/lib/smSaveForm.inc');
            }
            $xmlPage = SM_saveForm($myForm);

            $this->say("YOUR XML PAGE <BR> <FORM>");
            $this->say("<TEXTAREA ROWS=50 COLS='150'>");
            $this->say($xmlPage);
            $this->say("</TEXTAREA>");
            $this->say("</FORM>");
        }
        
        if ($this->directive['dumpTemplate'] == true) {
            
            $this->saybr("TEMPLATE DUMP");
            $myForm->addDirective['dumpTemplate'];
            $myForm->buildTemplate();
            $dump = join("\n",$myForm->template->htmlTemplate);

            $this->say("<TEXTAREA WRAP='VIRTUAL' ROWS=20 COLS='80'>");
            $this->say($dump);
            $this->say("</TEXTAREA>");
            $this->say("</FORM>");
        }   
    }

    /**
     *Returns an array of 'columnDefinition' objects that
     *will be used to build the actual smart form.
     */ 
    function getTableDef() {
        $this->getTableFormat();
        return $this->tableDef;
    }

    // get table format
    // this will read a description of the table and
    // setup some data structures for later use
    function getTableFormat() {

        //if we already did this, don't do it again :)
        if(!$this->runTableFormat){
             return;
        }

        global $SM_siteManager;

        //load the class for our columnDefinition
        $SM_siteManager->includeLib($this->directive['columnClass']);

        //include the hook if we have one
        if(isset($this->directive['dbHook'])) {
            $className = preg_replace("/\.inc$/","",$this->directive['dbHook']);
            $SM_siteManager->includeLib($className);
        }

	if (empty($this->dbHL[$this->directive['dataBaseID']]))
	   $this->fatalErrorPage("Invalid database handle: ".$this->directive['dataBaseID']);

        $rh = $this->dbHL[$this->directive['dataBaseID']]->query('DESC '.$this->directive['tableName']);
        $this->dbErrorCheck($rh);
        //$res = $this->dbHL[$this->directive['dataBaseID']]->tableInfo($this->directive['tableName']);
    
        //for($i = 0; $i<sizeof($res); $i++) {
        while ($rr = $rh->fetch()) {

            //create a new tableDefinition object
            $columnDef = new $this->directive['columnClass']();
            $columnDef->setTableName($this->directive['tableName']);

            //pass along some directives
            $columnDef->setDbOverRide($this->directive['dbOverRide']);
            $columnDef->setDataField($this->directive['dataField']);
            if (!empty($this->directive['orderBy']))            
                $columnDef->setDirectives(array('orderBy'=>$this->directive['orderBy']));
            if (!empty($this->directive['whereClause']))            
                $columnDef->setDirectives(array('whereClause'=>$this->directive['whereClause']));

            if(isset($className)) {
                $hook = new $className();
                $hook->addDirective('dataBaseID',$this->directive['dataBaseID']);
                $columnDef->setDbHook($hook);
            }

            //configure the object
            $columnDef->config($rr, $this->directive['autoFilter']);
                        
            //*** Tweek the object a little

            // don't let shown size go past max size for this form
            if ($columnDef->getSize() > $this->directive['maxVisibleSize'])
                $columnDef->setSize($this->directive['maxVisibleSize']);
                
            // if we're in remove mode, none of these are required
            if ($this->sType == 'remove')
                $columnDef->setRequired(false);
                        
            //add this object to the array
            $this->tableDef[$columnDef->getName()] = $columnDef;
        }
        //flag that we don't have run it again. 
        $this->runTableFormat=false;
    }    

    /**
     *Sets the array of 'columnDefinition' objects that
     *will be used to build the smart form.
     */
    function setTableDef($newTableDef) {
        $this->tableDef = $newTableDef;
        //if we get a table def, we don't need to build one.
        $this->runTableFormat=false;
    }

    /**
     * Wrapper function for adding a directive to an entity on the smart form.
     * i.e. we have a databse field named myText that dbGui recognizes as
     * a 'textAreaEntity' and we want to make sure that the 'textAreaEntity' 
     * directive 'rows' was set to '15' insted of the default '5'. The call
     * would look like this:
     *
     *  $mod->addEntityDirective('myText','rows','15');
     *
     * @param $entity - Name of the database field we are adding to
     * @param $key - directive key
     * @param $value - directive value
     */
    function addEntityDirective($entity, $key, $val){
        $this->getTableFormat();
        if (!isset($this->tableDef[$entity])) {
            return false;
        }
        $this->tableDef[$entity]->setSfDirectives(array($key=>$val));
        return true;
    }

    /**
     * get a table def entity so you can modify it
     * @param $entity (string) the entity varname
     */
    function getEntity($entity) {
        $this->getTableFormat();
        if (!isset($this->tableDef[$entity])) {
            return false;
        }
        return $this->tableDef[$entity];
    }

    /**
     * wrapper methods for hiding fields
     * @param $eList (array) a list of var names to not show 
     */
    function hideEntity($eList) {
        $this->getTableFormat();
        foreach ($eList as $name) {
            if (isset($this->tableDef[$name])) {
                $this->tableDef[$name]->setDisplay(false);
            }
        }
    }

    /**
     * wrapper function for setting the type of an inputEntity
     * @param $entity (string) the entity name
     
     */
    function setEntityType($entity, $newType) {
        $this->getTableFormat();
        if (!isset($this->tableDef[$entity])) {
            return false;
        }
        $this->tableDef[$entity]->setSfType($newType);
       return true;
    }

    /**
     * Function to add a filter to a field of the smartform.
     *
     *  i.e.:
     *      $mod->addFilter('databaseFieldName','smartFormFilter');
     *  or
     *      $mod->addFilter('databaseFieldName','smartFormFilter', $hashOfDirectives);
     *
     * @param $dbField - name of the database field to apply the filter to
     * @param $filterName - smart form filter to add to this field
     * @param $configureParams - (optional) hash of directives to add to this filter
     *
     */
    function addFilter($dbField, $filterName, $configureParams='') {
        $this->getTableFormat();
        if (!isset($this->tableDef[$dbField])) {
            return false;
        }
        $this->tableDef[$dbField]->setSfFilters($filterName,$configureParams);
        return true;
    }    

}

?>
