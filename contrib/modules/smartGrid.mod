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
* Change Log:
*
*      2/15/2002 - Added a directive and supporting code to limit
*                   the max number of pages returned from a query.
*                   The default is 150 pages. (Gollum)
*
*      2/18/2002 - Changed the way the table "header" row was built and
*                   the way the data was retrieved so that it wouldn't
*                   break if the select had 'table.column' (Gollum)
*
*      2/19/2002 - Added support for links on data in the grid field. The
*                   link can either be pulled from another column in the
*                   table or to a 'redirect' page. Also made changes to
*                   the way the 'viewFields' directive is handled. This is
*                   only an issue if you are doing a join and 2 of the resulting
*                   columns have the same name. one of the columns must
*                   be aliased. (Gollum)
*
*       3/5/2002 - Added directive 'showDataField' to flag weather ot not to show
*                   the dataField int the output of the table. also set the default
*                   dataField to idxNum. (Gollum)
*
*
* TODO:
*   - Add the ability to have links for multiple columns insted of just one column.
*
*/

/**
 * a skeleton module. extends base SM_module class.
 *
 */
class smartGrid extends SM_module {

    /** Query to get the total row count **/
    var $countSQL = '';

    /** hash to keep track of column configuration options **/
    var $columnConfig = array();
     
    /** Holds the actual query **/
    var $SQL = '';

    /** total number of rows form our query **/
    var $total = NULL;

     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        // Main options
        $this->addDirective('outputInSpan',false);
        $this->addDirective('outputInTable',false);

        // Database Options
        $this->directive['dataBaseID']  = 'default';
        $this->directive['tableName']   = NULL;
        $this->directive['dataField']   = 'idxNum';
        $this->directive['linkField']   = NULL;
        $this->directive['viewFields']  = NULL;
        $this->directive['whereClause'] = NULL;
        $this->directive['orderBy']     = NULL;
        $this->directive['groupBy']     = NULL;

        // Formating options
        $this->directive['headerClassTag']      = NULL;
        $this->directive['footerClassTag']      = NULL;
        $this->directive['columnHeaderClassTag']= NULL;      // use link style

        $this->directive['normalClassTag1']     = NULL;
        $this->directive['normalClassTag2']     = NULL;
        $this->directive['rowColorAlt1']        = '#FFFFFF';
        $this->directive['rowColorAlt2']        = '#CCCFCC';
        $this->directive['headerBgColor']       = '#0E7B51';
        $this->directive['tableWidth']          = 700;
        $this->directive['tableBorder']         = 1;
        $this->directive['tableCellPadding']    = 5;

        $this->directive['showOrderLink']       = true;
        $this->directive['showDataField']       = false;
        $this->directive['maxPerPage']          = 25;
        $this->directive['maxPages']            = 150;

        $this->directive['linkRedirectPage']    = NULL;
        $this->directive['columnToLink']        = NULL;
        $this->directive['gridLinkClassTag']    = NULL;
        $this->directive['gridLinkExtraTag']    = NULL; //Any extra stuff for the link tag
        
        // Header & footer
        $this->directive['showHeader']       = true;
        $this->directive['showFooter']       = true;

        // In and Out vars
        $this->addOutVar('sColumn','','string');
        $this->addOutVar('start',0,'int');

