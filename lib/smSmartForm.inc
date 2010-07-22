<?php

/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001-2009 Roadsend, Inc.(http://www.roadsend.com)
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


global $SM_siteManager;

// input frobbers
$SM_siteManager->includeLib('sfInputFrob');

// include input entities
$SM_siteManager->includeLib('sfInputEntities');

// filters
$SM_siteManager->includeLib('sfFilters');

// form entities
$SM_siteManager->includeLib('sfFormEntity');


/**
 * SmartForms abstract HTML forms to code.
 * With SmartForms, you define a form by telling it exactly what you need and how
 * the information needs to be formatted. SmartForms handles all other aspects of the form.
 *
 * SmartForms should not be instantiated directly, but reather created in modules
 *
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 * @see SM_module::newSmartForm
 */
class SM_smartForm extends SM_object {

    /**
     * array of formEntities in this form, keyed by varName
     * @see SM_formEntity
     * @var array
     */
    var $entityList = NULL;

    /**
     * double hash of groups of form variables (fieldsets)
     * @var array
     */
    var $groupList = array();

    /**
     * legends for field sets, by group name
     * @var array
     */
    var $legendList = array();

    /**
     * first time form load or not
     * @var bool
     */
    var $ftl = true;

    /**
     * raw text counter. used internally by {@link addText}
     * @var int
     */
    var $rt = 0;

    /**
     * final buffered output
     * @var string
     */
    var $sfOutput = '';

    /**
     * a header to place at the top of the form table. set with {@link setHeader}
     * @var string
     */
    var $header = '';

    /**
     * a footer to place at the bottom of the form table. set with {@link setFooter}
     * @var string
     */
    var $footer = '';

    /**
     * the template we either built or loaded, that form entities will appear in
     * @var string
     */
    var $template = '';

    /**
     * an expanded array of hashes used to mimic the data structure
     * when the XML file is read in
     * @var array
     */
    var $xEntity = array();

    /**
     * a list of variables available from getVar, but that are not available through
     * entityList because they are inside of another entity
     * @var array
     */
    var $internalVarList = array();

    /**
     * a unique form ID
     * @var string
     */
    var $formID = '';

    /**
     * SmartForm reader, for creating forms from XML or database
     * @see SM_configReader_SMARTFORM
     * @var SM_configReader_SMARTFORM
     */
    var $cFormReader = NULL;

    /**
     * Track if this form has run. This is set to true
     * at the end of the runForm function.
     * @var boolean
     **/
     var $formHasRun = false;

    /**
     * Default form entity template
     *
     */
    var $formEntityTpt = '';

     /**
      * create new SmartForm using the ID passed, which is used in the form name. SmartForms
      * should NOT be instantiated directcly, see {@link SM_module::newSmartForm}
      * @param string $formID unique (to this page) ID, used to make formName
      */
    function SM_smartForm($formID) {

        // setup session handlers
        $this->_smoConfigure();

        /// TEMPLATE BUILDING

        // style sheet classes used during build of a template, if one isn't specified
        $this->directive['requiredClass']    = 'sf_required'; // applies to wrapper div
        $this->directive['optionalClass']    = 'sf_optional'; // applies to wrapper div
        $this->directive['errorClass']       = 'sf_error'; // applies to wrapper div
        $this->directive['errorMsgClass']    = 'sf_errorMsg'; // applies to error msg text
        $this->directive['entityClass']      = ''; // default class for all input entities (default none)

        // this is the class for the main FORM tag
        $this->directive['formClass']        = '';

        // bad form message, displayed at top if
        // any filters failed
        $this->directive['badFormMessage']      = '';

        // dump the smartform template to HTML source
        $this->directive['dumpTemplate']        = false;

        ///

        // if this is set to true, ALL entities will be presetText, regardless of what is
        // specified as the entity type in the add() call
        $this->directive['readOnly']            = false;

        // sessions vars to be excluded from propagating in this form
        $this->directive['varExcludeList']      = NULL;

        // reset button off by default
        $this->directive['resetButton']         = '';           // when blank, it wont show up. otherwise it should be
                                                                // the VALUE for the reset button

        // onSubmit event of form
        $this->directive['onSubEvent']      = '';

        // script to POST to. normally $PHP_SELF. if you change this, all validation functionality goes
        // out the window.
        $this->directive['postScript']          = $_SERVER['PHP_SELF'];

        // Set the encode type for a smartform
        $this->directive['enctype']             = '';

        // submit button image
        $this->directive['submitImage']         = '';

        // raw form attributes that should be inserted into the <form> tag
        $this->directive['formAttributes']      = '';

        // a flag used by SF tag to say that the end form tag has been written
        // so that we don't duplicate it
        $this->directive['formIsEnded']         = false;

        // header is blank by default
        $this->header                           = '';

        // footer is blank by default
        $this->footer                           = '';

        // form ID
        $this->formID                           = $formID;

        // the FORM NAME
        $this->directive['formName']            = $this->formID;

        // global frob list. if non empty, add each frobber to each entity
        $this->directive['globalFrobList']      = NULL;

        // setup FTL
        $myFTL = $this->formID.'_ftl';
        $ftlVal = $this->inVarH->getPOST($myFTL);

        // is this a first time load?
        $this->ftl = !isset($ftlVal);

        // default fieldset
        $this->groupList['default']  = array();
        $this->legendList['default'] = '';

        // default form entity template
        $this->formEntityTpt = '<sm type="sf" data="title" var="___VARNAME___"><sm type="sf" data="errorMsg" var="___VARNAME___"><sm type="sf" data="entity" var="___VARNAME___">';

    }

     /**
      * add a block of text in the input form
      * @param string $text the text to add. may contain HTML
      * @param constant $tpt form entity template, optional
      */
    function addText($text, $tpt=NULL) {

        $this->rt++;
        $rtName = 'rawText_'.$this->rt;

        if (empty($tpt))
            $tpt = $this->formEntityTpt;

        $rt = $this->add($rtName,'','rawText',false,'',NULL,$tpt);
        $rt->addDirective('staticText', $text);

    }

     /**
      * setup a header to be displayed at the top of the form table
      * note: not used when the SmartForm uses a user-defined template
      * @param string $hText the text/html to be displayed
      */
    function setHeader($hText) {
        $this->header = $hText;
    }


     /**
      * setup a footer to be displayed at the bottom of the form table
      * note: not used when the SmartForm uses a user-defined template
      * @param string $fText the text/html to be displayed
      */
    function setFooter($fText) {
        $this->footer = $fText;
    }


    /**
     * override debugLog to show our form ID
     *
     */
    function debugLog($msg, $verbosity=0, $addBacktrace=false) {
        parent::debugLog($this->formID.': '.$msg, $verbosity, $addBacktrace);
    }

    /**
     * use a SiteManager template to format the output of the SmartForm
     * @param string $templateFile the template file to load. will search template file path.
     * @return SM_layoutTemplate the newly loaded template
     */
    function loadTemplate($templateFile) {

        global $SM_siteManager;

        // attempt to load the template file
        $this->template = $SM_siteManager->loadTemplate($templateFile);
        $this->template->addDirective('SF', array('parentForm' => $this));
        return $this->template;

    }

    /**
     * return the current SM_layoutTemplate to be used for SmartForm output
     * @return SM_layoutTemplate current layout template
     */
    function getTemplate() {
        return $this->template;
    }

    /**
     * tell the smartform it should use the given SM_layoutTemplate template
     * for layout
     * @param SM_layoutTemplate $templateObj the template the form should use for output
     */
    function useTemplate($templateObj) {
        $this->template = $templateObj;
        $this->template->addDirective('SF', array('parentForm' => $this));
    }


   /**
    * add a new entity to the form
    *
    * @param string $varName the variable name in the form
    * @param string $title the text description to use as the title of the input entity
    * @param string $type the input type
    * @param bool $req required input or not
    * @param mixed $val initial value
    * @param array $args arguments to pass along to configure the input entity
    * @param string $tpt optional form entity template
    * @param string $fieldset which fieldset this belongs to, or null for none (manual)
    * @param bool $prepend place entity at the begining of the list
    * @return SM_inputEntity input entity created
    */
    function add($varName, $title, $type, $req=false, $val='', $args=NULL, $tpt=NULL, $fieldset=NULL, $prepend=false) {

        // don't allow the same variable twice
        if (isset($this->entityList[$varName])) {
            $this->debugLog("add: variable name $varName was already defined. ignoreing.");
            return NULL;
        }

        // if this is a read only form, change type to 'staticText' entity
        if ($this->directive['readOnly'])
            $type = 'presetText';

        // verify and load this type
        if (!SM_sfLoadEntity($type)) {
            $this->debugLog("add: bad/unknown type passed (input entity module not found): $type");
            return NULL;
        }

        // setup default layout
        if ($tpt === NULL)
            $tpt = $this->formEntityTpt;

        // good entity, add it in
        $newEnt = new SM_formEntity($varName, $title, $type, $req, $this, $tpt, $val, $fieldset);
        if ($prepend) {
            $pre[$varName] = $newEnt;
            $this->entityList = array_merge($pre,(array)$this->entityList);
        }
        else {
            $this->entityList[$varName] = $newEnt;
        }

        // add to fieldset
        if ($fieldset)
            $this->groupList[$fieldset][] = $this->entityList[$varName];

        // if they passed args, do them here
        if (is_array($args))
            $this->entityList[$varName]->inputEntity->configure($args);

        // global frobbers?
        if (!empty($this->directive['globalFrobList'])) {
            foreach ($this->directive['globalFrobList'] as $frobber) {
                $this->entityList[$varName]->inputEntity->addFrobber($frobber);
            }
        }

        // success, return reference to formEntity
        return $this->entityList[$varName]->inputEntity;

    }

    /**
     * add a hidden variable to the form
     * @param string $name the name of the variable to add
     * @param string $value value of variable
     */
    function addHidden($name, $value) {
        $this->directive['hiddens'][$name] = $value;
    }

     /**
      * remove an entity from the form that has previously been added
      * @param string $varName the variable to remove
      */
    function remove($varName) {

        if (isset($this->entityList[$varName])) {
            unset($this->entityList[$varName]);
            return true;
        }
        else {
            $this->debugLog("remove: variable not found for remove: $varName");
            return false;
        }

    }

    /**
     * add a variable to the 'internal' var list. this is a variable that should be returned in getVarList()
     * that wasn't actually an entry in the entityList hash. this happens (for example) when input entities
     * have more than one variable name, or when you want to include a hidden in the getVarList() hash
     * @param string $varName the variable name to add
     */
    function addInternalVar($varName) {
        if (!in_array($varName, $this->internalVarList)) {
            $this->internalVarList[] = $varName;
        }
    }

     /**
      * add a filter to an entity. the form will only pass the {@link dataVerified} check
      * when all filters in the form pass successfully.
      * @param string $varName the variable to add the filter to
      * @param string $fType the filter type to add
      * @param string $bMsg the message to show if the filter doesn't pass
      * @param array $args optional arguments to set to configure the filter
      * @return SM_entityFilter newly created filter, or NULL on error
      */
    function addFilter($varName, $fType, $bMsg='', $args=NULL) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("addFilter: variable not found for filter add: $varName");
            return NULL;
        }
        else {
            if (($newFilter = $this->entityList[$varName]->inputEntity->addFilter($fType, $bMsg)) == NULL) {
                $this->debugLog("addFilter: error adding filter type $fType to variable $varName");
                return NULL;
            }
            // if we have arguments, set them now
            if (is_array($args))
                $newFilter->configure($args);

            return $newFilter;
        }

    }


     /**
      * remove a filter from an entity that has previously been added
      * @param string $varName the variable name to change
      * @param string $fType the filter to remove
      */
    function removeFilter($varName, $fType) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("removeFilter: variable not found for filter remove: $varName");
            return false;
        }
        else {
            if (!$this->entityList[$varName]->inputEntity->removeFilter($fType)) {
                $this->debugLog("removeFilter: filter type $fType not found for variable $varName");
                return false;
            }
            else {
                // if this is a required filter, remove from form entity
                if ($fType == 'required') {
                    $this->entityList[$varName]->required = false;
                }
                return true;
            }
        }

    }

     /**
      * set arguments (configuration directives) for a particular input entity
      * @deprecated use {@link getEntity} and then {@link SM_object::configure}
      * @param string $varName variable to configure
      * @param array $args key/value directive list
      * @return bool true on success, false otherwise
      */
    function setArgs($varName, $args) {

        if (!is_array($args)) {
            $this->debugLog("setFilterArgs: args variable was not an array");
            return false;
        }

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setArgs: variable not found for filter add: $varName");
            return false;
        }
        else {
            $this->entityList[$varName]->inputEntity->configure($args);
        }

    }

     /**
      * set a filters arguments (directives)
      * @param string $varName the variable to edit
      * @param string $fType the filter to edit
      * @param array $args an array of key/value arguments
      * @return bool true on success, false on error
      */
    function setFilterArgs($varName, $fType, $args) {

        if (!is_array($args)) {
            $this->debugLog("setFilterArgs: args variable was not an array");
            return false;
        }

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setFilterArgs: variable not found for argument set: $varName");
            return false;
        }

        if (!isset($this->entityList[$varName]->inputEntity->filterList[$fType])) {
            $this->debugLog("setFilterArgs: filter type [$fType] not found for variable [$varName]");
            return false;
        }

        // configure
        $this->entityList[$varName]->inputEntity->filterList[$fType]->configure($args);
        return true;

    }


    /**
     * set a new title for a form entity
     * @param string $varName the variable to set
     * @param string $newTitle new title for this entity
     */
    function setEntityTitle($varName, $newTitle) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setEntityTitle: variable not found: $varName");
            return false;
        }

        $this->entityList[$varName]->setTitle($newTitle);

    }

     /**
      * set a variables form template
      *
      * @param string $varName the variable to set
      * @param string $tpt form template
      * @return bool true on success, false on error
      */
    function setVarFormTemplate($varName, $tpt) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setVarFormTemplate: variable not found for argument set: $varName");
            return false;
        }

        $this->entityList[$varName]->template = $tpt;
        return true;

    }


    /**
     * turn an entity that's already been added into the form, into read only (presetText)
     * @param string $varName the variable to set
     * @param mixed $defaultValue the text the variable should display
     * @return bool true on success, false on error
     */
    function setReadOnly($varName, $defaultValue=NULL) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setLayout: variable not found for argument set: $varName");
            return false;
        }

        $this->entityList[$varName]->setReadOnly($defaultValue);
        return true;

    }


     /**
      * change a set default value for an input entity
      * @param string $varName the variable to set
      * @param mixed $default the new value to display
      * @param bool $forceNotFTL when true, it will set default value even if we're not FTL
      * @return bool true on success, false on error
      */
    function setDefaultValue($varName, $default, $forceNotFTL=false) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog("setDefaultValue: variable not found for argument set: $varName");
            return false;
        }

        // we only set defaults on FTL, unless force is on
        if ($this->ftl || $forceNotFTL) {
            // we set force to true on the inputEntity setDefaultValue, to avoid another ftl check
            $this->entityList[$varName]->inputEntity->setDefaultvalue($default,true);
            return true;
        }
        else {
            return false;
        }

    }

    /**
     * set default values for all matching entities in varHash
     * @param array $varHash key/value pairs of entityName/default values
     * @param bool $forceNotFTL when true, it will set default value even if we're not FTL
     */
    function setDefaults($varHash, $forceNotFTL=false) {
        foreach ($varHash as $k => $v) {
            if (isset($this->entityList[$k])) {
                $this->setDefaultValue($k, $v, $forceNotFTL);
            }
        }
    }

    /**
     * specify that a particular input entity is part of a certain fieldset
     * @param string $varName the variable name to set
     * @param string $fieldset the fieldset to set it to
     * @return bool true on success, false on error
     */
    function setVarFieldset($varName, $fielset) {

        if (!isset($this->entityList[$varName])) {
            $this->debugLog($this, "setVarFieldset: variable not found for group set: $varName");
            return false;
        }

        $this->entityList[$varName]->fieldset = $fieldset;
        $this->groupList[$varName][] = $this->entityList[$varName];

        return true;

    }

    /**
     * specify that a particular input entity is part of a certain fieldset
     * @param array $varList an array of variable names to put in this fieldset
     * @param string $fieldset the fieldset to set it to
     * @return bool true on success, false on error
     */
    function defineFieldset($varList, $fieldset) {

        if (!is_array($varList)) {
            $this->debugLog("defineFieldset: parameter \$varList is not array of variable names");
            return false;
        }

        foreach ($varList as $varName) {

            if (!isset($this->entityList[$varName])) {
                $this->debugLog("defineFieldset: variable not found for group set: $varName");
                return false;
            }

            $this->entityList[$varName]->fieldset = $fieldset;
            $this->groupList[$varName][] = $this->entityList[$varName];

        }

        return true;

    }


    /**
     * wrapper function for runForm(). identical in every way
     * only for consistency.
     */
    function run() { $this->runForm(); }

     /**
      * called to apply filters and "execute" the form
      */
    function runForm() {

        if ($this->formHasRun)
            return;

        // blank form?
        if (empty($this->entityList)) {
            $this->debugLog('form was blank!');
            return;
        }

        foreach ($this->entityList as &$ent) {

            // make sure entity has a group, default if none other
            if (empty($ent->fieldset)) {
                $ent->fieldset = 'default';
                $this->groupList['default'][] = $ent;
            }

            // apply filters/frobbers if this isn't a ftl (ie, dont do server side if this is a first time load)
            if (!$this->ftl) {
                $ent->applyFilters();
                $ent->inputEntity->applyFrobbers();
            }

        }

        // Mark that this form has run
        $this->formHasRun = TRUE;

    }


     /**
      * this function should be called AFTER setting up the entire form. it will return TRUE
      * if all the input entities have passed all of their filters, letting the calling script
      * know that all the data they need has been entered and verified as correct according
      * to the filters applied
      *
      * @return bool true if all data is "verified" and ready for submission, false otherwise
      */
    function dataVerified() {

        // if this is a first time load, simply return false
        // this is also important if there are multiple forms on the same page, as it will
        // avoid us trying to dataVerify on another forms data, since the FTL hidden var name
        // is based on our smartform id, which is based on the module we are in.
        if ($this->ftl)
            return false;

        //check to see if the form has run. if not, run it
        if(!$this->formHasRun)
            $this->runForm();

        // loop through each entity and ask if it's valid or not
        foreach ($this->entityList as $ent) {

            // skip text entities
            if ($ent instanceof rawTextEntity)
                continue;

            if (!$ent->inputEntity->isValid) {
                $this->debugLog("entity [{$ent->varName}] failed filter check");
                return false;
            }

        }

        return true;

    }

    /**
     * build a valid SM_layoutTemplate template, to output the form
     * @param string $submit text of the default submit button. if not specified, no default
     *                       submit button is added.
     */
     function buildTemplate($submit='') {

        // if $submit is set, convert html entities to allow >, etc
        if ($submit != '') {
            $submit = htmlspecialchars($submit);
        }

        // clean
        $template = '';

        // if this is NOT a ftl, it's a bad form. if badFormMessage is set,
        // use that as a header
        if (!$this->ftl && isset($this->directive['badFormMessage'])) {
            $this->header .= "<span class=\"".$this->directive['errorMsgClass']."\">".$this->directive['badFormMessage']."</span>";
        }

        // add header if we've got one
        if ($this->header != '')
            $template .= $this->header;

        // loop through by group
        foreach ($this->groupList as $groupName => $entList) {
             foreach ($entList as $ent) {

                // get info from entity objects for display
                $errClass = ($ent->inputEntity->isValid) ? '' : ' '.$this->directive['errorClass'];
                $template .= "<div id='".$ent->varName.'_wrap'."' class='".(($ent->required)?$this->directive['requiredClass']:$this->directive['optionalClass'])."$errClass'>";
                $template .= str_replace('___VARNAME___', $ent->varName, $ent->template);
                $template .= "</div>\n";

             }
        }

        ///////////////////////////////////////////////////////////////////
        // bottom of table, after form entities


        // add footer if we've got one
        if ($this->footer != '')
            $template .= $this->footer;


        // submit button
        if ($this->directive['resetButton'] != '') {

            if ($submit != '')
                $template .= '<sm type="sf" data="submit" title="'."$submit\">";
            $template .= '<sm type="sf" data="reset" title="'.$this->directive['resetButton'].'">';

        }
        else {
            if ($submit != '')
                $template .= '<sm type="sf" data="submit" title="'."$submit\">";
        }

        // create a new template, set it's template data to the template we just made
        $this->template = new SM_layoutTemplate();
        $this->template->addDirective('SF', array('parentForm' => $this));
        $this->template->setTemplateData($template);


     }

     /**
      * return output of this form. should be called after {@link run}
      * @param string $submit value of the submit button (if not using custom template). leave blank to not add a submit button automatically
      * @param array $hiddens a hash of hidden variables (key/value pairs) to pass with the script
      * @return string the final output of the form, including all HTML, suitable for output to client
      */
    function output($submit="", $hiddens=NULL) {

        // add the ftl last
        $this->addHidden($this->formID.'_ftl','1');

        // if we don't have any entities, quit now
        if ((!isset($this->entityList)) || (!is_array($this->entityList))) {
            $this->debugLog("Warning: no input entities were added, but output was called!");
            return '';
        }

        // if we don't already have a template, make one
        if (empty($this->template))
            $this->buildTemplate($submit);

        // run template, which runs all entityThinks
        $templateOutput = $this->template->run();

        $onSub = $this->directive['onSubEvent'];

        // specify enctype if required
        if (!empty($this->directive['enctype']))
            $enctype =  'enctype="'.$this->directive['enctype'].'"';
        else
            $enctype = '';

        // specify enctype if required
        if (!empty($this->directive['formClass']))
            $fClass =  'class="'.$this->directive['formClass'].'"';
        else
            $fClass = '';

        $this->sfOutput  = "\n\n<form $onSub $enctype $fClass id=\"{$this->directive['formName']}\" action=\"{$this->directive['postScript']}\" method=\"post\"";

        if (!empty($this->directive['formAttributes']))
            $this->sfOutput .= ' '.$this->directive['formAttributes'].' ';

        $this->sfOutput .= ">\n";

        if (is_array($hiddens)|| (isset($this->directive['hiddens']) && is_array($this->directive['hiddens'] ) )) {

            // if they're both arrays, merge them
            if (is_array($hiddens) && (isset($this->directive['hiddens']) && is_array($this->directive['hiddens'])))
                $hiddens = array_merge($hiddens, $this->directive['hiddens']);

            // if only directive hiddens is set, make that $hiddens
            if ( (isset($this->directive['hiddens']) && is_array($this->directive['hiddens'])) && !(is_array($hiddens)))
                $hiddens = $this->directive['hiddens'];

            foreach($hiddens as $hKey => $hVal) {
                $this->sfOutput .= "<input type=\"hidden\" name=\"{$hKey}\" value=\"$hVal\" />\n";
            }

        }

        // ** SESSIONS **
        // session variables from sessions.inc, if available
        if (isset($this->sessionH)) {
            if (is_array($hiddens)) {
                if (is_array($this->directive['varExcludeList'])) {
                    $eList = array_merge($this->directive['varExcludeList'], array_keys($hiddens));
                }
                else {
                    $eList = array_keys($hiddens);
                }
            }
            else {
                $eList = $this->directive['varExcludeList'];
            }
            $this->sfOutput .= $this->sessionH->formID($eList);
        }

        // if we've been told to dump the template, do so here
        if ((!empty($this->directive['dumpTemplate'])) && ($this->directive['dumpTemplate'])) {
            if (is_string($this->template))
                $this->sfOutput .= "<!--\n\n TEMPLATE DUMP\n\n$this->template\n\n -->\n";
            else {
                $tOutput = join("\n",$this->template->htmlTemplate);
                $this->sfOutput .= "<!--\n\n TEMPLATE DUMP\n\n{$tOutput}\n\n -->\n";
            }
        }

        // run template, which runs all entityThinks
        $this->sfOutput .= $templateOutput;

        // end form
        if (!$this->directive['formIsEnded'])
            $this->sfOutput .= "\n</form>\n";

        // return final output
        return $this->sfOutput;

    }


     /**
      * Get the value of a variable that's been added to the form, from the current POST of the script
      * @param string $varName the variable to retrieve
      */
    function getVar($varName) {
        // if we have the entity listed, use its getValue which applies frobs.
        // otherwise just get the POST value, which maintains backwards compatibility
        // and allows for internalVarList
        $v = NULL;
        if (isset($this->entityList[$varName])) {
            $v = $this->entityList[$varName]->inputEntity->getValue();
        }
        if (empty($v)) {
            $v = $this->inVarH->getPOST($varName);
        }
        return $v;
    }

    /**
     * get a reference to a {@link SM_formEntity} (NOT the input entity) by
     * variable ID
     * @see getEntity
     * @param string $varName form variable to retrieve
     * @return SM_formEntity reference to SM_formEntity object, or NULL if not found
     */
    function getFormEntity($varName) {

        // sanity
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("getFormEntity: variable not found: $varName");
            return NULL;
        }

        return $this->entityList[$varName];

    }

    /**
     * determine if an entity has been added to the form, by entity name
     * @param string $varName the var (entity) name
     * @return bool true if entity is part of this form, false otherwise
     */
    function entityExists($varName) {
        return isset($this->entityList[$varName]);
    }

    /**
     * get a reference to a smartform input entity that has been added to the form
     * by variable id
     * @param string $varName form variable to retrieve
     * @return SM_inputTypeEntity reference to SM_inputTypeEntity object requested, or NULL if not found
     */
    function getEntity($varName) {

        // sanity
        if (!isset($this->entityList[$varName]->inputEntity)) {
            $this->debugLog("getEntity: variable not found: $varName");
            return NULL;
        }

        return $this->entityList[$varName]->inputEntity;

    }

    /**
     * see what type of entity a certain varname represents
     * by variable id
     * @param string $varName form variable to lookup
     * @return string a type of input entity, or NULL if not found
     */
    function getEntityType($varName) {

        // sanity
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("getEntityType: variable not found: $varName");
            return NULL;
        }

        return $this->entityList[$varName]->getType();

    }


    /**
     * return the amount of input entities that have been added to the form
     * @return int number of entities
     */
    function getNumVars() {
        return sizeof($this->entityList);
    }

    /**
     * return an array of all input variable names in use
     * @return array list of variables which can be used with getVar()
     */
     function getVarList() {

        // return main list
        $varList = array_keys($this->entityList);

        // return 'internal' list, which would be entities that contain
        // more than one input variable
        if (is_array($this->internalVarList)) {
            foreach ($this->internalVarList as $vName) {
                $varList[] = $vName;
            }
        }

        return $varList;

     }

     /**
      * Return a hash of all input variable names in use, keyed by field name
      * and the value set to getVar(fieldName)
      * @param array $getList a list of fields to explicitly include - all others excluded
      * @return array hash of fieldName => form value
      */
     function getVarHash($getList=NULL) {

         $varList = $this->getVarList();
         foreach ($varList as $fieldName) {
             if (($getList != NULL) && (!in_array($fieldName, $getList))) {
                 continue;
             }
             $varHash[$fieldName] = $this->getVar($fieldName);
         }

         return $varHash;

     }

    /**
     * dump all variables for this form (module).
     * useful for debugging after data gets verified
     */
    function dumpFormVars() {

        $varList = $this->getVarList();
        $output = '';
        foreach ($varList as $formVar) {
            if (isset($_POST[$formVar])) {
                $output .= $formVar." => ".$this->getVar($formVar).'|'.$_POST[$formVar]."<br />";
            }
            else {
                $output .= $formVar.' => [NOT PASSED IN POST]<br />';
            }
        }

        return $output;

    }

    /**
     * dump smartform information
     * @return string debug information
     */
    function dumpInfo() {

        $op = "SmartForm Information:<br /><ul>\n";

        $op .= $this->getClassLog();

        $op .= "Entity List:<br />";
        $varList = $this->getVarList();
        foreach ($varList as $var) {
            $op .= "variable [$var], type ".get_class($this->entityList[$var]->inputEntity)."<br />\n";
        }

        $op .= $this->dumpDirectives();

        $op .= "</ul>";

        return $op;

    }

    /**
     * XML HANDLE FUNCTIONS
     */

    /**
     * parse xml form into smartform objects
     *
     */
    function _parseSF_XML() {

        // Loop through each entity
        // Make certain there are entities
        if (!empty($this->xEntity)) {
            foreach ($this->xEntity as $entityName => $entityValue) {

                // store entity name
                $eLoadList[] = $entityName;

                if (!isset($entityValue['required']))
                    $entityValue['required'] = false;
                if (!isset($entityValue['value']))
                    $entityValue['value'] = '';


                // Add the element
                $entity = $this->add($entityName,
                                      $entityValue['title'],
                                      $entityValue['type'],
                                      $entityValue['required'],
                                      $entityValue['value'],
                                      NULL
                                      // tpt
                                      );

                if ($entity == NULL) {
                    $this->debugLog("unable to add loaded entity to SmartForm: $entityName (defined twice?)");
                    continue;
                }

                // Add Filters to the form element
                if (isset($entityValue['filter'])) {
                    foreach ($entityValue['filter'] as $filterID => $filterIDValue) {
                        foreach($filterIDValue as $filterName => $filterValue){
                            // if there is no array parameter use a simple addDirective.
                            if (empty($filterID)) {
                                // filter value used to store the bad message for a filter with no id.
                                $this->addFilter($entityName,$filterName,$filterValue);
                            } else {
                                // if a filter array (name) is set
                                // then read in the arguments, create an array and set the filter arguments
                                // for each item set

                                // create the filter if it doesnt exist
                                if (!isset($this->entityList[$entityName]->inputEntity->filterList[$filterID])) {
                                    // filter message
                                    $filterMessage = $entityValue['filter'][$filterID]['message'];
                                    $this->addFilter($entityName,$filterID,$filterMessage);
                                }

                                $this->setFilterArgs($entityName,$filterID,array($filterName=>$filterValue));

                            }
                        }
                    }
                } // end filter

                // Add Directives to the form element
                if (isset($entityValue['directive'])) {
                    foreach ($entityValue['directive'] as $directiveID => $directiveIDValue) {
                        foreach($directiveIDValue as $directiveName => $directiveValue) {
                            // if there is no array parameter use a simple addDirective.
                            if (empty($directiveID)) {
                                $entity->addDirective($directiveName,$directiveValue);
                            } else {
                                // if a KEY was set, the value for this directive is an array

                                // check for the special key 'optionList', and use addOption on
                                // this entity
                                if (($directiveID == 'optionList')&&(method_exists($entity,'addOption'))) {
                                    $entity->addOption($directiveValue,$directiveName);
                                }
                                else {
                                    // set the array directly
                                    $entity->directive[$directiveID][$directiveName]=$directiveValue;
                                }
                            }
                        }
                    }
                }  // end directives
            } // end check of existing entities
        } // end entity

        // clear xEntity
        $this->xEntity = NULL;

        return $eLoadList;

    }

    /**
     * parse an XML form retrieved from an external source, such as a database
     * @param string $data the XML data to parse
     * @return array the list of field names loaded from the XML form
     */
    function loadFormData($data,$encoding = 'ISO-8859-1') {

        if (trim($data) == '') {
            $this->debugLog('no xml given');
            return;
        }

        $enc = '<?xml version="1.0" encoding="'.$encoding.'"?>';

        // if we dont have a reader yet, make it
        if ($this->cFormReader == NULL) {
            $this->cFormReader = SM_loadConfigReader('SMARTFORM', $this);
        }

        $this->cFormReader->XMLsetContent($enc.$data);
        $this->cFormReader->parseXML();

        return $this->_parseSF_XML();

    }

    /**
     * force a bad entity, as if a filter had failed, with the specified message
     * @param string $varName the variable to set
     * @param string $errorMsg the message to show inline
     */
    function triggerBadEntity($varName, $errorMsg) {

        // sanity
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("triggerBadEntity: variable not found: $varName");
            return false;
        }

        $this->entityList[$varName]->inputEntity->isValid = false;
        $this->entityList[$varName]->inputEntity->errorMsg = $errorMsg;

        return true;
    }

    /**
     * load a SmartForm definition through an XML file
     * @param string $configFile the xml file
     * @return array the list of field names loaded from the XML form
     */
    function loadForm($configFile) {

        global $SM_siteManager;

        // now find and load form. failing to find this WILL be fatal
        $configFile = $SM_siteManager->findSMfile($configFile, 'smartForms', 'xsm');

        if ($GLOBALS['SM_develState'])
            $this->debugLog("loadForm(): $configFile");

        // fall through: get config
        // if we dont have a reader yet, make it
        if ($this->cFormReader == NULL) {
            $this->cFormReader = SM_loadConfigReader('SMARTFORM', $this);
        }

        // check cache?
        $willCache = false;
        if ($this->siteConfig->getGlobalVar('cache','cacheSmartForm')) {

            $willCache = true;
            if (!$this->loadSerial($configFile, 'SMARTFORM', $this->xEntity)) {
                // if loadserial fails, parse the file
                $this->cFormReader->readConfigXML($configFile);
            }

        }
        else {

            // fall through: parse
            $this->cFormReader->readConfigXML($configFile);

        }

        // should we cache the config file?
        if ($willCache) {
            $this->saveSerial($configFile, 'SMARTFORM', $this->xEntity);
        }

        return $this->_parseSF_XML();

    }

    /**
     * determine if the given button element was pressed.
     * @param $varName the name of the button element that has already been added to the form
     * @return bool
     */
    function buttonPressed($varName) {

        // sanity
        if (!isset($this->entityList[$varName])) {
            $this->debugLog("buttonPressed: variable not found: $varName");
            return false;
        }

        return ($this->getVar($varName) == $this->entityList[$varName]->inputEntity->directive['value']);

    }

}


?>
