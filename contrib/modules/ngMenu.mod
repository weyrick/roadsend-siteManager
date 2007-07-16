<?php

/*********************************************************************
 *  Roadsend PHP SiteManager
 *  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
 **********************************************************************
 *
 * This source file is subject to version 1.0 of the Roadsend Public
 * License, that is bundled with this package in the file 
 * LICENSE, and is available through the world wide web at 
 * http://www.roadsend.com/license/rpl1.txt
 *
 **********************************************************************
 * Author(s): weyrick  -- weyrick@roadsend.com
 *
 * A SiteManager module for implementing an Object Oriented, heirarchical,
 * auto-expanding HTML menu.
 *
 * EXAMPLE (from inside of a module):
 *
 *   // get the menu module
 *   $menu = $this->loadModule('ngMenu');
 *   
 *   // add root item, return created child
 *   $child = $menu->addItem('Root Item');
 *   
 *   // add child (linked) to the child
 *   $child->addLinkItem('Child Item','test.php');
 *   
 *   // add a root item with an ID
 *   $menu->addItem('New Root Item','myID');
 *   
 *   // get that item back so we can add to it
 *   $gItem = $menu->getItem('myID');
 *   
 *   // add a child to it
 *   $newChild = $gItem->addItem('New Child');
 *   
 *   // add directives to the child
 *   $newChild->addDirective('preFix','<b>');
 *   $newChild->addDirective('postFix','</b>');
 *   
 *   // show the menu in this module
 *   $this->say($menu->run());
 *
 *
 * TODO: caching for getItem() 
 *
 * Changes:
 *      12/04/02 - add linkStyle directive to items which is used when item is a link (weyrick)
 *      10/17/02 - seperate functionality of displaying children output from display item output (weyrick)
 *      09/23/02 - add ability to sort items alphabetically (weyrick)
 *      08/02/02 - add auto expand functionality (weyrick)
 *      07/30/02 - add directives preMenuOutput and postMenuOutput to main module (weyrick)
 *      06/14/02 - return $newChild even when child is delayed, so it can be configured (weyrick)
 *      04/29/02 - add ability to add a child item to the menu without adding the parent first (weyrick)
 *      04/17/02 - add rowStart, rowEnd directives (weyrick)
 *               - allow custom menuItem class (weyrick)
 *      02/05/02 - don't display <br> for ROOT_NODE (thanks Philippe)
 *                 small HTML cleanup, make XHTML compatible (weyrick)
 *
 */

/** root item define */
define('NGMENU_ROOT_NODE','__nginit__');

/**
 * a single menu item
 */
class ngMenuItem extends SM_object {

    /** (string) an ID string for this menu item, which should be unique throughout the menu */
    var $id = NULL;

    /** (object: ngMenuItem) a reference to the parent of this item, or NULL if root item */
    var $parent = NULL;

    /** (array: ngMenuItem) a list of children items */
    var $childList = array();

    /** (string) the text this menu item displays */
    var $text = '';

    /** (string) an optional URL this item should link to */
    var $link = NULL;

    /** (object: ngMenuItem) the root item of the menu */
    var $rootItem = NULL;
    
    /** (bool) whether we are hidden because of auto expanding or not, or max depth */
    var $hidden = false;

    /** (bool) whether we are selected because of auto expanding or not */
    var $selected = false;
    
    /** (object: ngMenuItem) if an item is selected, this will point to it (in root item only) */
    var $selectedItem = NULL;

    /** (int) tree level */
    var $treeLevel = 0;

