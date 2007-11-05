<?php

/*

    Project: 
    using code from RoadsEnd PHP SiteManager
    
    description   : site wide global helper functions

    change history:
    
                05/06/02 - bugfixes (weyrick)
                11/20/01 - updates by weyrick        
                03/09/01 - script created by pym
                    
*/

// if you want real tabs, change this
//define('TAB_4',"\t");
define('TAB_4','    ');

// other defines
define('TAB_8',TAB_4.TAB_4);
define('TAB_12',TAB_8.TAB_4);
define('TAB_16',TAB_8.TAB_8);
define('SM_SAVEXML_VERSION','2');

/**
 * converts a string / boolean in to a xml safe
 * value with true or false, and no '<' '>' tags
 * @param(String or Booean - The value to be converted)
 *
 */
function xmlValue($value, $directiveName = 'unset') {


    // Set boolen to true or false as strings
    if (is_bool($value)) {
        if ($value == true) {
            $xmlSafeValue = "true";
        } else {
            $xmlSafeValue = "false";
        }
    
    // Replace < and > signs
    } elseif (is_string($value)) {
        $value = ereg_replace("<","&lt;",$value);
        $value = ereg_replace(">","&gt;",$value);
        
        $xmlSafeValue = $value;

    // No Change
    } else {
        $xmlSafeValue = $value;
    }
    
    return $xmlSafeValue;

}


/**
 * Save Form - create an XML description of smart form object
 * The form will handle new directives or entitys as they are created
 * but there is a limit of levels (object->entity->directive->object.
 * The most complex structure so it can handle is a entity directive with an array (ie select list) value
 * 
 * Note: If the struction of the smartForm inputEntity Bases class changes you may need
 * to change the exected parameters array,
 *
 * @param $SMobject - the smart form object (required)
 * @return (string) the full XML smartForm output
 */

