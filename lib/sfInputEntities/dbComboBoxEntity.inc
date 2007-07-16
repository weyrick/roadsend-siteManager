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

SM_sfLoadEntity('comboBox');        // load our parent


/**
 *
 * a select box with a list of options, plus a text field to
 *  input a new option, populates itself from a database pull
 *  INHERITS comboBoxEntity. 
 *
 *  directives:
 *          same as comboBoxEntity
 *          - plus -
 *           
 *          $dataBaseID     - which database connection to use. default is 'default'
 *          tableName      -- table to pull from
 *          viewField   -- field to use as input
 *                         
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */ 
class dbComboBoxEntity extends comboBoxEntity {
        
    /**
    * setup entity. requires database access 
    */

    function entityConfig() {
    
        parent::entityConfig();

        // setup some defaults
        $this->directive['tableName']       = '';
        $this->directive['viewField']       = '';

        if (empty($this->directive['dataBaseID']))
            $this->directive['dataBaseID'] = 'default';       

    }


    /**
    * populate the list with the database pull
    */

    function populateList() {
        
        if (empty($this->dbHL[$this->directive['dataBaseID']])) {
            $this->fatalErrorPage("attempt to use dbSelectEntity without a database connection on id [{$this->directive['dataBaseID']}]!");
        }        


        if (($this->directive['tableName'] == '')||($this->directive['viewField'] == '')) {
            $this->debugLog("table or viewField  was not set!");
            return;
        }
                    
        // order by
        $oB = 'ORDER BY '.$this->directive['viewField'];
        
        $SQL = "SELECT DISTINCT {$this->directive['viewField']} FROM {$this->directive['tableName']} $oB";
                        
        $rh = $this->dbHL[$this->directive['dataBaseID']]->query($SQL);
        if ($this->dbErrorCheck($rh, false)) {
            $this->debugLog($SQL);
            $this->addOption('** bad SQL query **');
            return;
        }

        while ($rr = $rh->fetch()) {
            $this->addOption($rr[$this->directive['viewField']]);
        }
        $rh = null;        
    
    }
    


}

?>
