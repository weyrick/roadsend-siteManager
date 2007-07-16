<?php

/*********************************************************************
*  <project>
*  Copyright (c) 2001-2003 <company name>
**********************************************************************
*
* This source file is subject to version 1.0 of the Roadsend Public
* License, that is bundled with this package in the file 
* LICENSE, and is available through the world wide web at 
* http://www.roadsend.com/license/rpl1.txt
*
**********************************************************************
* Author(s): pym
*/


/**
 * shopping services for members only web sites
 *
 */
global $SM_siteManager;
$SM_siteManager->includeLib('orders.inc');
$SM_siteManager->includeLib('orderItem.inc');

class store extends SM_module {

    var $currentOrder   = NULL;     // oject of type orders 
    var $currentStoreAction  = NULL;     // object of type store action
    var $currentMember  = NULL;     // oject of type member  (subject of order) 
    var $currentUser    = NULL;     // oject of type member (person accessing the system) 

     /**
      * configure the module. run before moduleThink
      */
    function moduleConfig() {

        // allow uID over-ride as inVar or directive
        $this->addInVar('uID');
        $this->addDirective('uID',$this->getVar('uID'));
        
        // TABLE NAME DIRECTIVES
        $this->addDirective('product','product');
        $this->addDirective('productCategoryList','productCategoryList');
        $this->addDirective('order','order');
        $this->addDirective('shippingMethod','shippingMethod');
        $this->addDirective('billingMethod','billingMethod');
        $this->addDirective('storeTransactionLog','storeTransactionLog');


        if (!isset($this->dbH)) {
            SM_fatalErrorPage("this module requires a database connection, which wasn't present!");
        }

    }

    // members only module
    function canModuleLoad() { 
        return $this->sessionH->isMember();
    }
     
    
    /**
    * @param (None)
    * @return (array) List of productCategory 
    */
    function getProductCategoryList() { 
        SM_debugLog("getProductCategoryList");
        $SQL = "SELECT idxNum,title FROM ". $this->directive['productCategoryList'] . " ORDER BY idxNum";
            $rh = $this->dbH->query($SQL);
            SM_dbErrorCheck($rh, $SQL);
            while($rr = $rh->fetch()) {
                $productCategoryList[$rr['idxNum']] = $rr['title'];
            }
            $rh = null;
            return $productCategoryList;
    }


    /**
    * @param status (string) 'active','inactive'
    * @param category_type (int) productCategoryList_idxNum
    * @return (hash) of products
    */
    function getProducts($status="active",$categoryType) { 
        $productCategoryListField = $this->directive['productCategoryList']."_idxNum";
        
        SM_debugLog("etProducts".$productCategoryListField);
        // Handle Category List Limit
        if(!empty($categoryType))
            $filter = "AND $productCategoryListField = $categoryType";
        
        $SQL = "SELECT idxNum,$productCategoryListField AS productCategoryList_idxNum,title,price,description
                FROM {$this->directive['product']}
                WHERE status = '$status'
                $filter";

        $rh = $this->dbH->query($SQL);
        SM_dbErrorCheck($rh, $SQL);
        while($rr = $rh->fetch()) {
            $productList[$rr['idxNum']] = $rr;
        }
        $rh = null;
        return $productList;
    }
    
    
    // LOG TRANSACTION

    // CHANGE SHIPPING ADDRESS
    // CHANGE BILLING ADDRESS

    // SHOW CURRENT ORDER
    // SHOW ORDER LIST (by member, by status)
    
    // MEMBER SERVICES
    // GET member status
    // SET member status
    // GET member type
    // SET member type
    // GET member location (city,state,zip,country,countryZone)


    /**
      * this function contains the core functionality entry point for the module.
      */
    function moduleThink() {

        // virtual module

    }
    
}

?>
