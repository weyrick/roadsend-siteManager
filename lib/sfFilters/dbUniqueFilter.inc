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
 *                      This was done so that it would be posible to get a 
 *                     list of directives on the fly. (gollum)
 *
 *       11/24/2002 - added Postrges database compatibility, Eric Andersen
 */

/**
 *
 *                      
 *  input must be unique with respect to values in a table
 *  this filter will FAIL if the value entered for this HTML FORM field by the user is
 *  already in the database under <checkField>, in table <tableName>
 *
 *  for example, use for pre-checking for duplicating userNames or emails when signing up
 *  for membership
 *
 *  directives:
 *      tableName   - the database table to look in
 *      checkField  - the field in the table to check against
 *      whereClause - optional where clause
 *      passThroughVal - optional "original" value. if the current value and the original value
 *                       are equal, the filter passes. this should be used when the filter is
 *                       failing erroneously because it's matching against itself on an edit
 *    
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 */
class dbUniqueFilter extends SM_entityFilter {


    /** setup filter */
    function filterConfig() {            
    

        // require database
        if (empty($this->directive['dataBaseID']))
            $this->directive['dataBaseID'] = 'default';       
        
        if (empty($this->dbHL[$this->directive['dataBaseID']])) {
            $this->fatalErrorPage("attempt to use dbUniqueFilter without a default database connection!");
        }

        $this->directive['tableName']=NULL;
        $this->directive['checkField']=NULL;
        $this->directive['whereClause']=NULL;

    }


    /** apply this filter */
    function filterThink() {

        // if the current data is the same as original, as passed to us in
        // the directive, then we succeed.
        if (isset($this->directive['passThroughVal']) &&
            ($this->data == $this->directive['passThroughVal']))
            return true;
    
        $wC = '';
        if ($this->directive['whereClause'] != '') {
            $wC = 'AND '.$this->directive['whereClause'];
        }


        switch ($this->siteConfig->getVar('db','dbType')) {
        case 'pgsql':
            $SQL = "SELECT \"{$this->directive['checkField']}\" FROM \"{$this->directive['tableName']}\" WHERE \"{$this->directive['checkField']}\"='$this->data' $wC";
            break;
        case 'mysql':
        default:
            $SQL = "SELECT {$this->directive['checkField']} FROM {$this->directive['tableName']} WHERE {$this->directive['checkField']}='$this->data' $wC LIMIT 1";
            break;        
        }

        $rh = $this->dbHL[$this->directive['dataBaseID']]->query($SQL);
        $this->dbErrorCheck($rh);
        return ($rh->rowCount() == 0);

    }

}

?>