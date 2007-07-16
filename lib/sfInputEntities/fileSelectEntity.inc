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
 * Author(s): Jon Michel (pym@roadsend.com)
 *
 */

SM_sfLoadEntity('select');      // load our parent


/**
 *
 *  a select box which populates itself from a database pull
 *  INHERITS selectEntity
 *  
 *  directives:
 *          same as selectEntity
 *          - plus -
 *          
 *          
 *          $rootDir      - the base directory (required)
 *
 * @author Jon Michel <pym@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class fileSelectEntity extends selectEntity {
    
    /**
    * Set up the defalt vaues for the entity
    * make sure the root directory exists and is readable
    */
     
    function entityConfig() {
            
        parent::entityConfig();

        // setup some defaults
        $this->directive['rootDir']          = '/_invalid_';
        $this->directive['extension']        = '';
        $this->directive['fileNameFilter']   = '';
        $this->directive['includePath']      = FALSE;
        $this->directive['hideDir']          = TRUE;


        if (empty($this->directive['rootDir'])) {
            $this->fatalErrorPage("attempt to use fileSelectEntity without root directory!");
        }

    }

    /** 
    * populate the list with the database pull 
    */

    function populateList() {

        // Get a list of all the files in the directory
        $root = $this->directive['rootDir'];

        if (!is_dir($root)) {
            $this->debugLog("invalid directory: $root");
            $this->addOption('(invalid directory)','');
            return;
        }

        $dh = dir($root);
        if ($dh === false) {
            // invalid directory
            $this->debugLog("invalid directory: $root");
            $this->addOption('(no files)','');
            return;
        }

        $ext = $this->getDirective('extension');
        $fileNameFilter = $this->getDirective('fileNameFilter');
        
        while (($entry = $dh->read()) !== false) {

            // build the absolute file name for later use 
            if ($this->directive['includePath'] == true) {
                $file  = $root ."/". $entry;
            } else {
                $file = $entry;
            }

            if ($this->directive['hideDir']) {
                if (($file == ".") || ($file == "..")) {
                    continue;
                }
            }

            // Filter out all the names we don't want
            if(!empty($fileNameFilter) && !preg_match($fileNameFilter,$entry)) {
                continue;
            }
            
            // Filter Testing when required
            if (empty($ext)) {
                $this->addOption($entry,$file);
            } elseif (preg_match("/\.$ext$/i",$entry)) {
                $extLength = -(strlen($ext)+1);

                $this->addOption(substr($entry,0,$extLength),$file);
            } 


        }

    } // end function

}

?>