function SM_saveForm($myForm) {

    // Create new XML SMART FORM DESCRIPTION    
    $xmlPage .= "<SM_XML>\n";
    $xmlPage .= '<SMARTFORM VERSION="'.SM_SAVEXML_VERSION.'">'."\n";  

    
    // create a smartform so we can get the current default directives
    $tSF = new SM_smartForm('testFORM');
    $defaultDirectives = $tSF->directive;

    // list of directives to skip
    $badDirs[] = 'postScript';
    $badDirs[] = 'formName';

    // If there are any directives add a directives section

    $haveDirs = false;
    foreach($myForm->directive as $directiveName => $directiveValue) {
        
        // skip bad
        if (in_array($directiveName, $badDirs))
            continue;

        // check if the directive value is set to the default here
        // only add non default values
        if ($defaultDirectives[$directiveName] != $myForm->directive[$directiveName]) {
            if (!$haveDirs) {
                $haveDirs = true;
                $xmlPage .= TAB_4.'<DIRECTIVE>'."\n";
            }
            $xmlPage .= TAB_8.'<VAR'.TAB_4.'name = "'.$directiveName .'"'.TAB_4.'value =  "' .xmlValue($directiveValue,$directiveName).'"/>'. "\n";
        }

    }
    
    if ($haveDirs)    
        $xmlPage .= TAB_4."</DIRECTIVE>\n";

    
    // If there are any entities add a smartform section
    if (!(empty($myForm->entityList))) {

        foreach($myForm->entityList as $entityName => $entityValue) {
            $xmlPage .= TAB_4.'<ENTITY name = "' .$entityName. '">'."\n";
            
            // BASIC INFORMATION - SMART FORM ENTITY Parameters
            $xmlPage .= TAB_8.'<PARAMETER>'."\n";
            
            // if subtype is in the expected parameter list and subtypeinfo is
            // not empty then print out 
            
            // also, don't do parameter if it's the same as the default
            // for that we need a clean formEntity
            $tFormEntity = new SM_formEntity('temp','title','text',false,$this);

            $expectedValueList = array('type','title','required','value','layout','titleAlign','inputAlign');
            foreach($expectedValueList as $parameter) {                

                // always do type
                if ($parameter == 'type') {
                    $xmlPage .= TAB_12.'<VAR name="'. $parameter .'"' .TAB_4. 'value="'.xmlValue($entityValue->$parameter) .'"/>'."\n";
                }
                elseif( (!empty($entityValue->$parameter)) &&
                    ($tFormEntity->$parameter != $entityValue->$parameter)
                   ) {
                    $xmlPage .= TAB_12.'<VAR name="'. $parameter .'"' .TAB_4. 'value="'.xmlValue($entityValue->$parameter) .'"/>'."\n";
                }

            }
    
            $xmlPage .= TAB_8."</PARAMETER>\n";
        
            // Directives (optional)
            
            // Are there any drectives for this entity
            if (is_array($entityValue->inputEntity->directive)) {
                
                //create a blank entity of this type to see if this directive is different from the default
                $cName = get_class($entityValue->inputEntity);
                $tEntity = new $cName($this);

                // Loop through a list of the entity directives
                foreach($entityValue->inputEntity->directive as $dirType => $dirInfo) {
                    
                    // if this directive is the same as the default, skip it
                    if ($dirInfo == $tEntity->directive[$dirType]) {
                        continue;
                    }

                    // Build Structure with Simple Directive
                    if (!(is_array($dirInfo))) {
                        $sDir = true;
                        $sDirList .= TAB_12.'<VAR name="' .$dirType.'" value= "'. xmlValue($dirInfo).'"/>'."\n";
                    
                    } else {
                        // COMPLEX Directives : where dirInfo is an array loop in and get the values
                        /* OPTION LIST HACK
                         * To protect the value of optionlists form select entitys and their
                         * extended classes set the id to optionList and the values will not be 
                         * set to xmlSafe if the name is true or false                        
                        */

                        // optionList
                        if ($dirType == 'optionList') {
                            
                            $xmlPage .= TAB_8.'<DIRECTIVE   KEY="'.$dirType.'">'."\n";
                            foreach($dirInfo as $tmpKey => $dInfo) {                            
                                foreach($dInfo as $dirKey => $dirValue) {
                                    $xmlPage .= TAB_12.'<VAR name="' .$dirKey.'" value= "'. xmlValue($dirValue) .'"/>'."\n";
                                }
                            }
                            $xmlPage .= TAB_8."</DIRECTIVE>\n";

                        }
                        // other directives
                        else {
                        
                            $xmlPage .= TAB_8.'<DIRECTIVE   KEY="'.$dirType.'">'."\n";
                            foreach($dirInfo as $dirKey => $dirValue) {
                                $xmlPage .= TAB_12.'<VAR name="' .$dirKey.'" value= "'. xmlValue($dirValue) .'"/>'."\n";
                            }
                            $xmlPage .= TAB_8."</DIRECTIVE>\n";
                        
                        }

                    }
    
                } // end loop through directives list
                
                // Output simple directives
                if ($sDir == true) {
                    $xmlPage .= TAB_8.'<DIRECTIVE>'."\n";
                    $xmlPage .= $sDirList;
                    $xmlPage .= TAB_8."</DIRECTIVE>\n";
                    
                    // After adding reset values
                    $sDirList = "";
                    $sDir == false;
                }



            } // end if conditional directives exist

            // Filters (optional)

            if (isset($entityValue->inputEntity->filterList)) {
                
                // Get a list of the filter objects
                foreach($entityValue->inputEntity->filterList as $filterType => $filterObject) {

                    // don't do required filter
                    if ($filterType == 'required')
                        continue;

                    // also, don't do filter directives if it's the same as the default
                    // for that we need a clean filter object
                    SM_sfLoadFilter($filterType);
                    $fName = $filterType.'Filter';
                    $tFilter = new $fName($this);

                    // Complex Filter Object
                    if (is_array(($filterObject->directive))) {
                        $xmlPage .= TAB_8.'<FILTER  KEY="'.$filterType.'"  message="'.$filterObject->badMessage.'">'."\n";
                        foreach($filterObject->directive as $filterKey => $filterValue) {
                            if ($filterValue == $tFilter->directive[$filterKey])
                                continue;
                             $xmlPage .= TAB_12.'<VAR  name="' .$filterKey. '"  value="'.xmlValue($filterValue) .'"/>'."\n";
                         }
                        $xmlPage .= TAB_8."</FILTER>\n";
                    
                    // Simple filter Object
                    } else {

                        $xmlPage .= TAB_8.'<FILTER>'."\n";
                        $xmlPage .= TAB_12.'<VAR name="'.$filterType .'"  value="'. $filterObject->badMessage . '"/>'."\n";
                        $xmlPage .= TAB_8."</FILTER>\n";

                    }
                } // end filter list loop

            }  // end if filters exist condition

        
            $xmlPage .= TAB_4."</ENTITY>\n";
        
        } // end foreach entity loop
    

    } // end if entity exist condition

    // End XML Object
    $xmlPage .="</SMARTFORM>\n";
    $xmlPage .="</SM_XML>\n";

    // End the form

    // display XML or write the XML to a file

    return $xmlPage;

}


?>
