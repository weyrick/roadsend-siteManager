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
* Author(s): pym  -- pym@roadsend.com
*
*
* NOTE: this module is deprecated, use ngMenu for new code instead
*
*
* USE: Load the menu module into with  directive script:
*      $mod1 = $SM_siteManager->loadModule('menu');
*      add  each items to the menu:
*      $mod1->addItem($id,$title,$parent,$link,$tabOrder,$accessReq)
*   
*/


/**
 * each item added to a new menu creates a new menuItem
 * $ID MUST BE UNIQUE FOR EACH ITEM.
 * 
 *
 */

class menuItem {
    
    /** parameter that can be passed with the add item function */
    /** (int) identifier - REQUIRED */
    var $ID = NULL;            
    /** (string) displayed text  - REQUIRED */
    var $title = '';         
    /** (int) identifier of parent  - DEFAULT 0 */
    var $parent = NULL;
    /** (int) override order item will disply in  DEFAULT end   */
    var $tabOrder = NULL;      
    /** (sting) the url of the link - blank or php_self will sue $PHP_SELF */ 
    var $link = '';         
    /** (int) minimum user level to show item - DEFAULT 0 , 1 members only */
    var $accessReq = 0;     
    
    /** created during the build menu phase */
    /** (int) number of parents until top of menu */  
    var $level;
    /** (array) children (type menuItem) of the item*/
    var $childList;

    /** a varable to keep track of header items. */
    var $isHeader;

}

class _menu extends SM_object {
    /** (hash)  the menuItems as they were imported */
    var $items;
    /** (string) required used to identify this menu from other */ 
    var $name;
    /** (hash) sorted hash of menu items with parent set to 0 created */
    var $parents;           
    /** (string) temp buffer for output */
    var $displayMenu = '';       
    /** (object) the menu item selected by user or default */
    var $selectedItem;      
    /** (array )an list of parents by ID for the selected item ensure it is displayed */
    var $path;                 
    /** (string)prefix of the calling module */    
    var $modulePrefix;
    /** (int)The access level (1) is assigned to members, */
    var $memberAccessLevel=0;
    /** (int) holds the level for the   last element displayed */
    var $_oldLevel;

    /**
     * Constructor Function
     * Sets module and default related information for the class
     * 
     * @param modulePrefix
     * @return nothing
     */
     function _menu(&$modulePrefix) {

        // configure
        $this->_smoConfigure();

        $this->modulePrefix = $modulePrefix;
        $this->_oldLevel =  0;
        
        // Menu item access
        // If the user is a logged in member give them access 1
        // If you have an accessLevel field in your members table, this can
        // use that: just set the memberAccessLevel to the member's access level instead of 1
        if ($this->sessionH->isMember()) {
            $this->memberAccessLevel = 1;
        }

        // defaults
        $this->directive['menuLink'] = '';
        $this->directive['menuLinkSelected'] = '';

    }

     /**
      *  Add an item into the items hash
      *
      * @param id (int) Unique identifier REQUIRED
      * @param title (string) displayed text REQUIRED
      * @param parent (int) id of items parent - 0 by default or for top level
      * @param link (string) url of item - $PHP_SELF by default
      * @param tabOrder (int) override listed order - bottom by default
      * @param accessReq (int) member access requirement to view item - 1 for member 
      * @return nothing
      */
    function addItem($id,$title,$parent=0,$link='',$tabOrder=99,$accessReq=0) {
        
        $this->items[$id] = new menuItem;
        $this->items[$id]->ID              = $id;
        $this->items[$id]->title           = $title;
        $this->items[$id]->parent          = $parent;
        $this->items[$id]->link            = $link;
        $this->items[$id]->tabOrder        = $tabOrder;
        $this->items[$id]->accessReq       = $accessReq;
        $this->items[$id]->isHeader        = false;
    }

    /**
     *
     *
     */
    function addHeader($id, $title, $parent=0, $tabOrder=99, $accessReq=0) {
        $this->items[$id] = new menuItem;
        $this->items[$id]->ID              = $id;
        $this->items[$id]->title           = $title;
        $this->items[$id]->parent          = $parent;
        $this->items[$id]->tabOrder        = $tabOrder;
        $this->items[$id]->accessReq       = $accessReq;
        $this->items[$id]->isHeader        = true;
    }

    /**
     * Adds a menu item selected by the user or the default.
     * Ensures it is displayed in the menu, regarless of the number of levels
     * that are required to delve down
     * @param id (int) An identifier for an item
     * @return nothing
     */
    // Get the selected item from a passed ID
    function setSelectedItem($id) {

        // check to make certain it is found
        $this->selectedItem ="";
        $this->selectedItem = $this->items[$id];

        // also set the path to parent
        // add selected item to the path
        $pathItem = $this->selectedItem;
        $pathLevel = $pathItem->parent;
        $this->path[] = $pathItem->ID;

        // add each parent of the selected item until the level of the parent = 0
        while ($pathLevel > 0) {
            $pathItem = $this->items[$pathItem->parent];
            $this->path[] = $pathItem->ID;
            $pathLevel = $pathItem->parent;
        }

    }
    
