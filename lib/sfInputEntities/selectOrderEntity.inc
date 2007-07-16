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

SM_sfLoadEntity('select');      // load our parent


/**
 * an entity for creating a select list that will order an item list
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class selectOrderEntity extends selectEntity {
    
    /** 
    *setup entity
    */
     
    function entityConfig() {

        parent::entityConfig();

        $this->requireJS = true;

        $this->addDirective('size','5');

        $fName = $this->parentFormEntity->directive['formName'];

        $this->sayJS('

function moveModule(o_col, d_col)
{
  o_sl = document.'.$fName.'[o_col].selectedIndex;
  d_sl = document.'.$fName.'[d_col].length;
  if (o_sl != -1 && document.'.$fName.'[o_col].options[o_sl].value > "") {
    oText = document.'.$fName.'[o_col].options[o_sl].text;
    oValue = document.'.$fName.'[o_col].options[o_sl].value;
    document.'.$fName.'[o_col].options[o_sl] = null;
    document.'.$fName.'[d_col].options[d_sl] = new Option (oText, oValue, false, true);
  } else {
    alert("Please select a module first");
  }
}

function orderModule(down, col) 
{
  sl = document.'.$fName.'[col].selectedIndex;
  if (sl != -1 && document.'.$fName.'[col].options[sl].value > "") {
    oText = document.'.$fName.'[col].options[sl].text;
    oValue = document.'.$fName.'[col].options[sl].value;
    if (document.'.$fName.'[col].options[sl].value > "" && sl > 0 && down == 0) {
      document.'.$fName.'[col].options[sl].text = document.'.$fName.'[col].options[sl-1].text;
      document.'.$fName.'[col].options[sl].value = document.'.$fName.'[col].options[sl-1].value;
      document.'.$fName.'[col].options[sl-1].text = oText;
      document.'.$fName.'[col].options[sl-1].value = oValue;
      document.'.$fName.'[col].selectedIndex--;
    } else if (sl < document.'.$fName.'[col].length-1 && document.'.$fName.'[col].options[sl+1].value > "" && down == 1) {
      document.'.$fName.'[col].options[sl].text = document.'.$fName.'[col].options[sl+1].text;
      document.'.$fName.'[col].options[sl].value = document.'.$fName.'[col].options[sl+1].value;
      document.'.$fName.'[col].options[sl+1].text = oText;
      document.'.$fName.'[col].options[sl+1].value = oValue;
      document.'.$fName.'[col].selectedIndex++;
    }
  } else {
    alert("Please select a module first");
  }
}

function xMod(col)
{
  req = "";
  sl = document.'.$fName.'[col].selectedIndex;
  if (sl != -1 && document.'.$fName.'[col].options[sl].value > "") {
    if (req.indexOf(document.'.$fName.'[col].options[sl].value) > -1) {
      alert ("You may not delete a required corporate module.");
    } else {
      if (confirm("This will delete the selected module.")) {
        if (document.'.$fName.'[col].options[sl].value!=".none") {
          if (document.'.$fName.'[col].length==1) {
            document.'.$fName.'[col].options[0].text="";
            document.'.$fName.'[col].options[0].value=".none";
          } else {
            document.'.$fName.'[col].options[sl]=null;
          }
        } else {
          alert("Please select a module first");
        }
      }
    }
  }
}

function check_'.$this->varName.'() {
                     // select all
                     col = "'.$this->varName.'[]";
                     for (var i = 0; i < document.'.$fName.'[col].options.length; i++) {
                         document.'.$fName.'[col].options[i].selected = true;
                     }
                     return true;
}

            ');

    }
   

    /**
    * output the entity
    */

    function entityThink() {
                
        // if no options, dont display
        if ((!isset($this->directive['optionList']))||(!is_array($this->directive['optionList']))) {
            $this->debugLog("output: no options for select to output");        
            return 'N/A';
        }
    
        $this->say("<table border=\"1\">");
        $this->say("<tr><td>");
        $this->say("<select name=\"{$this->varName}[]\" multiple=\"multiple\" size=\"{$this->directive['size']}\">\n");

        foreach ($this->directive['optionList'] as $item) {

            $title = key($item);
            $data  = $item[$title];

            $this->say("<option value=\"$data\" >$title</option>\n");

        }
        $this->say("</select>\n");
        $this->say("</td><td>");

        $this->say("<a href=\"javascript:orderModule(0,'{$this->varName}[]')\">Up</a><br />\n");
        $this->say("<a href=\"javascript:orderModule(1,'{$this->varName}[]')\">Down</a><br />\n");

        $this->say("</td>");
        $this->say("</table>\n");
        
    }


}

?>