        // Pre-think list
        $this->preThinkList[] = 'buildSQL';

    }

     /**
      * This function actually runs the SQl query and generates the 
      * resulting grid. 
      */
    function moduleThink() {
    
        // Do some setup
        if(isset($this->directive['columnHeaderClassTag'])){
            $columnHeaderClassTag = " class='{$this->directive['columnHeaderClassTag']}'";
        }   else {
            $columnHeaderClassTag = "";
        }

        $start = $this->getVar('start');
        if(empty($start))
            $start =0;

        // Get the reslts      
        $max = $this->directive['maxPerPage'] * $this->directive['maxPages'];
        if($start > $max)
            return;

        $rh = $this->dbHL[$this->directive['dataBaseID']]->limitQuery($this->SQL, $start, $this->directive['maxPerPage']);
        SM_dbErrorCheck($rh, $this);

        // If they want it, build & show the header
        if($this->directive['showHeader'])
            $this->saybr($this->buildHeader($start));

        // Start the main table
        $this->say("<table width=\"{$this->directive['tableWidth']}\" border='{$this->directive['tableBorder']}' cellpadding='{$this->directive['tableCellPadding']}'>\n");

        // Load the header line of the table.
        $this->say("<tr bgcolor={$this->directive['headerBgColor']}{$columnHeaderClassTag}>\n");

        $ft = true;

        // Get the data for the rows
        while($rr = $rh->fetch()) {

            if($ft) {

                $keys = array_keys($rr);
                foreach ($keys as $column) {

                    //skip certian columns
                    if(($column == $this->directive['dataField'] && !$this->directive['showDataField'])|| $column == $this->directive['linkField']){
                        continue;
                    }

                    // Check to see if we need to change the atual display name
                    if(!empty($this->columnConfig[$column]['display'])) {
                        $name = $this->columnConfig[$column]['display'];
                    } else {
                        $name = SM_prettyName($column);
                    }

                    // Check to see if we want to set a width
                    if(!empty($this->columnConfig[$column]['width'])) {
                        $width = " width=\"{$this->columnConfig[$column]['width']}\"";
                    } else {
                        $width = "";
                    }

                    // Output the table cell
                    if($this->directive['showOrderLink']) {
                        $this->say("<td{$width}>" .$this->hLink($_SERVER['PHP_SELF']."?sColumn=$column&start=0",$name) . "</td>");
                    } else {
                        $this->say("<td{$width}>{$name}</td>");
                    }
                    
                }
                $this->say("</tr>\n");
                $ft = false;

            }


            //set up the class tag to be initally empty
            $normalClassTag = "";            

            // alternate row color
            if(!empty($this->directive['rowColorAlt1'])){
                if ($bgColor == $this->directive['rowColorAlt1'] && !empty($this->directive['rowColorAlt2'])){

                    $bgColor = $this->directive['rowColorAlt2'];

                    // If we have a style, use it
                    if(isset($this->directive['normalClassTag2'])){
                        $normalClassTag = " class='{$this->directive['normalClassTag2']}'";
                    }

                } else {

                    $bgColor = $this->directive['rowColorAlt1'];

                    // If we have a style, use it.
                    if(isset($this->directive['normalClassTag1'])){
                        $normalClassTag = " class='{$this->directive['normalClassTag1']}'";
                    }

                }
            }

            // Display the table cell
            $this->say("<tr bgcolor='$bgColor'{$normalClassTag}>");
            foreach ($rr as $name=>$data) {


                //skip certian columns
                if(($name == $this->directive['dataField'] && !$this->directive['showDataField']) || $name == $this->directive['linkField']){
                    continue;
                }

                // Put a blank space in an empty column
                if(empty($data))
                    $data = "&nbsp;";

                // Check to see if we need to set the width
                if(!empty($this->columnConfig[$name]['width'])) {
                    $width=" width=\"{$this->columnConfig[$name]['width']}\"";
                } else {
                    $width="";
                }

                // Check to see if we need to add a prefix
                if(!empty($this->columnConfig[$name]['prefix'])) {
                    $prefix="{$this->columnConfig[$name]['prefix']}";
                } else {
                    $prefix="";
                }

                // Check to see if we need to add a postfix
                if(!empty($this->columnConfig[$name]['postfix'])) {
                    $postfix="{$this->columnConfig[$name]['postfix']}";
                } else {
                    $postfix="";
                }

                // Check to see if we need to add a style
                if(!empty($this->columnConfig[$name]['style'])) {
                    $style=" class=\"{$this->columnConfig[$name]['style']}\"";
                } else {
                    $style="";
                }

                // Display the cell
                if($this->directive['columnToLink'] == $name) {

                    if (!empty($this->directive['linkField'])) {
    
                        // Create a link for the data based on the 'linkField'
                        $field = $this->directive['linkField'];

                        // Try to strip everything before the dot only if we couldn't
                        // find the element in the hash (i.e. try to remove table name)
                        if((empty($rr[$field])) && ($loc = strpos($field, "."))){
                            $field = substr($field,$loc+1,strlen($field));
                        }
                        $link = $rr[$field];

                    } else if (!empty($this->directive['linkRedirectPage'])) {
    
                        // Create a link to the 'redirect' page
                        $field = $this->directive['dataField'];

                        // Try to strip everything before the dot only if we couldn't
                        // find the element in the hash (i.e. try to remove table name)
                        if((empty($rr[$field])) && ($loc = strpos($field, "."))){
                            $field = substr($field, $loc+1, strlen($field));
                        }
                        $link = $this->directive['linkRedirectPage']."?{$this->directive['dataField']}={$rr[$field]}";

                    }
    
                    // Output the link
                    $this->say("<td{$width}{$style}>".$this->hLink($link,"{$prefix}{$data}{$postfix}",$this->directive['gridLinkClassTag'],$this->directive['gridLinkExtraTag'])."</td>");

                } else {

                    // Plain old boring default ;)
                    $this->say("<td{$width}{$style}>{$prefix}{$data}{$postfix}</td>");

                }

            }

            $this->say("</tr>");
        }
        $this->saybr("</table>");
        $rh = null;

        // If they want it, build & show the footer
        if($this->directive['showFooter'])
            $this->saybr($this->buildFooter($start));

    }


    /**
     *
     */
    function buildFooter($start) {

        if(empty($this->total)) {
            // Get the actual total numbers of matching rows.
            $rh2 = $this->dbHL[$this->directive['dataBaseID']]->query($this->countSQL);
            SM_dbErrorCheck($rh2, $this);
            $rr2=$rh2->fetch();
            $this->total = $rr2[key($rr2)];
        }

        if($this->total < $this->directive['maxPerPage']){
            //nothing to do here
            return;
        }

        if(isset($this->directive['headerClassTag'])){
            $headerClassTag = "class='{$this->directive['footerClassTag']}'";
        }   else {
            $headerClassTag = "";
        }

        $footer = "<table width=\"{$this->directive['tableWidth']}\"><tr><td align=center>Pages \n";

        // check to make sure this isn't out of control
        if(($this->total/$this->directive['maxPerPage']) <= 20){

            // Count by ones
            $pageNum=1;
            for ($i=0; $i<$this->total; $i+=$this->directive['maxPerPage']) {
                if($i == $start) {
                    $footer.=" ".$pageNum++;
                } else {
                    $footer .= " " . $this->hLink($_SERVER['PHP_SELF'] . '?start=' . $i, $pageNum++);
                }
            }
            
        } else {

            $max = $this->directive['maxPerPage'] * $this->directive['maxPages'];
            if($max < $this->total) {
                $end = $max-1;
            } else {
                $end = $this->total;
            }

            //count by 5's up until the last one.
            $pageNum=1;
            for ($i=0; $i<$end; $i+=($this->directive['maxPerPage']*5)) {

                if($i == $start) {
                    $footer.=" ".$pageNum;
                } else {
                    $footer .= " " . $this->hLink($_SERVER['PHP_SELF'] . '?start=' . $i, $pageNum);
                }
                $pageNum += 5;
            }

            // if there is less than 5, only go to the last page.
            $last = $i-($this->directive['maxPerPage']*5);
            $diff = floor(($end % $last)/$this->directive['maxPerPage']);
            
            if($diff > 0) {
                $newStart = ($last+($this->directive['maxPerPage']*$diff));
                if($start == $newStart) {
                    $footer .= " " . ($pageNum-5+$diff);
                } else {
                    $footer .= " " . $this->hLink($_SERVER['PHP_SELF'] . '?start=' . $newStart, ($pageNum-5+$diff));
                }
            }

        }

        $footer .= "</td></tr></table>";
        return $footer;

    }

    /**
     * This Function generates the code for the "header" of the grid
     * and returns it as a string of HTML code. This function can be
     * overridden to change the look and feel of the header. In addition,
     * the directive 'showHeader' can be turned on or off to set wether 
     * it actually gets displayed.
     *
     * @param $start - The starting point for this page
     * 
     * @return $header - string containing the HTML to output
     */
    function buildHeader($start) {

        if(empty($this->total)) {
            // Get the actual total numbers of matching rows.
            $rh2 = $this->dbHL[$this->directive['dataBaseID']]->query($this->countSQL);
            SM_dbErrorCheck($rh2, $this);
            $rr2=$rh2->fetch();
            $this->total = $rr2[key($rr2)];
        }

        if(isset($this->directive['headerClassTag'])){
            $headerClassTag = "class='{$this->directive['headerClassTag']}'";
        }   else {
            $headerClassTag = "";
        }

        $header = "<table width=\"{$this->directive['tableWidth']}\">\n";
        $header .= "<tr $headerClassTag><td colspan=3>$this->total matches were found for your search<br/>";
        
        //make sure we don't get too ridiculous
        $max = $this->directive['maxPerPage'] * $this->directive['maxPages'];
        if($max < $this->total) {
            $header .="\nOnly Retrieving first $max results<br/>";
        }

        $header .="<br/></td></tr>\n";


        //if there were no matches, that's all she wrote
        if($this->total == 0) {
            $header = "</table>\n";
            return $header;
        }

        // Caculate the "end" of this page.
        $end = $start + $this->directive['maxPerPage'];

        // Make sure the "end" is never more than the actual toatl.
        if($end > $this->total)
            $end = $this->total;

        $header .= "<tr $headerClassTag><td>";
        // If we're not on the first page (i.e. start=0) then have a 'prev' link
        if($start > 0){
            $header .= $this->hLink($_SERVER['PHP_SELF']."?start=".($start - $this->directive['maxPerPage']),"prev");
        } else {
            $header .= "&nbsp;";
        }
        
        $header .= "</td>\n<td align=center>Displaying matches " . ($start+1) . " to $end</td>\n<td align=right>";

        // If we are not on the last page, show a 'next' link
        if($end < $this->total && $end < $max) {
            $header .= $this->hLink($_SERVER['PHP_SELF']."?start=$end","next");
        } else {
            $header .= "&nbsp;";
        }

        $header .= "</td>\n</tr></table>\n";

        return $header;

    }

    /**
     * This function handles the actual building of the SQL query
     * that are going to be executed to generate the grid, as well
     * as the "SELECT count(*)" query for totals. This function is
     * in the pre-think list and always runs befor the think.
     */
    function buildSQL() {       

        // Start the inital countSQL 
        $this->countSQL = "SELECT count(*) FROM {$this->directive['tableName']} ";

        // Start the regular SQL
        if(!empty($this->directive['viewFields']) && is_array($this->directive['viewFields'])) {

            // Build the list
            $cols = join(",",$this->directive['viewFields']);

            // Make sure the datafield is selected if we have
            // a re-direct page defined.
            if (!empty($this->directive['linkRedirectPage']) && isset($this->directive['dataField'])){
                $cols .= ",{$this->directive['dataField']}";
            }

            // Make sure the linkField is selected if we have it.
            if (!empty($this->directive['linkField'])) {
                $cols .= ",{$this->directive['linkField']}";
            }

            $this->SQL = "SELECT $cols FROM {$this->directive['tableName']} ";

        } else {

            // Otherwise, selecte everything
            $this->SQL = "SELECT * FROM {$this->directive['tableName']} ";

        }        

        // Where clause
        if(!empty($this->directive['whereClause'])) {

            $this->SQL .= "WHERE {$this->directive['whereClause']} ";
            $this->countSQL .= "WHERE {$this->directive['whereClause']} ";

        }

        // Group by
        if(!empty($this->directive['groupBy'])) {

            $this->SQL .= "GROUP BY {$this->directive['groupBy']} ";
            $this->countSQL .= "GROUP BY {$this->directive['groupBy']} ";

        }
        
        // Order by
        $orderBy = $this->getVar('sColumn');
        if(!empty($orderBy)) {
            $this->SQL .= "ORDER BY $orderBy ";
        } else if (isset($this->directive['orderBy'])) {
            $this->SQL .= "ORDER BY {$this->directive['orderBy']} ";
        }

    }

    /**
     * Set the prefix text for a column
     *
     * @param $columnName - the name of the column
     * @param $prefix - value to prefix to the column data
     *
     */
    function setColumnPrefix($columnName, $prefix) {
        $this->columnConfig[$columnName]['prefix'] = $prefix;
    }

    /**
     * Set the postfix text for a column
     *
     * @param $columnName - name of the column
     * @param $postfix - value to append to the column data
     *
     */
    function setColumnPostfix($columName, $postfix) {
        $this->columnConfig[$columnName]['postfix'] = $postfix;
    }

    /**
     * Set the column CSS class
     *
     * @param $columnName - name of the column
     * @param $style - the CSS class to set for this column
     *
     */
    function setColumnStyle($columnName, $style) {
        $this->columnConfig[$columnName]['style'] = $style;
    }

    /**
     * Set the Column table width
     *
     * @param $columnName - name of the column
     * @param $width - width to use for this column
     *
     */
    function setColumnWidth($columnName, $width) {
        $this->columnConfig[$columnName]['width'] = $width;
    }

    /**
     * Set the display name for this column insted of using the
     * column name from the database.
     *
     * @param $columnName - name of the column
     * @param $name - name to display for the column
     *
     */
    function setColumnDisplayName($columnName, $name){
        $this->columnConfig[$columnName]['display'] = $name;
    }
    
}

?>
