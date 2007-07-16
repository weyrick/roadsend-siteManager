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

SM_sfLoadEntity('select');      // load our parent

/**
 *
 * a select box which is a list of a states.
 *  INHERITS selectEntity
 *  
 *  directives:
 *          same as selectEntity
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */ 
class stateListEntity extends selectEntity {

    
    function entityConfig() {
            
        parent::entityConfig();

        // setup some defaults
        $this->directive['showOutsideUS']          = TRUE;

    }

    /**
    * populate the list 
    */
    
    function populateList() {
                
        // setup state abbreviation list
        $this->addOption("Alabama",'AL');
        $this->addOption("Alaska",'AK');
        $this->addOption("Arizona",'AZ');
        $this->addOption("Arkansas",'AR');
        $this->addOption("California",'CA');
        $this->addOption("Colorado",'CO');
        $this->addOption("Connecticut",'CT');
        $this->addOption("Delaware",'DE');
        $this->addOption("District of Columbia",'DC');
        $this->addOption("Florida",'FL');
        $this->addOption("Georgia",'GA');
        $this->addOption("Hawaii",'HI');
        $this->addOption("Idaho",'ID');
        $this->addOption("Illinois",'IL');
        $this->addOption("Indiana",'IN');
        $this->addOption("Iowa",'IA');
        $this->addOption("Kansas",'KS');
        $this->addOption("Kentucky",'KY');
        $this->addOption("Louisiana",'LA');
        $this->addOption("Maine",'ME');
        $this->addOption("Maryland",'MD');
        $this->addOption("Massachusetts",'MA');
        $this->addOption("Michigan",'MI');
        $this->addOption("Minnesota",'MN');
        $this->addOption("Mississippi",'MS');
        $this->addOption("Missouri",'MO');
        $this->addOption("Montana",'MT');
        $this->addOption("Nebraska",'NE');
        $this->addOption("Nevada",'NV');
        $this->addOption("New Hampshire",'NH');
        $this->addOption("New Jersey",'NJ');
        $this->addOption("New Mexico",'NM');
        $this->addOption("New York",'NY');
        $this->addOption("North Carolina",'NC');
        $this->addOption("North Dakota",'ND');
        $this->addOption("Ohio",'OH');
        $this->addOption("Oklahoma",'OK');
        $this->addOption("Oregon",'OR');
        $this->addOption("Pennsylvania",'PA');
        $this->addOption("Rhode Island",'RI');
        $this->addOption("South Carolina",'SC');
        $this->addOption("South Dakota",'SD');
        $this->addOption("Tennessee",'TN');
        $this->addOption("Texas",'TX');
        $this->addOption("Utah",'UT');
        $this->addOption("Vermont",'VT');
        $this->addOption("Virginia",'VA');
        $this->addOption("Washington",'WA');
        $this->addOption("West Virginia",'WV');
        $this->addOption("Wisconsin",'WI');
        $this->addOption("Wyoming",'WY');

        $this->addOption("Puerto Rico",'PR');

        if ($this->getDirective('showOutsideUS')) {
            $this->addOption('Outside US',"Outside US");
        }

                
    }

}

?>
