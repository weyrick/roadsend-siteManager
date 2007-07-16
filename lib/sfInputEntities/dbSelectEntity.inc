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
 * Change Log
 * 
 *       11/29/2001 - all directives not already initilized were set to 'Null'.
 *                     This was done so that it would be posible to get a 
 *                     list of directives on the fly. (gollum)
 *
 *
 *       1/14/2002 - added group by clause - allows sudo distint lists (pym)
 *
 */

SM_sfLoadEntity('select');      // load our parent


/**
 *
 *  a select box which populates itself from a database pull
 *  INHERITS selectEntity. 
 *  directives:
 *          same as selectEntity
 *          - plus -
 *          
 *          $dataBaseID     - which database connection to use. default is 'default'
 *          $tableName      - table to pull from
 *          $whereClause    - optional where clause to add
 *          $viewField      - field to "show" in select list  (if not set, it will be the second field in the table)
 *          $dataField      - field to use as data in select list
 *          $orderBy        - field(s) to order by
 *          $vfSeparator    - character that will separate multiple view fields in the select list
 * 
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class dbSelectEntity extends selectEntity {
    
    /**
    * make sure we have database connectivity
    */
     
    function entityConfig() {

        if (empty($this->directive['dataBaseID']))
            $this->directive['dataBaseID'] = 'default';       

        parent::entityConfig();

        $this->directive['tableName']   = NULL;
        $this->directive['whereClause'] = NULL;
        $this->directive['viewField']   = NULL;
        $this->directive['dataField']   = NULL;
        $this->directive['orderBy']     = NULL;
        $this->directive['groupBy']     = NULL;
        $this->directive['vfSeparator'] = ', ';
    }

    /**
    * populate the list with the database pull 
    */

    function populateList() {
    
        if (empty($this->dbHL[$this->directive['dataBaseID']])) {
            $this->fatalErrorPage("attempt to use dbSelectEntity without a database connection on id [{$this->directive['dataBaseID']}]!");
        }

        if (($this->directive['tableName'] == '')||($this->directive['dataField'] == '')) {
            $this->debugLog("table or dataField was not set!");
            return;
        }
        
        if (!empty($this->directive['whereClause'])) {
            if (!eregi("WHERE",$this->directive['whereClause']))
                $wC = 'WHERE '.$this->directive['whereClause'];
            else
                $wC = $this->directive['whereClause'];
        }
        else {
            $wC = '';
        }
        
        $vF = $this->directive['viewField'];
        if (empty($vF)) {
            $this->fatalErrorPage("viewField wasn't set");
        }

        // order by viewField by default
        if ($this->directive['orderBy'] != '')
            $oB = 'ORDER BY '.$this->directive['orderBy'];
        elseif ($this->directive['orderBy'] != '*')
            $oB = 'ORDER BY '.$this->directive['viewField'];
        else 
            $oB = '';
        
        // order by viewField by default
        if ($this->directive['groupBy'] != '')
            $gB = ' GROUP BY '.$this->directive['groupBy'];
        else 
            $gB = '';



        // check for a dbOverRide
        if(isset($this->directive['dbOverRide']))
           $dbName = $this->directive['dbOverRide'];

        $SQL = "SELECT {$this->directive['dataField']} AS table_id, {$this->directive['viewField']} FROM {$this->directive['tableName']} $wC $gB $oB";
        
        $rh = $this->dbHL[$this->directive['dataBaseID']]->query($SQL);
        if ($this->dbErrorCheck($rh, $this)) {
            $this->addOption('** bad SQL query **');
            return;
        }

        while ($rr = $rh->fetch(PDO::FETCH_NUM)) {

            $title = '';
            for ($fc=1; $fc < sizeof($rr); $fc++) {
                $title .= $rr[$fc].$this->directive['vfSeparator'];
            }
            $cutVal = strlen($this->directive['vfSeparator']);
            $title = substr($title, 0, -$cutVal);

            $this->addOption($title, $rr[0]);

        }

        $rh = null;        
    }

}

?>