    /**
     * constructor
     * @param &$parent (object) a reference to the parent of the new item, or bool(true) if root item
     * @param $text (string) the text this menu item will display
     * @param $id (string) an optional ID string to be associated with this item. If not passed, it will be
     *                     generated
     */
    function ngMenuItem(&$parent, $text, $id='') {

        // sitemanager setup
        $this->_smoConfigure();

        // default directives
        $this->directive['spaceType']   = 'spacer'; // spacer or levelStartEnd
        $this->directive['spaceFactor'] = 2;        // print spacer (below) n * spaceFactor * menu_depth times
        $this->directive['spacer']      = '&nbsp;'; // print this character as a spacer, using above formula
        $this->directive['preFix']      = '';       // print this before the menu item text (after spacer)
        $this->directive['postFix']     = '';       // print this after the menu item text
        $this->directive['rowStart']    = '';       // print before entire row, including spacer and preFix
        $this->directive['rowEnd']      = '<br />'; // print after entire row, including postFix
        $this->directive['style']       = '';       // use this CSS style when outputting the text
        $this->directive['linkStyle']   = '';       // use this CSS style when outputting a link
        $this->directive['levelStart']  = '<ul>';   // when in levelStartEnd mode, print this at each menu_depth begin
        $this->directive['levelEnd']    = '</ul>';  // when in levelStartEnd mode, print this at each menu_depth end
        $this->directive['inheritDirectives'] = true;   // if true, children inherit their parents directives
        $this->directive['varExcludeList'] = NULL;  // session variables to exclude from linked menu items

        // handle creation of root node
        if (is_bool($parent) && $id == NGMENU_ROOT_NODE) {
            $this->parent = NULL;
            $this->rootItem = $this;
            $this->treeLevel = 0;
            $this->hidden = true;
        }
        else {

            // not root
            $this->parent = $parent;

            // link to root
            $this->rootItem = $parent->rootItem;

            // set treeLevel based on parent
            $this->treeLevel = $parent->treeLevel + 1;

            // inherit parents directives?
            if ($this->parent->directive['inheritDirectives'])            
                $this->configure($parent->directive);

            // max depth?
            if ( ($maxDepth = $this->rootItem->getDirective('maxDepth')) > 0) {

                // if i'm above max depth, i'm hidden
                if ($this->treeLevel > $maxDepth) {
                    $this->hidden = true;
                }

            }

            // check root to see if we're auto expanding
            if ($this->rootItem->getDirective('autoExpand') && ($this->rootItem->getDirective('selectVar') != '')) {

                // we are auto expanding, determine if I'm selected
                if ($this->inVarH->getGET($this->getDirective('selectVar')) == $id) {

                    // yes this item is selected
                    $this->selected = true;
                    $this->rootItem->selectedItem = $this;                    

                }

            }

        }
        
        // may be blank
        if ($id == '') {
            $id = $text.'_'.sizeof($parent->childList);
        }

        $this->id = $id;
        $this->text = $text;

    }


    /**
     * custom ngMenuItem sort function
     */
    function _ngMenuItemSort($item1, $item2) {
                       
        if (strtoupper($item1->text) < strtoupper($item2->text)) {
            return -1;
        }
        elseif (strtoupper($item1->text) == strtoupper($item2->text)) {
            return 0;
        }
        elseif (strtoupper($item1->text) > strtoupper($item2->text)) {
            return 1;
        }

        return 0;           
        
    }
    
    
    /**
     * sort my children alphabetically by their text title
     */
    function sortByText() {
        
        // if no children, return
        if (!sizeof($this->childList))
            return;
        
        // sort child list
        uasort($this->childList, array($this, '_ngMenuItemSort'));
        
        // tell my children to sort their lists, etc.
        foreach ($this->childList as $id => $child) {
            $this->childList[$id]->sortByText();
        }
        
    }
    
    /**
     * set all parents of this item to shown status (!hidden)
     */
    function recurseParentShow() {
        if ($this->parent != NULL) {
            $this->hidden = false;
            $this->parent->recurseParentShow();
        }
    }

    /**
     * set all immediately childrent shown
     */
    function recurseShowImmediateChildren() {
        if ($this->parent != NULL) {
            if (sizeof($this->childList)) {
                foreach ($this->childList as $id => $child) {
                    $this->childList[$id]->hidden = false;
                }
            }
            $this->parent->recurseShowImmediateChildren();
        }
    }

    /**
     * determines if any child of mine is selected
     */
    function haveSelectedChild() {

        if ($this->selected) {
            return true;
        }

        if (sizeof($this->childList)) {
            foreach ($this->childList as $id => $child) {
                if ($this->childList[$id]->haveSelectedChild()) {
                    return true;
                }
            }
        }

        // default to no
        return false;

    }

    /**
     * determines if any child of mine is not hidden
     */
    function haveNonHiddenChild() {

        if (sizeof($this->childList)) {
            foreach ($this->childList as $id => $child) {
                if (!$this->childList[$id]->hidden) {
                    return true;
                }
                if ($this->childList[$id]->haveNonHiddenChild()) {
                    return true;
                }
            }
        }

        // default to no
        return false;

    }


    /**
     * run on the root item to autoexpand children properly
     */
    function autoExpand() {

        if ($this->id != NGMENU_ROOT_NODE) {
            return;
        }

        if (!$this->rootItem->getDirective('autoExpand') || $this->selectedItem == NULL)
            return;


        // have a selected item, make sure it's visible
        $this->selectedItem->hidden = false;

        // set all parents of the selected item to show
        $this->selectedItem->recurseParentShow();

        // set all immediately children of the selected item to show
        // set all siblings of the selected item to show (w/o their children)
        $this->selectedItem->recurseShowImmediateChildren();

    }

