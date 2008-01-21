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
* Author(s): gollum
*
*
* Change Log
* 
*       11/20/2001 - Created. Moved this class out of dbGui so that
*                    it would be more flexable and extensible. (gollum)
*
*       11/28/2001 - Modify setSfFilter to accept 2 parameters to make the
*                    code a little cleaner. The first one is the filter name
*                    and the second is optional an array of directives.
*
*       12/4/2001 - Modified class NOT to extend SM_object so that when 
*                   the object gets unserialized, there is no confict
*                   with SM_object properties.
*
*       3/11/2002 - Added a defaultValue var and approprate getter & setter
*                   methods. Also added a check in 'addToSmartForm' so that
*                   if no value is passed, it uses the default. (Gollum)
* 
*       3/12/2002 - Added a forceDbSelect var andapproprate getter & setter
*                   methods. This is so that you have the option of not 
*                   automatically having the dbSelect or dbComboBox entities
*                   turning into text boxes if a viewfield isn't specified or
*                   found. (Gollum).
*
*/

class columnDefinition {

    /** use this as the dataField for a table **/
    var $dataField=NULL;

    /** The dataType of this object**/
    var $dataType=NULL;

    /** Keeps track of alternate databse for linked tables **/
    var $dbOverRide=false;

    /** Configuration for a dbSelectEntity **/
    var $dbSelectConfig = array();

    /** hash of directives passed along **/
    var $directives = array();

    /** default value for this column **/
    var $defaultValue = NULL;

    /** Weather or not to display this column **/
    var $display=true;
    
    /** database specfic hook **/
    var $dbHook=NULL;

    /** if true, Forces a dbSelect or dbComboBox 
          *NOT* to be turned into a text field **/
    var $forceDbSelect = false;

    /** max sixe of an entity **/
    var $maxSize=NULL;

    /** The column name **/
    var $name=NULL;

    /** keep track if we need to quote this column **/
    var $needQuotes=true;

    /** Keep track of the pretty Name of this column **/
    var $prettyName = NULL;

    /** (bool) Is this a required field **/
    var $required = false;

    /** (hash: mixed) List of smart form directives for this column **/
    var $sfDirectives = array();

    /** (hash: ?) List of smart form filters for this column **/
    var $sfFilters = array();

    /** The smart form entity type for this column **/
    var $sfType = 'text';
    
    /** Size of this column **/
    var $size = NULL;

    /** The table name for this column **/
    var $tableName = NULL;

    /**
     * constructor for columnDefinition
     */
    function columnDefinition() {

        //set the default state of some of the vars
    }

