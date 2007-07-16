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
 * Change Log
 * 
 *
 */

/**
 *      
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */
class poolSelectEntity extends SM_inputTypeEntity {

    /**
     * the total list of options, including all available choices
     * @var array
     */
    var $optionList = array();

    /**
     * the pool list, generated from the option list, minus those selected
     *.@var array
     */
    var $poolList = array();

    /**
     * the list of selected items
     * @var array
     */
    var $selectedList = array();

    /**
     * the template to use for output
     * @var SM_layoutTemplate
     */
    var $template = NULL;

    /**
     * the pool select form entity
     * @var SM_formEntity
     */
    var $poolEntity = NULL;


    /**
     * the selected select form entity
     * @var SM_formEntity
     */
    var $selectedEntity = NULL;

    /**
     * the add submit button entity
     * @var SM_formEntity
     */
    var $addEntity = NULL;

    /**
     * the remove submit button entity
     * @var SM_formEntity
     */
    var $removeEntity = NULL;

    /** 
     * flag to denote whether the form entities have been created
     * @var bool
     */
    var $haveEntities = false;

    /** 
     * flag to determine if the add button was pressed. mutually exclusive with removeSelected
     * @var bool
     */
    var $addSelected = false;

    /** 
     * flag to determine if the remove button was pressed. mutually exclusive with addSelected
     * @var bool
     */
    var $removeSelected = false;

