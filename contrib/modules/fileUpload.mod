<?php

/*********************************************************************
*  Roadsend SiteManager
*  Copyright (c) 2001 Roadsend, Inc.(http://www.roadsend.com)
**********************************************************************
*
* This source file is subject to version 1.0 of the Roadsend Public
* License, that is bundled with this package in the file 
* LICENSE, and is available through the world wide web at 
* http://www.roadsend.com/license/rpl1.txt
*
**********************************************************************
* Author(s): pym
*
* Change Log
* 
*       1/10/2002 - Added a directive for the text for the submit
*                   button. Moved the confermation message so that
*                   it isn't displayed untill after everything else
*                   is done :) (gollum)
*
*       01/15/2004 - Added the ability to either blindly allow overwriting
*                   of files (old and now default behavior), not allow overwriting
*                   of files at all or to ask the user if they want to overwrite
*                   the file that already exists. (gollum) 
*/

/**
 * a skeleton module. extends base SM_module class.
 *
 */
class SM_fileUpload extends SM_module {
     
     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        // Directives
        $this->addDirective('path','/tmp/');
        $this->addDirective('controlsOnRight',true);
        $this->addDirective('tableWidth','400');
        $this->addDirective('submitButton','Submit');
        
        //yes, no, or ask
        $this->addDirective('overwriteExistingFile','yes');
        $this->addDirective('alwaysAskOverwrite',false);
        
        $this->addInVar('askOverwrite',0,'int');
    }

     /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {
        global $PHP_SELF, $HTTP_POST_FILES;
        
        
        // New Smart Form
        $myForm = $this->newSmartForm();
        
        // configure form
        $myForm->addDirective("requiredTag","");
        
        if ($this->directive['controlsOnRight'])
            $myForm->addDirective("controlsOnRight","true");
        
        
        // New smartform file upload
        $myForm->addDirective("enctype","multipart/form-data");
        
        $myForm->add('fileName','File Name','fileUpload',false,'',array('size'=>'20'),0);
        
        // If askOverwrite is set to 1, then the file they tried to uplad exists. ask
        // if they want to overwrite the existing file.
        if((strtolower($this->getDirective('overwriteExistingFile'))=='ask') && ($this->getVar('askOverwrite') == 1 || $this->getDirective('alwaysAskOverwrite'))) {
            $myForm->addDirective('tableCellPadding','5');
            if($this->getVar('askOverwrite') == 1) $myForm->setHeader('File Already Exists. Check the checkbox to overwrite.');
            $cb = $myForm->add('overwrite','','checkBox');
            $cb->addOption('Overwrite existing File?','yes');
        }

        // run the form
        $myForm->runForm();

        // was data verified?        
        if ($myForm->dataVerified()) {

            // yes!
            $prefix = $this->modulePrefix;

            if(empty($prefix)) {
                $fileName = $HTTP_POST_FILES['fileName']['name'];
                $tmpName =  $HTTP_POST_FILES['fileName']['tmp_name'];
            } else {
                $fileName = $HTTP_POST_FILES['fileName_'.$prefix]['name'];
                $tmpName =  $HTTP_POST_FILES['fileName_'.$prefix]['tmp_name'];
            }

            // Check to see if the file exists.
            $exists = file_exists($this->directive['path'].$fileName);
            
            // Unless otherwise stated, we want to move the file.
            $attemptMove = true;
            
            switch(strtolower($this->getDirective('overwriteExistingFile'))) {
                    case 'no':
                        // File exists and we aren't alowing overwrite.
                        if($exists) $attemptMove=false;
                        break;
                    
                    case 'ask':
                        $approveOverwrite = $myForm->getVar('overwrite');
                        if($exists) {
                            if(empty($approveOverwrite)) {
                                SM_reloadPage(array('askOverwrite'=>1));
                                $attemptMove = false;
                            } else {
                                $attemptMove = true;
                            }
                        } 
                    
                        break;
                    case 'yes':
                    default:
                        //nothing to do
            }
            
            
            if($attemptMove){
                if(move_uploaded_file($tmpName,$this->directive['path'].$fileName)) {
                    $this->saybr("$fileName has been added to {$this->directive['path']}");
                }else {
                    $this->saybr("There was a problem uploading $fileName . Please contact the system adminstrator");
                }
            } else {
                $this->saybr("File already exists. Can't overwrite existing file.");
            }
            
        }
        else {
        
            // no! output the form
            $this->say($myForm->output($this->directive['submitButton']));
            
        }

    }
    
}

?>