    /**
     * Gets the items display level
     *
     * @param id (int) An identifier for an item
     * @return the items level (number of elements to a top level parent)
     */
    function getItemLevel($id) {

        $pathItem = $this->items[$id];
        $pathLevel = $pathItem->parent;
        $level = 0;
        // add each parent of the selected item until the level of the parent = 0
        while ($pathLevel > 0) {
            $pathLevel = $this->items[$pathLevel]->parent;
            $level++;
            if ($level == 99) SM_fatalErrorPage("menu::getItemLevel level > 99, endless loop detected");
        }
        return $level;
    }


    /**
     * Build a sorted array of parent items
     *
     * @param none
     * @return hash of top level parent items
     */
    function getParents(){
        foreach ($this->items as $id) {
            // if this is a parent add it
            if ($id->parent != 0) continue;
            $this->parents[] = $id;
        }
        
        // sort the list of parents by tab order
        if (empty($this->parents)) contine;
        uasort($this->parents,create_function('$a,$b','if ($a->tabOrder == $b->tabOrder) return 0 ; return ($a->tabOrder < $b->tabOrder) ? -1 : 1;'));
        
        // an object of menuItems
        return $this->parents;
    }

    /**
     * Build a sorted array of child items
     *
     * @param node (int) An identifier for an item 
     * @return nothing
     */
    function getChildren($node){

        foreach ($this->items as $id) {
            // if this is a child of the selected node add it
            if ($id->parent != $node) continue;

            // if there is no access to the item contine
            if ($id->accessReq > $this->memberAccessLevel) continue;

            // don't go beyond a certain level unless this is the selected item or
            // in the path to the selected item
            if (($id->level > $this->directive['maxLevel']) && (!(in_array($node,$this->path)))) continue;


            // add the child to the childList of its parent             
            $this->items[$node]->childList[] = $id;
            
            // Sort the list by tabOrder (add a directive to sort as string by title)
            uasort($this->items[$node]->childList,create_function('$a,$b','if ($a->tabOrder == $b->tabOrder) return 0 ; return ($a->tabOrder < $b->tabOrder) ? -1 : 1;'));
            
            // get children of child
            $this->getChildren($id->ID);
        }

        return true;

    }

    /**
     * Calls functions of the menu class to build a sorted and complete object.
     * @param none
     * @return nothing
     */
    function build(){
        
        if (empty($this->items)) {
            $this->displayMenu = "No items were added";
            $this->completePage();
            return;
        }
        

        // add item level to each element
        foreach($this->items as $element) {
            $this->items[$element->ID]->level = $this->getItemLevel($element->ID);
        }


        // Get a (sorted) list of ALL parent items
        $topLevel = $this->getParents();
        
        // For each parent, get a recursive list of children
        foreach($topLevel as $element){
            $this->getChildren($element->ID);

            // refresh parent so that children are part of the object
            $realElement = $this->items[$element->ID];
            $this->display($realElement);
        }
    }

    /**
     * Add menuItems with formating into the output buffer that will be displayed
     *
     * @param $element (object) of type menuItem
     * @return nothing
     */
    function display($element){

        // Set link (Alert: requires PHP 4.08 now) 
        $link = $element->link;
        if (($link == 'php_self') || ($link == ''))
            $link = $_SERVER['PHP_SELF'];
        
        // If the link has parameters keep them intact
        if (! preg_match('/^http/',$link)) {        
            if (!(strpos($link,"?") === false)) {
                $link .= "&SM_menuID=".$element->ID;
            } else {
                $link .= "?SM_menuID=".$element->ID;
            }
            $target = '';
        }        
        else
            $target = 'TARGET="_new"';

        // set an identifier for menu (not required)
        if (!(empty($this->directive['menuIDVar']))) {
            $link .= "&".$this->directive['menuIDVar']."=".$element->ID;;
        }


        // Set class style and selected prefix 
        if ($element->ID == $this->selectedItem->ID) {
            
            // Set Class
            $class = $this->directive['menuLinkSelectedClass'];
            
            // Set Selected prefix for each level
            if(isset($this->directive['selectedMenuPrefix_'.$element->level])) {       
                $prefix = $this->directive['selectedMenuPrefix_'.$element->level];
            }
            elseif (empty($prefix)) {
                if (isset($this->directive['selectedMenuPrefix']))
                    $prefix =  $this->directive['selectedMenuPrefix'];            
            }
        
        } else
            $class = $this->directive['menuLinkClass'];
        

        // make lower case
        $menuStyle = strtolower($this->directive['menuStyle']);
         
        // Make certain spaces are positive
        ($this->directive['menuSpaces'] > 0) ? $menuSpaces =  $this->directive['menuSpaces'] : $menuSpaces=1;
         

        // Set the prefix for non selected icons
        if (empty($prefix)) {       
            if(isset($this->directive['menuPrefix_'.$element->level]))        
                $prefix = $this->directive['menuPrefix_'.$element->level];

            elseif (empty($prefix))
                $prefix =  $this->directive['menuPrefix'];
        
            else
                $prefix = '';
        }




         
         switch ($menuStyle) {
         case "sp":
            $startIcon =  str_repeat("&nbsp;",($element->level) * $menuSpaces);
            $startIcon .= $prefix;
            
            $endIcon = "<BR>";
            break;
         case "ul":
         case "ol":

            if ($element->tabOrder == 1){
               $startIcon = "<".$menuStyle.">\n";
               
            }
            
            if ($element->level < $this->_oldLevel) {
               $startIcon = "</".$menuStyle.">\n";
            }
            
            $startIcon .=  "<LI>";
            $this->_oldLevel =  $element->level;

            break;


         case "nb";
             $startIcon= $prefix;
             $endIcon="&nbsp";
             break;

         default:
              $startIcon = "";
              $endIcon = "<BR>";
        }
        
        if($element->isHeader) {
            $class = $this->directive['menuHeaderClass'];
            $this->displayMenu .= "<SPAN CLASS='$class'>$startIcon". $element->title . "</SPAN>$endIcon";
        } else {        
            $this->displayMenu .= "$startIcon". $this->sessionH->hLink($link , $element->title, $class, $target)."$endIcon";
        }
        
        if ((!isset($element->childList))||(!is_array($element->childList))) return;

        foreach ($element->childList as $child){
            $realChild = $this->items[$child->ID];
            $this->displayMenu .= $this->display($realChild);
        }
    }