    /**
     *  setup entity, defaults
     */ 
    function entityConfig() {

        global $SM_siteManager;

        SM_sfLoadEntity('select');
        SM_sfLoadEntity('submit');

        $this->addDirective('selectSize','5');
        $this->addDirective('addButtonText','>>');
        $this->addDirective('removeButtonText','<<');
        $this->addDirective('sort',true);
        
        $jscript = <<< END_JSCRIPT
// ===================================================================
// Author: Matt Kruse <matt@mattkruse.com>
// WWW: http://www.mattkruse.com/
//
// NOTICE: You may use this code for any purpose, commercial or
// private, without any further permission from the author. You may
// remove this notice from your final code if you wish, however it is
// appreciated by the author if at least my web site address is kept.
//
// You may *NOT* re-distribute this code in any way except through its
// use. That means, you can include it in your product, or your web
// site, or any other form where the code is actually being used. You
// may not put the plain javascript up on your site for download or
// include it in your javascript libraries for download. 
// If you wish to share this code with others, please just point them
// to the URL instead.
// Please DO NOT link directly to my .js files from your site. Copy
// the files to your server and use them there. Thank you.
// ===================================================================

/* SOURCE FILE: selectbox.js */
function hasOptions(obj){if(obj!=null && obj.options!=null){return true;}return false;}
function selectUnselectMatchingOptions(obj,regex,which,only){if(window.RegExp){if(which == "select"){var selected1=true;var selected2=false;}else if(which == "unselect"){var selected1=false;var selected2=true;}else{return;}var re = new RegExp(regex);if(!hasOptions(obj)){return;}for(var i=0;i<obj.options.length;i++){if(re.test(obj.options[i].text)){obj.options[i].selected = selected1;}else{if(only == true){obj.options[i].selected = selected2;}}}}}
function selectMatchingOptions(obj,regex){selectUnselectMatchingOptions(obj,regex,"select",false);}
function selectOnlyMatchingOptions(obj,regex){selectUnselectMatchingOptions(obj,regex,"select",true);}
function unSelectMatchingOptions(obj,regex){selectUnselectMatchingOptions(obj,regex,"unselect",false);}
function sortSelect(obj){var o = new Array();if(!hasOptions(obj)){return;}for(var i=0;i<obj.options.length;i++){o[o.length] = new Option( obj.options[i].text, obj.options[i].value, obj.options[i].defaultSelected, obj.options[i].selected) ;}if(o.length==0){return;}o = o.sort(
function(a,b){if((a.text+"") <(b.text+"")){return -1;}if((a.text+"") >(b.text+"")){return 1;}return 0;});for(var i=0;i<o.length;i++){obj.options[i] = new Option(o[i].text, o[i].value, o[i].defaultSelected, o[i].selected);}}
function selectAllOptions(obj){if(!hasOptions(obj)){return;}for(var i=0;i<obj.options.length;i++){obj.options[i].selected = true;}}
function moveSelectedOptions(from,to){if(arguments.length>3){var regex = arguments[3];if(regex != ""){unSelectMatchingOptions(from,regex);}}if(!hasOptions(from)){return;}for(var i=0;i<from.options.length;i++){var o = from.options[i];if(o.selected){if(!hasOptions(to)){var index = 0;}else{var index=to.options.length;}to.options[index] = new Option( o.text, o.value, false, false);}}for(var i=(from.options.length-1);i>=0;i--){var o = from.options[i];if(o.selected){from.options[i] = null;}}if((arguments.length<3) ||(arguments[2]==true)){sortSelect(from);sortSelect(to);}from.selectedIndex = -1;to.selectedIndex = -1;}
function copySelectedOptions(from,to){var options = new Object();if(hasOptions(to)){for(var i=0;i<to.options.length;i++){options[to.options[i].value] = to.options[i].text;}}if(!hasOptions(from)){return;}for(var i=0;i<from.options.length;i++){var o = from.options[i];if(o.selected){if(options[o.value] == null || options[o.value] == "undefined" || options[o.value]!=o.text){if(!hasOptions(to)){var index = 0;}else{var index=to.options.length;}to.options[index] = new Option( o.text, o.value, false, false);}}}if((arguments.length<3) ||(arguments[2]==true)){sortSelect(to);}from.selectedIndex = -1;to.selectedIndex = -1;}
function moveAllOptions(from,to){selectAllOptions(from);if(arguments.length==2){moveSelectedOptions(from,to);}else if(arguments.length==3){moveSelectedOptions(from,to,arguments[2]);}else if(arguments.length==4){moveSelectedOptions(from,to,arguments[2],arguments[3]);}}
function copyAllOptions(from,to){selectAllOptions(from);if(arguments.length==2){copySelectedOptions(from,to);}else if(arguments.length==3){copySelectedOptions(from,to,arguments[2]);}}
function swapOptions(obj,i,j){var o = obj.options;var i_selected = o[i].selected;var j_selected = o[j].selected;var temp = new Option(o[i].text, o[i].value, o[i].defaultSelected, o[i].selected);var temp2= new Option(o[j].text, o[j].value, o[j].defaultSelected, o[j].selected);o[i] = temp2;o[j] = temp;o[i].selected = j_selected;o[j].selected = i_selected;}
function moveOptionUp(obj){if(!hasOptions(obj)){return;}for(i=0;i<obj.options.length;i++){if(obj.options[i].selected){if(i != 0 && !obj.options[i-1].selected){swapOptions(obj,i,i-1);obj.options[i-1].selected = true;}}}}
function moveOptionDown(obj){if(!hasOptions(obj)){return;}for(i=obj.options.length-1;i>=0;i--){if(obj.options[i].selected){if(i !=(obj.options.length-1) && ! obj.options[i+1].selected){swapOptions(obj,i,i+1);obj.options[i+1].selected = true;}}}}
function removeSelectedOptions(from){if(!hasOptions(from)){return;}for(var i=(from.options.length-1);i>=0;i--){var o=from.options[i];if(o.selected){from.options[i] = null;}}from.selectedIndex = -1;}
function removeAllOptions(from){if(!hasOptions(from)){return;}for(var i=(from.options.length-1);i>=0;i--){from.options[i] = null;}from.selectedIndex = -1;}
function addOption(obj,text,value,selected){if(obj!=null && obj.options!=null){obj.options[obj.options.length] = new Option(text, value, false, selected);}}


/* SOURCE FILE: OptionTransfer.js */

function OT_transferLeft(){moveSelectedOptions(this.right,this.left,this.autoSort,this.staticOptionRegex);this.update();}
function OT_transferRight(){moveSelectedOptions(this.left,this.right,this.autoSort,this.staticOptionRegex);this.update();}
function OT_transferAllLeft(){moveAllOptions(this.right,this.left,this.autoSort,this.staticOptionRegex);this.update();}
function OT_transferAllRight(){moveAllOptions(this.left,this.right,this.autoSort,this.staticOptionRegex);this.update();}
function OT_saveRemovedLeftOptions(f){this.removedLeftField = f;}
function OT_saveRemovedRightOptions(f){this.removedRightField = f;}
function OT_saveAddedLeftOptions(f){this.addedLeftField = f;}
function OT_saveAddedRightOptions(f){this.addedRightField = f;}
function OT_saveNewLeftOptions(f){this.newLeftField = f;}
function OT_saveNewRightOptions(f){this.newRightField = f;}
function OT_update(){var removedLeft = new Object();var removedRight = new Object();var addedLeft = new Object();var addedRight = new Object();var newLeft = new Object();var newRight = new Object();for(var i=0;i<this.left.options.length;i++){var o=this.left.options[i];newLeft[o.value]=1;if(typeof(this.originalLeftValues[o.value])=="undefined"){addedLeft[o.value]=1;removedRight[o.value]=1;}}for(var i=0;i<this.right.options.length;i++){var o=this.right.options[i];newRight[o.value]=1;if(typeof(this.originalRightValues[o.value])=="undefined"){addedRight[o.value]=1;removedLeft[o.value]=1;}}if(this.removedLeftField!=null){this.removedLeftField.value = OT_join(removedLeft,this.delimiter);}if(this.removedRightField!=null){this.removedRightField.value = OT_join(removedRight,this.delimiter);}if(this.addedLeftField!=null){this.addedLeftField.value = OT_join(addedLeft,this.delimiter);}if(this.addedRightField!=null){this.addedRightField.value = OT_join(addedRight,this.delimiter);}if(this.newLeftField!=null){this.newLeftField.value = OT_join(newLeft,this.delimiter);}if(this.newRightField!=null){this.newRightField.value = OT_join(newRight,this.delimiter);}}
function OT_join(o,delimiter){var val;var str="";for(val in o){if(str.length>0){str=str+delimiter;}str=str+val;}return str;}
function OT_setDelimiter(val){this.delimiter=val;}
function OT_setAutoSort(val){this.autoSort=val;}
function OT_setStaticOptionRegex(val){this.staticOptionRegex=val;}
function OT_init(theform){this.form = theform;if(!theform[this.left]){alert("OptionTransfer init(): Left select list does not exist in form!");return false;}if(!theform[this.right]){alert("OptionTransfer init(): Right select list does not exist in form!");return false;}this.left=theform[this.left];this.right=theform[this.right];for(var i=0;i<this.left.options.length;i++){this.originalLeftValues[this.left.options[i].value]=1;}for(var i=0;i<this.right.options.length;i++){this.originalRightValues[this.right.options[i].value]=1;}if(this.removedLeftField!=null){this.removedLeftField=theform[this.removedLeftField];}if(this.removedRightField!=null){this.removedRightField=theform[this.removedRightField];}if(this.addedLeftField!=null){this.addedLeftField=theform[this.addedLeftField];}if(this.addedRightField!=null){this.addedRightField=theform[this.addedRightField];}if(this.newLeftField!=null){this.newLeftField=theform[this.newLeftField];}if(this.newRightField!=null){this.newRightField=theform[this.newRightField];}this.update();}
function OptionTransfer(l,r){this.form = null;this.left=l;this.right=r;this.autoSort=true;this.delimiter=",";this.staticOptionRegex = "";this.originalLeftValues = new Object();this.originalRightValues = new Object();this.removedLeftField = null;this.removedRightField = null;this.addedLeftField = null;this.addedRightField = null;this.newLeftField = null;this.newRightField = null;this.transferLeft=OT_transferLeft;this.transferRight=OT_transferRight;this.transferAllLeft=OT_transferAllLeft;this.transferAllRight=OT_transferAllRight;this.saveRemovedLeftOptions=OT_saveRemovedLeftOptions;this.saveRemovedRightOptions=OT_saveRemovedRightOptions;this.saveAddedLeftOptions=OT_saveAddedLeftOptions;this.saveAddedRightOptions=OT_saveAddedRightOptions;this.saveNewLeftOptions=OT_saveNewLeftOptions;this.saveNewRightOptions=OT_saveNewRightOptions;this.setDelimiter=OT_setDelimiter;this.setAutoSort=OT_setAutoSort;this.setStaticOptionRegex=OT_setStaticOptionRegex;this.init=OT_init;this.update=OT_update;}
END_JSCRIPT;
        $this->sayJS($jscript);
        
    }