    /**
     * Add what's represented by this object
     * to the smart form that gets passed in.
     */
    function addToSmartForm($myForm, $sType, $dbID, $value='') {
                
        // Check if we need to do anything with this one
        if ($this->getDisplay() == false || $this->getSfType() == '')
            return;

        // If the value passed was empty, try to use 
        // the 'defaultValue' if it is set.
        if(empty($value)) {
            $value = $this->getDefaultValue();
        }

        if($sType == 'remove') {
            $myForm->add($this->getName(), $this->getPrettyName(), 'presetText', $this->getRequired(), $value);
            return;
        }

        // Display Name
        if (isset($this->sfDirectives['displayName'])) {
            $displayName = $this->sfDirectives['displayName'];
        } else {
            $displayName = $this->getPrettyName();
        }

        $ie = $myForm->add($this->getName(), $displayName , $this->getSfType(), $this->getRequired(), $value);

        // if this is a dbSelect or dbComboBox, set it up some more
        if (($this->getSfType() == 'dbSelect') || ($this->getSfType() == 'dbComboBox')) {

            // if we aren't required, addNoOption
            if(!$this->getRequired()) {
                $this->sfDirectives['addNoOption'] = true;
            }

            // try to use a viewfield that was specified as a directive for this entity
            if (isset($this->sfDirectives['viewField'])) {
                $viewField = $this->sfDirectives['viewField'];
            }

            // otherwise, try to look it up by guessing at the second field in the table
            if (empty($viewField)) {                
                global $SM_siteManager;

                if (isset($SM_siteManager->dbHL[$dbID])) {
                    $dbH = $SM_siteManager->dbHL[$dbID];
                } elseif (isset($SM_siteManager->dbH)) {
                    $dbH = $SM_siteManager->dbH;
                }
                else {
                    $dbH = NULL;
                    SM_debugLog("no valid database connection to lookup viewfield for $this->name, will change to text",$this);
                }

                if ($dbH) {
                    // try to get the table name from the var
                    $name = $this->getTableName();

                    // if name is still empty, try to get it from the sdDirectives
                    if(empty($name)) {
                        $name = $this->sfDirectives['tableName'];
                        $this->setTableName($name);
                    }

                    //try to get the viewField
                    $SQL = "SHOW FIELDS FROM $name";
                    $rh = $dbH->query($SQL);
                    if ($rh) {
                        $rr = $rh->fetch();
                        // take the second, if we have it
                        if ($rh) {
                            $rr = $rh->fetch();
                            $viewField = $rr['Field'];
                            $this->sfDirectives['viewField'] = $viewField;
                            SM_debugLog("guessing at viewField for $this->name: $viewField (saving for later use)",$this);
                        }
                        else {
                            $viewField = NULL;
                            SM_debugLog("couldn't lookup viewField for $this->name",$this);
                        }
                    }
                    else {
                        $viewField = NULL;
                        SM_debugLog("couldn't lookup viewField for $this->name",$this);
                    }
                }

            }

            // Not a valid foriegn key, just show a text box.
            // Only if we don't want to force this to be a dbSelect or dbComboBox
            if (empty($viewField) && !$this->forceDbSelect) {
                $myForm->remove($this->getName());
                $this->setSfType('text');
                $this->setPrettyName($this->getPrettyName());
                $ie = $myForm->add($this->getName(), $this->getPrettyName(), 'text', $this->getRequired(), $value);
            }

            // Allows the dbSelectEntity to take different directives when needed                
            $dbSelectConfig = $this->getDbSelectConfig();
            if (isset($dbSelectConfig) && is_array($dbSelectConfig) && sizeof($dbSelectConfig)){
                $ie->configure($dbSelectConfig);
            } else {
                $ie->configure(array('dataBaseID'   =>$dbID,
                                     'tableName'    =>$this->getTableName(),
                                     'dataField'    =>$this->getDataField(),
                                     'viewField'    =>$viewField,
                                     'orderBy'      =>$this->getDirective('orderBy'),
                                     'whereClause'  =>$this->getDirective('whereClause'),
                                     'addNoOption'  =>$this->getDirective('addNoOption')
                                     ));
            }

        }

        
        if ($this->getSfType() == 'text') {
            //If we can limit by size (ie, text) do so here
            $ie->configure(array('size'=>$this->getSize(),'maxLength'=>$this->getMaxSize()));
        } 
        elseif ($this->getSfType() == 'date') {
            //If it's a date, set the year to be freeform
            $ie->configure(array('yearSelect'=>false));
        }

        // Configure entity with potential user overriden directives
        $sfDir = $this->getSfDirectives();
        if (sizeof($sfDir))
            $ie->configure($sfDir);

        // for entities with dataBase ID, override it here to point to the
        // database as it will appear once the form is saved
        if (isset($ie->directive['dataBaseID'])) {
            $ie->addDirective('dataBaseID',$dbID);
        }

        // if we have filters to add, do them here
        $filters = $this->getSfFilters();
        if (isset($filters)) {
            foreach ($filters as $fType=>$fArgs) {
                // add the filter
                $ie->addFilter($fType);

                // add arguments if any
                if (is_array($fArgs))
                    $ie->filterList[$fType]->configure($fArgs);
            }
        }

        // if we have one, run the hook for this method
        if(isset($this->dbHook)) {
            $this->dbHook->addToSmartFormHook($this, $ie);
        }

        // commit
        $myForm->commit($this->getName(), $ie);
    }

    /**
     * Takes the result from a 'tableInfo' query
     * and builds this object accordingly
     */
    function config($res, $autoFilter) {
//         $this->setTableName($res['table']);
//         $this->setName($res['name']);
//         $this->setDataType($res['type'], $res['flags'], $autoFilter);
//         $this->setMaxSize($res['len']);
//         $this->setSize($res['len']);
//         (eregi("null" , $res['flags'])) ? $this->setRequired(true) : $this->setRequired(false);
        if (preg_match('/^(\w+)\((\d+)\)/',$res['Type'],$m)) {
            $type = $m[1];
            $size = $m[2];
        }
        else {
            $type = $res['Type'];
            $size = 0;
        }
        // fake these like mysql_field_flags
        $flags = array();
        if (substr($type,0,3) == 'set') {
            $flags[] = 'set';
            $type = 'string';
        }
        if (substr($type,0,4) == 'enum') {
            $flags[] = 'enum';
            $type = 'string';
        }
        $this->setName($res['Field']);
        $this->setDataType($type, join($flags,','), $autoFilter);
        $this->setMaxSize($size);
        $this->setSize($size);
        ($res['Null'] == 'NO') ? $this->setRequired(true) : $this->setRequired(false);
        ($res['Key'] == 'PRI') ? $this->setDisplay(false) : $this->setDisplay(true);
    }