    /**
     * set a new parent
     */
    function setParent(&$newParent) {
        $this->parent = $newParent;
    }

    /**
     * return my ID
     * @return (string) ID string
     */
    function getID() {
        return $this->id;
    }

    /**
     * tell this item to link to a URL with hLink
     * @param $link (string) the URL to link to
     */
    function setLink($link) {
        $this->link = $link;
    }

    /**
     * add an already instantiated ngMenuItem child to this item
     * @param &$newItem (object) the reference to an ngMenuItem object
     */
    function addChild(&$newItem) {
        $this->childList[] = $newItem;
    }


    /**
     * add an already instantiated ngMenuItem child to this item.
     * this version adds the child as a copy, not by reference (needed for delayedChildren)
     * @param $newItem (object) the reference to an ngMenuItem object
     */
    function addChildCopy($newItem) {
        $this->childList[] = $newItem;
    }

    /**
     * return the number of children available in this item
     * @return (int) number of childeren
     */
    function numChildren() {
        return sizeof($this->childList);
    }

    /**
     * return the requested child from my childList (by number)
     * @param $num (int) the element to retrieve the child from
     * @return (object) ngMenuItem, or NULL
     */
    function getChild($num) {
        if (isset($this->childList[$num])) {
            return $this->childList[$num]; 
        }
        else {
            return NULL;
        }
    }

    /**
     * return the child that follows the child passed, or NULL if there isn't one
     * @return (object) ngMenuItem, or NULL     
     */
    function getNextChild(&$childMatch) {
        
        $getNext = false;
        foreach ($this->childList as $num => $child) {
            if ($getNext) {
                return $this->childList[$num];
            }
            if ($child->id == $childMatch->id) {
                $getNext = true;
            }
        }

        // not found
        return NULL;

    }

    /**
     * return the child that comes before the child passed, or NULL if there isn't one
     * @return (object) ngMenuItem, or NULL     
     */
    function getPrevChild(&$childMatch) {
        
        $saveChild = NULL;
        foreach ($this->childList as $num => $child) {
            if ($child->id == $childMatch->id) {
                return $saveChild;
            }
            $saveChild = $this->childList[$num];
        }

        // not found
        return NULL;

    }

    /**
     * return a reference to my parent item
     * @return (object) ngMenuItem
     */
    function getParent() {
        return $this->parent;
    }

    /**
     * traverse forward through the tree
     * @param $checkChildren (bool) used in the recursion of finding a match (don't touch)
     */
    function traverse($checkChildren=true) {
        
        // try to get first child
        if ($checkChildren && isset($this->childList[0])) {
            return $this->childList[0];
        }

        // try to get next sibling
        if (is_object($this->parent)) {

            $sibling = $this->parent->getNextChild($this);
            // if we found it, return it
            if (isset($sibling)) {
                return $sibling;
            }
            else {
                return $this->parent->traverse(false);
            }
        }
        else {
            // root node, can't traverse further
            return NULL;
        }

    }

    /**
     * fetch an item by ID. If this item owns the requested ID, it returns itself.
     * if it's not, but has children, the children will be checked for the ID requested.
     * if my child has it's ID, return the child.
     * this is a recursive function, and should be called on the root node.
     * @param $id (string) the ID to fetch
     * @return (object) ngMenuItem requeted, or NULL if not found
     */
    function getItem($id) {

        if ($id == $this->id) {
            return $this;
        }

        if (sizeof($this->childList)) {

            foreach ($this->childList as $num => $child) {
                $match = $this->childList[$num]->getItem($id);
                if ($match != NULL) {
                    return $match;
                }
            }
           
        }
        else {
            return NULL;
        }

    }

    /**
     * add a new link item, which will create a child of this item
     * @param $text (string) the text to appear in the new menu item
     * @param $link (string) URL to link the new item to
     * @param $id (string) optional ID to associate with this item
     * @return (object: ngMenuItem) the newly created child
     */
    function addLinkItem($text, $link, $id='') {
        $class = get_class($this);
        $newChild = new $class($this, $text, $id);
        $newChild->setLink($link);
        $this->addChild($newChild);        
        return $newChild;
    }
    /**
     * add a new item, which will create a child of this item
     * @param $text (string) the text to appear in the new menu item
     * @param $id (string) optional ID to associate with this item
     * @return (object: ngMenuItem) the newly created child
     */
    function addItem($text, $id='') {
        $class = get_class($this);
        $newChild = new $class($this, $text, $id);
        $this->addChild($newChild);        
        return $newChild;
    }