    /**
     * create form entities used in the output of the poolSelectEntity
     *
     */
    function createEntities() {

        // pool select
        $this->poolEntity = new SM_formEntity('_pool', ' ', 'select',
                                               false, $this->parentFormEntity->parentSmartForm, 0);
        $this->poolEntity->inputEntity->configure(array('multiple'=>true,'size'=>$this->getDirective('selectSize')));

        // selected select
        $this->selectedEntity = new SM_formEntity('_selected', ' ', 'select',
                                                   false, $this->parentFormEntity->parentSmartForm, 0);
        $this->selectedEntity->inputEntity->configure(array('multiple'=>true,'size'=>$this->getDirective('selectSize')));

        // add button
        $this->addEntity = new SM_formEntity('_selectBut', ' ', 'submit',
                                              false, $this->parentFormEntity->parentSmartForm, 0);
        $this->addEntity->inputEntity->configure(array('value'=>$this->getDirective('addButtonText'),
                                                       'button'=>true,
                                                       'extra'=>'ONCLICK="opt.transferRight()"'));

        // remove button
        $this->removeEntity = new SM_formEntity('_removeBut', ' ', 'submit',
                                                 false, $this->parentFormEntity->parentSmartForm, 0);
        $this->removeEntity->inputEntity->configure(array('value'=>$this->getDirective('removeButtonText'),
                                                    'button'=>true,
                                                    'extra'=>'ONCLICK="opt.transferLeft()"'));

        $this->haveEntities = true;

    }