    /**
     * Wrapper function to lookup the menu title
     *
     * @param none
     * @return menuTitle
     */
    function getMenuTitle(){
        return $this->directive['menuTitle'] . "<BR>";
    }

    /**
     * Adds in the final touches to build the menu
     *
     * @param none
     * @return the complete menu as a string
     */
    function completePage() {

        $page = "<SPAN CLASS='menuTitle'>".$this->getMenuTitle()."</SPAN>";
        $page .= $this->displayMenu;

        return $page;
    }

}


/**
 * The menu module 
 *
 */
class SM_menu extends SM_module {

    var $myMenu;
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        // Directives
        $this->addDirective('centerInTable',false);
        $this->addDirective('menuTitle','<B>My Menu</B>');
        $this->addDirective('menuStyle','sp');              // valid types are ul,ol,sp,nb
        $this->addDirective('menuSpaces','4');
        $this->addDirective('maxLevel','1');
        $this->addDirective('selectedItemDefault','1');
        $this->addDirective('menuIDVar','');

        // These are style classes
        $this->addDirective('menuLinkSelectedClass','menuTitleSelected');
        $this->addDirective('menuLinkClass','menuTitle');
        $this->addDirective('menuHeaderClass','menuTitle');

        // In Vars
        $this->addInVar('SM_menuID',0,'int',true);
        
        // build the menu
        $this->myMenu = new _menu($this->modulePrefix);
    }
        
    /**
     * Wrapper for menu::addItem function
     * used to allow items to be added from the directive script
     *
     * @param id (int) Unique identifier REQUIRED
     * @param title (string) displayed text REQUIRED
     * @param parent (int) id of items parent - 0 by default or for top level
     * @param link (string) url of item - $PHP_SELF by default
     * @param tabOrder (int) override listed order - bottom by default
     * @param accessReq (int) member access requirement to view item - 1 for member
     * @return nothing
     */
    function addItem($id,$title,$parent=0,$link='',$tabOrder=99,$accessReq=0) {
        $this->myMenu->addItem($id,$title,$parent,$link,$tabOrder,$accessReq);
    }

    /**
     * Wrapper for menu::addHeader function
     *
     * @param id (int) Unique identifier REQUIRED
     * @param title (string) displayed text REQUIRED
     * @param parent (int) id of items parent - 0 by default or for top level
     * @param tabOrder (int) override listed order - bottom by default
     * @param accessReq (int) member access requirement to view item - 1 for member
     *
     */
    function addHeader($id, $title, $parent=0, $tabOrder=99, $accessReq=0) {
        $this->myMenu->addHeader($id, $title, $parent, $tabOrder, $accessReq);
    }

    /**
     * Add a menu to your page.
     * 
     *
     * @param
     * @return nothing
     */
    function moduleThink() {
        $this->myMenu->configure($this->directive);
        
        // Select which item it selected
        $selectedItem = $this->getVar('SM_menuID');
        if (empty($selectedItem)) {
            $this->myMenu->setSelectedItem($this->directive['selectedItemDefault']);
        } else {
            $this->myMenu->setSelectedItem($selectedItem);
        }
        
        $this->myMenu->build();
        $this->saybr($this->myMenu->completePage());

    }
    
}


?>
