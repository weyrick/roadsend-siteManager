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

/**
 *
 *  text input box. should also
 *  be used for password input.
 *  
 *  directives:
 *  
 *      size        -   size of the text input box
 *                      on screen
 *                      default (20)
 *
 *      maxLength   - maximum length the total input
 *                    can be
 *                    default (20)
 *            
 *      passWord    - when true, input type will actually
 *                  be PASSWORD, so the characters will
 *                  be hidden when typed.
 *                  default (false)
 *
 *      allowPasswordValue - when true, we won't clear out password
 *                           values set through defaultValue, which is the
 *                           default behavior. (set to true to be able to
 *                           prefill this password field with a value)
 *
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class textEntity extends SM_inputTypeEntity {

    /**
    * setup entity 
    */

    function entityConfig() {
            
        // setup some defaults
        $this->directive['size']       = 20;
        $this->directive['maxLength']  = 20;
        $this->directive['passWord']   = false;
        $this->directive['allowPasswordValue'] = false;
        
    }

    /**
    * output 
    */

    function entityThink() {
        
        if ($this->directive['passWord']) {
           $type = "password";
           if ($this->directive['allowPasswordValue']) {
               $val = $this->value;
           }
           else {
               $val = '';
           }
        } else {
           $type = "text";
           $val = $this->value;
        }

        // quote val?
        $val = htmlentities($val, ENT_COMPAT);
        
        // maxLength should be at least size big
        if ($this->directive['size'] > $this->directive['maxLength']) {
            $this->directive['maxLength'] = $this->directive['size'];
        }

        $attr = $this->getCommonAttr();
        $this->say("<input $attr type=\"$type\" name=\"$this->varName\" size=\"{$this->directive['size']}\" maxlength=\"{$this->directive['maxLength']}\" value=\"$val\" />");
    
    }

}

?>