    /**
     * return children output
     */
    function displayChildren() {

        global $NG_MENU_DEPTH;

        $op = '';

        // if we have children, display them as well
        if (sizeof($this->childList)) {

            if ($this->id != NGMENU_ROOT_NODE)
                $NG_MENU_DEPTH++;

            if ($this->directive['spaceType'] == 'levelStartEnd')
                $op .= $this->directive['levelStart'];

            foreach ($this->childList as $id => $child) {
                if (!$this->childList[$id]->hidden) {
                    $op .= $this->childList[$id]->display();
                }
            }

            if ($this->directive['spaceType'] == 'levelStartEnd')
                $op .= $this->directive['levelEnd'];

            if ($this->id != NGMENU_ROOT_NODE)
                $NG_MENU_DEPTH--;
        }

        return $op;

    }


    /**
     * display myself and my children
     * this is a recursive function, and should be called on the root node
     * @return (string) output from myself and my children
     */
    function display() {

        // keep track of menu depth (should be static class variable)
        global $NG_MENU_DEPTH;
        
        // handle spacing of items
        if ($this->directive['spaceType'] == 'spacer') {
            $spacer = str_repeat($this->directive['spacer'],$this->directive['spaceFactor']*$NG_MENU_DEPTH);
        }
        else {
            $spacer = '';
        }

        $op = '';

        // if we're not the root item, select prefixes
        if ($this->id != NGMENU_ROOT_NODE) {
            $op = $this->directive['rowStart'].$spacer.$this->directive['preFix'];
            if (empty($this->link)) {       
                if (!empty($this->directive['style'])) {
                    $op .= '<span class="'.$this->directive['style'].'">'.$this->text.'</span>';
                }
                else {
                    $op .= $this->text;
                }
            }
            else {        
                if (!empty($this->directive['linkStyle'])) {
                    $style = $this->directive['linkStyle'];
                }
                elseif (!empty($this->directive['style'])) {
                    $style = $this->directive['style'];
                }
                else {
                    $style = '';
                }
                $op .= $this->sessionH->hLink($this->link, $this->text, $style, '', $this->directive['varExcludeList']);
            }

            $op .= $this->directive['postFix'].$this->directive['rowEnd']."\n";
        }
        
        // get childrens output
        $op .= $this->displayChildren();

        // return final output
        return $op;
    }

}

/**
 * SiteManager module that implements a heirarchical text menu
 */
class ngMenu extends SM_module {

    /** (object: ngMenuItem) the root item */
    var $rootItem = NULL;

    /** (string) the class we should use for menu items. needs to be ngMenuItem, or based on it */
    var $menuItemClass = 'ngMenuItem';

    /** (array) an array of children items that need to be added later because their parents aren't here yet */
    var $delayedChildList = NULL;

    /** create the root item */
    function moduleConfig() {        
        $this->setRootItem();
        // this is the amount of times the menu will try to run delayed children (those who were added before their parents)
        $this->addDirective('maxRecursions', 30); // set this higher is you have a large menu with lots of unordered children
        $this->addDirective('preMenuOutput', ''); // echo before root module output
        $this->addDirective('postMenuOutput',''); // echo after root module output
        $this->addDirective('maxDepth',       0); // max depth to display, 0 is all depths
        $this->addDirective('selectVar',     ''); // if we need to track which item is selected, this is the variable name
        $this->addDirective('autoExpand', false); // auto expand mode. need to set selectVar if true.
        $this->addDirective('autoExpandFull', false); // when item is selected, if this is true the selected element shows ALL children,
                                                      // otherwise it only shows the next level down 
                                                      // NOTE currently unused! 8/2/02
    }

    /** set the root item by creating the root node, based on menuItemClass */
    function setRootItem($newClass=NULL) {
        if ($newClass != NULL) {
            $this->menuItemClass = $newClass;
        }
        if (!class_exists($this->menuItemClass)) {
            $this->fatalErrorPage("Requested menu item class $this->menuItemClass doesn't exist");
        }
        $init = true;
        $this->rootItem = new $this->menuItemClass($init, '', NGMENU_ROOT_NODE);
    }

    /** configure the menu module, and also the root item */
    function configure($data) {
        parent::configure($data);
        $this->rootItem->configure($this->directive);
    }

    /** configure the menu module, and also the root item */
    function addDirective($key, $val) {
        parent::addDirective($key, $val);
        $this->rootItem->addDirective($key, $val);
    }

