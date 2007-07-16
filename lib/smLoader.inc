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
 * @package roadsend_siteManager
 */
/**
 * load a component
 * this may be called multiple times on the same component, as it will not load
 * it more than once
 * @param string $cID the component to load
 */
function SM_loadComponent($cID) {

    global $SM_componentMap, $SM_siteManager, $SM_componentCache;

    if (isset($SM_componentCache[$cID])) {
        return;
    }

    if (isset($SM_componentMap[$cID])) {
        $includeFile = $SM_componentMap[$cID];
        SM_debugLog("loading SiteManager component [$cID] from $includeFile",NULL,1);
        $SM_siteManager->includeLib($includeFile);
        $SM_componentCache[$cID] = true;
    }
    else {
        SM_fatalErrorPage("requested component [$cID] was not defined");
    }

}

/**
 * determine if a component is loaded or not
 * @param string $cID component to check
 */
function SM_isComponentLoaded($cID) {
    global $SM_componentCache;
    return (isset($SM_componentCache[$cID]));
}

/**
 * determine if a component is registered or not
 * @param string $cID component to check
 */
function SM_isComponentRegistered($cID) {
    global $SM_componentMap;
    return isset($SM_componentMap[$cID]);
}

/**
 * register a new component
 * @param string $cID a unique component ID, usually a class name
 * @param string $componentFile the file this component definition can be found in
 */
function SM_registerComponent($cID, $componentFile) {

    global $SM_componentMap;

    $SM_componentMap[$cID]   = $componentFile;

}

?>