    /**
     * Getter method for $dataField
     */
    function getDataField() {
        return $this->dataField;
    }

    /**
     * Getter method for $dbOverRide
     */
    function getDbOverRide() {
        return $this->dbOverRide;
    }

    /**
     * Getter method for $dbHook
     */
    function getDbHook() {
        return $this->dbHook;
    }

    /**
     * Getter method for $dbSelectConfig
     */
    function getDbSelectConfig() {
        return $this->dbSelectConfig;
    }

    /**
     * Getter method for $defaultvalue
     */
    function getDefaultValue() {
        return $this->defaultValue;
    }

    /**
     * Getter method for a specific directive
     */
    function getDirective($name) {
        if (isset($this->directives[$name]))
            return $this->directives[$name];
        else
            return NULL;
    }

    /**
     * Getter method for all of the $directives
     */
    function getDirectives() {
        return $this->directives;
    }

    /**
     * Getter method for $display
     */
    function getDisplay() {
        return $this->display;
    }

    /*
     * Getter method for $forceDbSelect
     */
    function getForceDbSelect() {
        return $this->forceDbSelect;
    }

    /**
     * Getter method for $maxSize
     */
    function getMaxSize() {
        return $this->maxSize;
    }
    
    /**
     * Getter method for $name
     */
    function getName() {
        return $this->name;
    }

    /**
     * Getter for $prettyName
     */
    function getPrettyName() {

        return $this->prettyName;
    }

    /**
     * Getter method for $required
     */
    function getRequired() {
        return $this->required;
    }

    /**
     * Getter method for $sfDirectives
     */
    function getSfDirectives() {
        return $this->sfDirectives;
    }

    /**
     * Getter method for $sfFilters
     */
    function getSfFilters($filterName='') {
        if($filterName == '')
            return $this->sfFilters;
        else
            return $this->sfFilters[$filterName];
    }

    /**
     * Getter method for $sfType
     */
    function getSfType() {
        return $this->sfType;
    }

    /**
     * Getter method for $size
     */
    function getSize() {
        return $this->size;
    }

    /**
     * Getter method for $tableName
     */
    function getTableName() {
        return $this->tableName;
    }

    /**
     * Setter method for $dataField
     */
    function setDataField($newDataField) {
        $this->dataField = $newDataField;
    }

    /**
     * Setter method for $dataType
     */
    function setDataType ($newDataType, $flags='', $autoFilter=true) {
        /* PEAR types: int, real, date, datetime, time, timestamp, year, string, blob */

        $this->columnType = $newDataType;
    
        //make adjustments based on column type.
        if($newDataType == '' || $newDataType == 'timestamp') {
            $this->setdisplay(false);
            return;
        }
        
        // if it's a link to another table, change it's type to dbSelect
        // we tell this by looking for "_idxNum" on the end of a field name
        if (preg_match("/(\w+)_idxNum\$/",$this->getname(),$m)) {
            global $SM_siteManager;

            if (isset($SM_siteManager->dbHL[$dbID])) {
                $dbH = $SM_siteManager->dbHL[$dbID];
            } elseif (isset($SM_siteManager->dbH)) {
                $dbH = $SM_siteManager->dbH;
            }
            else {
                $dbH = NULL;
                SM_debugLog("no valid database connection to lookup viewfield for $this->name, will change to text",$this);
            }
            // only do this if there are less than 100 rows
            $SQL = "SELECT COUNT(*) AS cnt FROM $m[1]";
            $rh = $dbH->query($SQL);
            if ($rh) {
                $rr = $rh->fetch();
                if ($rr['cnt'] < 1000) {
                    $this->setSfType('dbSelect');
                    $this->setTableName($m[1]);
                    $this->setDataField('idxNum');
            
                    // handle dbOverride on table lookup
                    $overRide = $this->getDbOverRide();
                    if (!empty($overRide)) {
                        $this->setTableName("$overRide.{$m[1]}");
                    }
                }
            }
            
            return;
        }
    
        // deal with blobs
        if($newDataType == 'text') {
//            if(!(eregi('binary',$flags))) {
                $this->setSfType('textArea');
//            } else {
                //since we dont' deal with binary yet,
                // just stick up a text message.
/*                $this->setSfType('rawText');
                $this->setSfDirectives(array('staticText'=>'This is a binary object'));
            }*/
        }

        // if we are one of the following, change needQuotes to false
        if($newDataType == 'int' || $$newDataType='real') {
            $this->setNeedQuotes(false);
        }
    
        // deal with date
        if($newDataType == 'date' || $newDataType == 'datetime') {
            $this->setSfType('date');
        }

        // if we have one, run the hook for this method
        if(isset($this->dbHook)) {
            $this->dbHook->setDataTypeHook($this, $flags);
        }
        
        // try to pick some more appropriate input entities based on it's name            
        // STATE
        if (eregi('state',$this->getName()))
            $this->setSfType('stateList');
    
        // COUNTRY
        if (eregi('country',$this->getName()))
            $this->setSfType('countryList');

        if ($autoFilter) {
            // try to guess some filters to add based on it's name
            // EMAIL
            // FIXME - removed for now (weyrick)
            //if (eregi('email',$this->getName()))
                //$this->setSfFilter('email');
        }
    }