    /**
     * add a new text item to the root menu
     * @param $text (string) the text the item should display
     * @param $id (string) an optional ID to associate with this item
     * @param $parentID (string) an optional ID to use as the parent to this item, which doesn't
                                 have to exist before adding the child (just before display)
     * @return (object: ngMenuItem) the newly created item
     */
    function addItem($text, $id='', $parentID=NULL) {

        if ($parentID != NULL) {
            // try to find parent
            $parent = $this->rootItem->getItem($parentID);
            if ($parent == NULL) {
                // parent isn't here yet. create it and save it in delay list
                // set parent to rootItem for now
                $newChild = new $this->menuItemClass($this->rootItem, $text, $id);
                $this->delayedChildList[$parentID][] = $newChild;
                return $newChild;
            }
            else {
                $newChild = new $this->menuItemClass($parent, $text, $id);
                $parent->addChild($newChild);
                return $newChild;
            }
        }
        else {
            // no parent specified, do top item
            $newChild = new $this->menuItemClass($this->rootItem, $text, $id);
            $this->rootItem->addChild($newChild);
            return $newChild;
        }

    }
    /**
     * add a new linked text item to the root menu
     * @param $text (string) the text the item should display
     * @param $link (string) URL to link this item to
     * @param $id (string) an optional ID to associate with this item
     * @param $parentID (string) an optional ID to use as the parent to this item, which doesn't
                                 have to exist before adding the child (just before display)
     * @return (object: ngMenuItem) the newly created item
     */
    function addLinkItem($text, $link, $id='', $parentID=NULL) {

        if ($parentID != NULL) {
            // try to find parent
            $parent = $this->rootItem->getItem($parentID);
            if ($parent == NULL) {
                // parent isn't here yet. create it and save it in delay list
                // set parent to rootItem for now
                $newChild = new $this->menuItemClass($this->rootItem, $text, $id);
                $newChild->setLink($link);
                $this->delayedChildList[$parentID][] = $newChild;
                return $newChild;
            }
            else {
                $newChild = new $this->menuItemClass($parent, $text, $id);
                $newChild->setLink($link);
                $parent->addChild($newChild);
                return $newChild;
            }
        }
        else {
            // no parent specified, do top item
            $newChild = new $this->menuItemClass($this->rootItem, $text, $id);
            $newChild->setLink($link);
            $this->rootItem->addChild($newChild);
            return $newChild;
        }

    }

    /**
     * retrieve an already added menu item by it's item ID
     * @param $id (string) the ID to fetch
     * @return (object: ngMenuItem) the requested item, or NULL if not found
     */
    function getItem($id) {
        return $this->rootItem->getItem($id);
    }

    /**
     * attempt to insert children who had no parent initially into the menu
     *
     */
    function runDelayedChildren($recur=0) {

        // unset list
        $uList = array();

        if ($recur > $this->directive['maxRecursions']) {
            $this->fatalErrorPage("Too manu recursions running delayed children");
        }

        // if we don't have any, return
        if (sizeof($this->delayedChildList) == 0) {
            return;
        }

        // but since we do, we'll loop
        foreach ($this->delayedChildList as $parentID => $childList) {

            // does the parent exist yet?
            $parent = $this->rootItem->getItem($parentID);

            if ($parent != NULL) {
                // add to unset list
                $uList[] = $parentID;
                // yes!
                foreach ($childList as $child) {
                    // add in the waiting children
                    // NOTE: $child should be a copy, not a reference
                    $child->setParent($parent);
                    $parent->addChildCopy($child);
                }
            }

        }

        // if we found some matches...
        if (sizeof($uList)) {
            // remove parents we found from the main list
            foreach ($uList as $parentID) {
                unset($this->delayedChildList[$parentID]);
            }
            // if we still have some, call ourself
            if (sizeof($this->delayedChildList)) {
                $recur++;
                $this->runDelayedChildren($recur);
            }
            // finish
            return;
        }
        else {
            // we didn't find any matches, which means we've added
            // all we can add
            if (sizeof($this->delayedChildList)) {
                // still had some children we couldn't find a parent for.. 
                // poor little jimmy won't get a family today, it seems.
                $this->debugLog("ended up with ".sizeof($this->delayedChildList)." orphaned nodes");
            }
            return;
        }

    }

    /** sort all items, starting at root, alphabetically by text */
    function sortByText() {
        $this->rootItem->sortByText();
    }
    
    
    /** display the menu */
    function moduleThink() {

        // reset depth counter
        global $NG_MENU_DEPTH;
        $NG_MENU_DEPTH = 0;

        // attempt to insert delayed children
        $this->runDelayedChildren();

        // update hidden status based on selected items
        $this->rootItem->autoExpand();

        // display menu
        $this->say($this->directive['preMenuOutput'].$this->rootItem->display().$this->directive['postMenuOutput']);

    }

}


?>