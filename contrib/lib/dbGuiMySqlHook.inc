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
 * Author(s): gollum
 *
 *
 * Change Log:
 *      10/30/2001 - Created. Mysql implemntation of the dbGuiBaseHook
 *
 *      11/20/2001 - modified to use the new 'includeLib' function. (gollum)
 *
 */

//load the base class
global $SM_siteManager;
$SM_siteManager->includeLib('dbGuiBaseHook.inc');

class dbGuiMySqlHook extends dbGuiBaseHook {

    /** do we allow multiple? **/
    var $multiple = false;

    /** keep track of the set elements **/
    var $set = NULL;

    /**
     * class constructor
     */
    function dbGuiMySqlHook () { 

        //call the dbGuiBaseHook's constructor.
        parent::dbGuiBaseHook();

        // add a directive for dataBaseID
        $this->addDirective('dataBaseID','default');

        // set the 'className'
        $this->setClassName('dbGuiMySqlHook');

    }

    /**
     * Over-ridden from base class. Slightly modifies the 
     * smartform to add sets. 
     */
    function addToSmartFormHook($cDef, $ie) {

        if (isset($this->set) && is_array($this->set)) {
            foreach ($this->set as $dI)
                $ie->addOption($dI);

            // allow multiple?
            if ($this->multiple)
                $ie->configure(array('multiple'=>true));
        }

    }

    /**
     * Over-ridden from base class. Looks for the MysSQL specfic
     * data types of 'enum' and 'set'. modifies the column object
     * accordinglly
     */
    function setDataTypeHook($cDef, $flags) {
        if (eregi('enum', $flags) || eregi('set', $flags)) {
            $SQL = "SHOW FIELDS FROM " . $cDef->getTableName();
            $rh = $this->dbHL[$this->directive['dataBaseID']]->query($SQL);
            SM_dbErrorCheck($rh, $this);
    
            // loop through each field and gather information
            while ($rr = $rh->fetch()) {
                if ($rr['Field'] == $cDef->getName()) {
                    $type = $rr['Type'];
                    break;
                }
            }

            (preg_match("/\\('(.+)'\\)/",$type,$m)) ? $mySet = $m[1] : $mySet=0;

            if(!$mySet) return;

            $this->set = explode("','",$mySet);
            $cDef->setSfType('select');

            if(eregi('set', $flags))
                $this->multiple = true;

        }

    }

}

?>