    /**
     * set the main pool list. this should include ALL available options, INCLUDING
     * the ones that may be selected! This method MUST be called, and BEFORE calling
     * setSelectedList
     *
     * @param array $optionList the complete option list, in hash, keyed by title
     */
    function setPoolList($optionList) {

        $this->optionList = $optionList;

    }

    /**
     * set the selected list. should be a subset of items from pool list. this needs to be called
     * AFTER setPoolList
     *
     * @param array $selectedList the complete option list, in hash, keyed by title
     */
    function setSelectedList($selectedList) {

        if ($this->optionList == NULL) {
            $this->fatalErrorPage('optionList must be set before setting selectedList');
        }

        if (!$this->haveEntities) {
            $this->createEntities();
        }

        $this->selectedList = $selectedList;

        // calculate pool list
        foreach ($this->optionList as $title => $data) {

            if (empty($this->selectedList[$title])) {
                $this->poolList[$title] = $data;
            }

        }
    
        $this->poolEntity->inputEntity->setList($this->poolList);
        $this->selectedEntity->inputEntity->setList($this->selectedList);

        if ($this->selectedEntity->inputEntity->optionCount() == 0) {
            $this->selectedEntity->inputEntity->addDirective('addNoOption',true);
        }
        if ($this->poolEntity->inputEntity->optionCount() == 0) {
            $this->poolEntity->inputEntity->addDirective('addNoOption',true);
        }

        if ($this->directive['sort']) {
            $this->poolEntity->inputEntity->sortByTitle();
            $this->selectedEntity->inputEntity->sortByTitle();
        }

    }
    
    /**
     * set SM_layoutTemplate to use for output. this is optional, as a default basic template
     * will be used if this is not set.
     * 
     * to make a template, there should be four areas created:
     *  poolItems      - output of pool select entity
     *  selectedItems  - output of selected selet entity
     *  selectButton   - output of select (add) button
     *  removeButton   - output of remove button
     *
     * @param string $templateName the template to load. should be in template GCS path
     * @return SM_layoutTemplate the loaded template
     */
    function setTemplate($templateName) {
        
        global $SM_siteManager;

        $this->template = $SM_siteManager->loadTemplate($templateName);
        return $this->template;
     
    }

