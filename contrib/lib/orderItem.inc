<?php

/*********************************************************************
*  Roadsend SiteManager
*  Copyright (c) 2001-2003 Roadsend, Inc.(http://www.roadsend.com)
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
*
*/
class orderItem  {
    
    // DB row
    var $orderItem_idxNum = 0;
    
    // new field
    var $orderItemID = '';
                    
     // idxNum for product added
    var $itemProduct_idxNum = '';
     
    // productCategoryList_idxNum 
    var $itemProductCategoryList_idxNum = '';
        
    // product title at time of order
    var $itemProductTitle = '';
    
    // product price at time of order
    var $itemProductPrice = '';
    
    // quantity of items orders
    var $itemQuantity = 1;
    
    // quantity of items orders
    var $itemProductOptions = '';

    
    // Contructor - creates a new item
    function orderItem($productID,$quantity=1,$options=NULL,$orderItemID=NULL,$title=NULL) {
        global $SM_siteManager;

        // Build a new Item
        if (empty($orderItemID)) {
            
            $this->orderItemID = md5(uniqid(rand(), true));            
            // get item data from catalog table  
            $SQL = "SELECT idxNum AS item_idxNum, 
                        title AS productTitle, 
                        price AS productPrice 
                    FROM product WHERE idxNum = $productID
                    LIMIT 1";
        
        // Lookup an existing item
        } else {
            $this->orderItemID = $orderItemID;
            // get item data from order item table
            $SQL = "SELECT product_idxNum AS item_idxNum, 
                        product_title AS productTitle, 
                        product_price AS productPrice 
                    FROM orderItem WHERE itemID = '$orderItemID'
                    LIMIT 1";
        }
        
        // Process query
        $rh = $SM_siteManager->dbH->query($SQL);
        SM_dbErrorCheck($rh, $SQL);
        $rr = $rh->fetch();
        
        // Set class vars
        
        // ALlow incoming title to be used instead of product title
        if (empty($title)){
            $this->itemProductTitle = $rr['productTitle'];
        } else {
            $this->itemProductTitle = $title;
        }
        
        
        $this->itemProduct_idxNum = $productID;            
        $this->itemQuantity = $quantity;
        $this->orderItem_idxNum = $rr['item_idxNum'];
        
        $this->itemProductPrice = $rr['productPrice'];
        $this->itemProductOptions = $options;



        // Clean record handle        
        $rh = null;
        return;
    }

}

?>