    /**
     * Setter method for $dbOverRide
     */
    function setDbOverRide($newDbOverRide){
        $this->dbOverRide = $newDbOverRide;
    }
    
    /**
     * Setter method for $dbSelectConfig
     */
    function setDbSelectConfig($newDbSelectConfig) {
        $this->dbSelectConfig = $newDbSelectConfig;
    }

    /**
     * Setter method for defaultValue
     */
    function setDefaultValue($newDefaultValue) {
        $this->defaultValue = $newDefaultValue;
    }

    /**
     * Setter method for $directives
     */
    function setDirectives($newDirectives, $overWrite=false) {
        if ($overWrite) {
            unset($this->directives);
        }

        if(!is_array($newDirectives)) {
            $newDirectives = array($newDirectives);
        }

        if(!empty($newDirectives)) {
            $this->directives = array_merge($this->directives, $newDirectives);
        } else {
            $this->directives = $newDirectives;
        }

    }

    /**
     * Setter method for $displayed
     */
    function setDisplay ($newDisplay) {
        $this->display = $newDisplay;
    }

    /**
     * Setter method for $dbhook
     */
    function setDbHook($newHook) {
        if(is_subclass_of($newHook, 'dbGuiBaseHook')) {
            $this->dbHook = $newHook;
        }        
    }

    /**
     * Setter method for $forceDbSelect
     */
    function setforceDbSelect($newForceDbSelect) {
        $this->forceDbSelect = $newForceDbSelect;
    }

    /**
     * Setter method for $maxSize
     */
    function setMaxSize ($newMaxSize) {
        $this->maxSize = $newMaxSize;
    }
    
    /**
     * Setter method for $name
     */
    function setName($newName) {
        $this->name= $newName;
        $this->prettyName = SM_prettyName($newName);
    }

    /**
     * Setter method for $needQuotes
     */
    function setNeedQuotes ($newNeedQuotes) {
        $this->needQuotes = $newNeedQuotes;
    }

    /**
     * Setter method for $prettyName
     */
    function setPrettyName ($newPrettyName) {
        $this->prettyName=$newPrettyName;
    }

    /**
     * Setter method for $required
     */
    function setRequired ($newRequired) {
        $this->required = $newRequired;
    }

    /**
     * consistency wrapper
     */
    function addDirective($key, $val='') {
        $this->setSfDirectives(array($key=>$val));
    }

    /**
     * consistency wrapper
     */
    function configure($cDir) {
        $this->setSfDirectives($cDir);
    }

    /**
     * Setter method for $sfDirectives
     */
    function setSfDirectives($newSfDirectives, $overWrite=false) {
        if ($overWrite) {
            unset($this->sfDirectives);
        }
        
        if(!is_array($newSfDirectives)) {
            $newSfDirectives = array($newSfDirectives);
        }

        if(!empty($this->sfDirectives)) {
           $this->sfDirectives = array_merge($this->sfDirectives, $newSfDirectives);
        } else {
           $this->sfDirectives = $newSfDirectives;
        }
    }

    /**
     * Setter method for $sfFilters
     */
    function setSfFilters($newSfFilter, $newFilterDirectives='', $overWrite=false) {
        if ($overWrite) {
            unset($this->sfFilters);
        }

        if(!is_array($newFilterDirectives)) {
            $newFilterDirectives = array($newFilterDirectives);
        }

        $this->sfFilters[$newSfFilter] = $newFilterDirectives;
    }

    /**
     * Setter method for $sfType
     */
    function setSfType ($newSfType) {
        $this->sfType = $newSfType;
    }

    /**
     * Setter method for $size
     */
    function setSize ($newSize) {
        $this->size = $newSize;
    }

    /**
     * Setter method for $tableName
     */
    function setTableName ($newTableName) {
        $this->tableName = $newTableName;
    }
    
}

?>