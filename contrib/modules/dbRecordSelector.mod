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
* Author(s): Shannon Weyrick (weyrick@roadsend.com)
*
* Change Log:
*
*       11/22/2002 - add directive "defaultAction"
*       11/19/2002 - add ability to set rNum inVar type
*       03/01/2002 - small directive cleanup from Tim White
*       10/22/2001 - added the ability to have a "dynamicWhereClause" that is an
*                    extra optional filter on the pulldown. it is passed in as
*                    a directive that contians a hash of 'display name'=> 'where clause'
*                    as it's key-value pairs. (gollum)
*
*       10/24/2001 - add the 'dataBaseID' directive and passed it allong to the smart
*                    form elements do that you can use any database connection.
*
*/


/**
 * A module to select a particular record from a table. use in conjuction with
 * mySQLdbGUI to crate a system to edit any MySQL table with SmartForms, by readin
 * the table definition and creating a SmartForm on the fly
 */
class SM_dbRecordSelector extends SM_module {

     /**
      * configure the module. run before moduleThink
      */    
    function moduleConfig() {
        
        // directives       
        $this->addDirective('propagateInVars',false);
        $this->addDirective('tableName');   // table name to operate on     
        $this->addDirective('viewField');   // field to show
        $this->addDirective('dataField','idxNum');   // field to operate with (primary key)
        $this->addDirective('extraOptions');   // other options as array
        $this->addDirective('dataBaseID', 'default');
        $this->addDirective('hiddens', array()); // list of hiddens for this smartForm        
        $this->addDirective('orderBy',''); // Sort order for viewField
        $this->addDirective('whereClause',''); // Conditions for viewField list 
        $this->addDirective('smartFormDirectives');
        $this->addDirective('allowAdd',true);
        $this->addDirective('allowEdit',true);
        $this->addDirective('allowRemove',true);

        // layout
        $this->addDirective('tableBorder','1');
        $this->addDirective('tableWidth','100%');
            
        // default action / record var names
        $this->addDirective('rNumVar','rNum');
        $this->addDirective('rNumVarType','int');
        $this->addDirective('actionVar','sType');

        $this->addDirective('defaultAction','add');

        // incoming variables
        $this->addDirective('allowDynamicWhere',false);  // off by default
        $this->addInVar('dynamicWhereClause','');

        $this->preThinkList[] = 'setInVars';

    }


    /**
     * set invars for rNum and sType, once we give them a chance to override their names
     */
    function setInVars() {

        // add var names
        $this->addInVar($this->directive['rNumVar'],NULL,$this->directive['rNumVarType']);
        $this->addInVar($this->directive['actionVar'],'add','string');

    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
                        
        //build extra where clause based on the
        //dynamic where clause field
        if ($this->directive['allowDynamicWhere'])
            $this->buildWhereClause();

        // generate a list to use to select a record
        $myForm = $this->newSmartForm();
        
        $smartFormDirectives = $this->directive['smartFormDirectives'];        
        if(!empty($smartFormDirectives)) {
            $myForm->configure($smartFormDirectives);
        }
        else {
            // default setup
            $myForm->addDirective('requiredTag',0);
            $myForm->addDirective('requiredStar','');
            $myForm->addDirective('controlsOnRight',true);
        }
        
        // hiddens
        foreach ($this->directive['hiddens'] as $key => $val) {
            $myForm->addHidden($key, $val);
        }

        // select list
        $sel = $myForm->add($this->directive['rNumVar'],'Select Record','dbSelect',true,$this->getVar($this->directive['rNumVar']));
        $sel->configure(array(  'dataBaseID'    =>$this->directive['dataBaseID'],
                                'tableName'     =>$this->directive['tableName'],
                                'dataField'     =>$this->directive['dataField'],
                                'viewField'     =>$this->directive['viewField'],
                                'orderBy'       =>$this->directive['orderBy'],
                                'whereClause'   =>$this->directive['whereClause']));
        
        //raido button options
        $action = $this->getVar($this->directive['actionVar']);
        if (empty($action)) {
            $action = $this->directive['defaultAction'];
        }
        $st = $myForm->add($this->directive['actionVar'],'Action','radio',true,$action);

        if($this->directive['allowAdd'])
            $st->addOption('add');
        if($this->directive['allowEdit'])
            $st->addOption('edit');
        if($this->directive['allowRemove'])
            $st->addOption('remove');

        if (!empty($this->directive['extraOptions'])) {
            $st->addDirective('optionBreak',"<BR>");
            foreach($this->directive['extraOptions'] as $newOption){
                $st->addOption($newOption);
            }
        }

        //dynamic where clause.
        if(!empty($this->directive['dynamicWhereOptions'])) {
            $check = $myForm->add('dynamicWhereClause','Aditional Filters','checkBox',true,$this->getVar('dynamicWhereClause'));
            $check->addDirective('optionBreak',true);

            $options = $this->directive['dynamicWhereOptions'];
            foreach($options as $key=>$val) {
                $check->addOption($key,urlencode($val));
            }                                               
        }

        // run form     
        $myForm->runForm();
        $this->say($myForm->output("Select"));

    }
    
    /**
     * This function checks to see if the 'dynamicWhereClause' is set and 
     * if it is, it adds whatever is in the dynamicWhereClause to what ever
     * was already in the whereClause directive.
     */
    function buildWhereClause() {
        //get the var
        $where = $this->getVar('dynamicWhereClause');
        //make sure it is set and not empty
        if (!empty($where)) {
            if (is_array($where)) {
                //if it's an array, loop thru each element
                //and add them on.
                foreach($where as $item) {
                    if(empty($this->directive['whereClause'])) {
                        $this->directive['whereClause'] = urldecode($item);
                    } else {
                        $this->directive['whereClause'] .= " AND " . urldecode($item);
                    }
                }
            } else {
                //otherwise just add it to what we've already got.
                if(empty($this->directive['whereClause'])) {
                    $this->directive['whereClause'] = urldecode($where);
                } else {
                    $this->directive['whereClause'] .= " AND " . urldecode($where);
                }
            }
        }

    }

}

?>
