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
 *  input entity implementing a survey grid 
 *
 *  use this to create an entity that sets up a grid of quesitons where
 *  the list of responses are the same for each question. each question 
 *  will get a radio or check box for each response added. this is useful for
 *  survey type questions where the reponse list is to the effect of
 *  (Excellent, Good, Average) and there are several questions where
 *  the user must choose a rating.
 *
 *  make sure you add all responses BEFORE adding questions!
 *
 *  directives:
 *  inputType - either 'radio' or 'checkBox'  
 *
 *
 *  standard table settings
 *   
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */

// make sure radio entity is loaded
SM_sfLoadEntity("radio");

class surveyGridEntity extends SM_inputTypeEntity {


    /**
    * configure the entity 
    */

    function entityConfig() {

        // clean layout up, we'll do our own
        $this->parentFormEntity->layout = SF_LAYOUT_SINGLENOBR;
        
        // force filters on
        $this->parentFormEntity->forceFilter = true;

        // default to radio
        $this->directive['inputType']           = 'radio';

        // table settings        
        $this->directive['tableBorder']         = 0;
        $this->directive['tableWidth']          = "50%";
        $this->directive['tableCellPadding']    = "3";
        $this->directive['tableCellSpacing']    = "3";
        $this->directive['tableBgColor']        = "";

    }

    /**
     * add a response the that will be available to the user for each question
     * @param mixed $response either a full hash of response (key=response,val=value) or a single response
     * @param mixed $val the value of the response given. only used if $response is NOT a hash
     */
    function addResponse($response,$val=0) {

        // if it's an array, use the whole thing as a reponse list, otherwise add it singularly
        if (!is_array($response)) {        
            $this->directive['responseList'][$response] = $val;
        }
        else {
            $this->directive['responseList'] = $response;
        }

    }

    /**
     * add a question to this grid. this should be called only AFTER all responses have been added!
     * @param string $q the question (or "title")
     * @param string $varName the varName to use for this input. you will use this later in getVar()
     * @param bool $req whether this question requires input or not
     * @param mixed $val the initial response this question has checked
     */
    function addQuestion($q,$varName,$req=false,$val='') {

        // sanity check
        if (($this->directive['inputType'] != 'radio')&&($this->directive['inputType'] != 'checkBox')) {
            $this->fatalErrorPage("input type must be 'radio' or 'checkBox'");
        }

        // create a radio entity
        $newEntity = new SM_formEntity($varName, $q, $this->directive['inputType'], $req, $this->parentFormEntity->parentSmartForm, 0, $val);

        // add to smartForm internal var list
        $this->parentFormEntity->parentSmartForm->addInternalVar($varName);

        // set it up
        $newEntity->inputEntity->addDirective('preOut','<td align="center">');
        $newEntity->inputEntity->addDirective('postOut','</td>');

        // add our list of responses as options
        foreach ($this->directive['responseList'] as $r => $v) {
            $newEntity->inputEntity->addOption('',$v);
        }

        // save it for output
        $this->directive['questionList'][$q] = $newEntity;

    }
    
    /**
    * override SM_inputEntity::applyFilters so we can run the filters of our internal entities
    */
     
    function applyFilters() {

        // apply filters for my entities
        foreach ($this->directive['questionList'] as $q => $entity) {

            $entity->applyFilters();

            // if the filter wasn't satisfied, then neither are we
            if (!$entity->inputEntity->isValid) {
                $this->isValid = false;
            }

        }

        // if we're invalid, stop here
        if (!$this->isValid) {
            return;
        }

        // call parent
        parent::applyFilters();

    }

    /**
    * output 
    */

    function entityThink() {

        
        $this->say("<table width=\"{$this->directive['tableWidth']}\" border=\"{$this->directive['tableBorder']}\"");

        if ($this->directive['tableBgColor'] != '')
            $this->say(" bgcolor=\"{$this->directive['tableBgColor']}\" ");

        $this->say(" cellpadding=\"{$this->directive['tableCellPadding']}\" cellspacing=\"{$this->directive['tableCellSpacing']}\"><tr><td>");
        
        if ($this->directive['header'] != '') {
            $this->say($this->directive['header']);
        }
        else {        
            $this->say("<br />");
        }
        
        $this->say("</td>");

        // header: response list
        foreach ($this->directive['responseList'] as $r => $val) {
            $this->say("<td align=\"center\"><b>$r</b></td>\n");
        }

        $this->say('</tr>');

        // get title and output of our radio entities
        foreach ($this->directive['questionList'] as $q => $entity) {

            $this->say("<tr><td>");
            $this->say($entity->getTitle());
            $this->say("</td>");
            $this->say($entity->output());
            $this->say('</tr>');

        }

        $this->say('</table>');

    }

}

?>
