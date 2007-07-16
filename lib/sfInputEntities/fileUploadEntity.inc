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
 * Author(s): Jonathan Michel (pym@roadsend.com)
 *
 */

/**
 *
 *  file upload box.
 *  
 *  directives:
 *  
 *      size        -   size of the text input box
 *                      on screen
 *                      default (20)
 *
 * @author Jon Michel <pym@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class fileUploadEntity extends SM_inputTypeEntity {

    /**
    * setup entity 
    */

    function entityConfig() {
            
        // setup some defaults
        $this->directive['size']       = 20;
        
        // set form encode type of parent smartform
        $this->parentFormEntity->parentSmartForm->addDirective('enctype','multipart/form-data');

    }

    /**
    * output
    */

    function entityThink() {

        $attr = $this->getCommonAttr();
        $this->say("<input $attr type=\"file\" name=\"$this->varName\" size=\"{$this->directive['size']}\" maxlength=\"{$this->directive['maxLength']}\" value=\"$this->value\" />");
    
    }

}

?>