    /**
     * override SM_inputEntity::applyFilters so we can see if our submit was pressed
     *
     */
    /*
    function applyFilters() {

        if (isset($_POST['_selectBut'])) {
            $this->addSelected = true;
        }
        if (isset($_POST['_removeBut'])) {
            $this->removeSelected = true;
        }
        
        if ($this->addSelected || $this->removeSelected) {
            $this->isValid = true;
        }
        else {
            $this->isValid = false;
        }

    }
    */


    function getAddedLeft() {
        return $_POST['_addLeft'];
    }

    function getRemovedLeft() {
        return $_POST['_remLeft'];
    }

    function getAddedRight() {
        return $_POST['_addRight'];
    }

    function getRemovedRight() {
        return $_POST['_remRight'];
    }

    function getNewLeft() {
        return $_POST['_newLeft'];
    }

    function getNewRight() {
        return $_POST['_newRight'];
    }
    
    /**
     * output the entity
     */
    function entityThink() {

        global $SM_siteManager;
        
        // they'll be blank if they don't exist by now, but at least we won't error
        if (!$this->haveEntities) {
            $this->createEntities();
        }

        $this->sayJS('var opt = new OptionTransfer("_pool[]","_selected[]");');
        if ($this->directive['sort']) {
            $this->sayJS('opt.setAutoSort(true);');
        }
        $this->sayJS('opt.saveRemovedLeftOptions("_remLeft");');
        $this->sayJS('opt.saveRemovedRightOptions("_remRight");');
        $this->sayJS('opt.saveAddedLeftOptions("_addLeft");');
        $this->sayJS('opt.saveAddedRightOptions("_addRight");');
        $this->sayJS('opt.saveNewLeftOptions("_newLeft");');
        $this->sayJS('opt.saveNewRightOptions("_newRight");');
        
        
        // NOTE: this script requires the HTMLBODY and JAVASCRIPT SM tags in the root template
        $rootTemplate = $SM_siteManager->rootTemplate;
        $bodyTags = $rootTemplate->getTagsByType('HTMLBODY');
        if (empty($bodyTags))
            $this->fatalErrorPage('poolSelectEntity needs root template to have an HTMLBODY tag to work');
        // XXX if there was already a bodyOther, this will munge it!!
        $bodyTags[0]->addDirective('bodyOther','onLoad="opt.init(document.forms[\''.$this->parentFormEntity->parentSmartForm->formID.'\'])"');
        $jsTags = $rootTemplate->getTagsByType('JAVASCRIPT');
        if (empty($jsTags))
                $this->fatalErrorPage('poolSelectEntity needs root template to have a JAVASCRIPT tag to work');

        if ($this->template == NULL) {
            $tData = '<table width=100% align=center>
                        <tr>
                            <td><sm type="area" name="poolItems"></td>
                            <td>
                                <sm type="area" name="selectButton"><br>
                                <sm type="area" name="removeButton">
                            </td>
                            <td><sm type="area" name="selectedItems"></td>
                        </tr>
                      </table>';
            $this->template = new SM_layoutTemplate();
            $this->template->setTemplateData($tData);
        }
        
        // run entities
        $this->template->addText($this->poolEntity->output(), 'poolItems');
        $this->template->addText($this->selectedEntity->output(), 'selectedItems');
        $this->template->addText($this->addEntity->output(), 'selectButton');
        $this->template->addText($this->removeEntity->output(), 'removeButton');

        $this->say('<input type="hidden" name="_addLeft">');
        $this->say('<input type="hidden" name="_remLeft">');
        $this->say('<input type="hidden" name="_addRight">');
        $this->say('<input type="hidden" name="_remRight">');
        $this->say('<input type="hidden" name="_newLeft">');
        $this->say('<input type="hidden" name="_newRight">');
        
        $this->say($this->template->run());

    }
}

?>
