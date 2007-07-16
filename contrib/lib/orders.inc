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

// PAYMENT TYPES
define(CREDIT_CARD_PAYMENT,1);
define(CHECKING_PAYMENT,2);

class orders extends SM_object {

    // DB row number
    var $order_idxNum = 0;

    // MD5 - uniqueID
    var $orderID = '';

    // ID of shipping method
    var $shippingMethod_idxNum = 1;
    
    // ID of billing method
    var $paymentMethod_idxNum = CREDIT_CARD_PAYMENT;

    // members_idxNum (note NOT always current member)
    var $members_idxNum = '';

    // text of order (recipt)
    var $orderDescription = '';

    // subTotalPrice - price without shipping fees or taxes
    var $subTotalPrice = 0;

    // shipping fee
    var $shippingPrice = 0;

    // total price
    var $totalPrice = 0;

    //status (new','open','hold','return','complete','cancel','contested)
    // SHOULD BE DEFINED
    var $orderStatus = 'new';
    
    var $dateCreated = '';
    var $lastUpdated = '';

    var $errMSG = '';

    // List of items in the current order
    var $orderItemList = array();

    
    // CONSTRUCTOR
    function orders() { 
        $this->_smoConfigure();
    }
    
    /**
    * CREATE A NEW ORDER
    * @param
    * @return
    */
    function createOrder() { 
        $this->orderID = md5(uniqid(rand(),true));
        
        // If creating a new order for another member by a siteadmin default payment method to checking
        if (checkPriv('siteadmin')) {
            $this->paymentMethod_idxNum = CHECKING_PAYMENT;
        }
        return $this->orderID;
    }
    
    
    function cascadeDeleteOrder($orderID) {

        $SQL = "DELETE FROM orders WHERE orderID = '$orderID'";
        $rh = $this->dbH->query($SQL);
        SM_dbErrorCheck($rh, $SQL);
        
        $SQL = "DELETE FROM orderItem WHERE orders_orderID = '$orderID'";
        $rh = $this->dbH->query($SQL);
        SM_dbErrorCheck($rh, $SQL);

    }
    
    // SET OPEN ORDER
    function openOrder($orderID=NULL,$members_idxNum=NULL) {
        
        // Handle blanks
        if (empty($orderID)) {
            $orderID = $this->createOrder();
        } else {
            SM_debugLog("using open order when should be using load ($orderID)");
        }
        
        // CASCADE DELETE OTHER OPEN ORDERS/ITEMS for the members_idxNum
        $SQL = "SELECT orderID FROM orders 
                WHERE members_idxNum=$members_idxNum
                AND orderID != '$orderID'
                AND status IN ('new','open')";
        $rh = $this->dbH->query($SQL);
        SM_dbErrorCheck($rh, $SQL);
        
        while($rr = $rh->fetch()) {
            $this->cascadeDeleteOrder($rr['orderID']);
        }
        $rh = null;



        if (empty($members_idxNum)) {
            SM_fatalErrorPage("You need to declare a member to open an order");        
            $memberData = $SM_siteManager->sessionH->getMemberData();
            $this->members_idxNum = $memberData['idxNum'];
        } else {
            $this->members_idxNum = $members_idxNum;
        }

        $this->currentOrder = $orderID;
        // set status and save
        $this->orderStatus = 'open';
        $this->saveOrder($members_idxNum);

        return $this->currentOrder;
    }
    
    /**
    *@param (int) productID - idxNum of product in the catalog table
    *@param (int) quantity - Number of products to order
    *@param (string) existing record of order item in table
    *@return (string) $orderItemID
    */
    function addOrderItem($productID,$quantity=1,$options=NULL,$orderItemID=NULL,$title=NULL) { 
        SM_debugLog("addOrderItem $productID with title $title");

        // reference a new item
        $newOrderItem = new orderItem ($productID,$quantity,$options,$orderItemID,$title);
        SM_debugLog("Complete addOrderItem $productID");

        // add item to the current order's order list
        $this->orderItemList[$newOrderItem->orderItemID] = $newOrderItem; 
    
        return $orderItemID;
    }


    /**
    * @param members_idxNum
    * @return boolean
    */
    function saveOrder($members_idxNum=NULL) {
        global $SM_siteManager;

        // HANDLE AS TRANSACTION
         $this->subTotalPrice = 0;
         $this->totalPrice=0;
         $this->orderDescription = '';

        // Current Items (process first to build recipt and totals)
        foreach ($this->orderItemList as $orderItemID=>$itemInfo) {

            // Handle single quotes in products
            $productTitle = $SM_siteManager->dbH->quote($itemInfo->itemProductTitle);

            $SQL = "REPLACE INTO orderItem SET 
                    orders_orderID = '$this->orderID' ,
                    itemID = '$itemInfo->orderItemID' ,
                    product_idxNum = $itemInfo->itemProduct_idxNum,
                    product_title =  $productTitle,
                    productOptions =  '".$itemInfo->itemProductOptions."',
                    product_price =  '".$itemInfo->itemProductPrice."',
                    quantity = '".$itemInfo->itemQuantity."'";

            $rh = $SM_siteManager->dbH->query($SQL);
            SM_dbErrorCheck($rh, $SQL);

            $this->subTotalPrice += ($itemInfo->itemProductPrice * $itemInfo->itemQuantity);
            $dLine = "$itemInfo->itemProductTitle \t $itemInfo->itemProductPrice \t $itemInfo->itemQuantity \n";
            $this->orderDescription .= $dLine;

        }

        // Calculate total order price
        $this->totalPrice = $this->subTotalPrice + $this->subShippingPrice;

        // Handle single quotes in products
        $orderDescription = $SM_siteManager->dbH->quote($this->orderDescription);

        // Current Order
        $SQL = "REPLACE INTO orders SET orderID = '$this->orderID' ,
                members_idxNum = $this->members_idxNum ,
                shippingMethod_idxNum = $this->shippingMethod_idxNum ,
                paymentMethod_idxNum = $this->paymentMethod_idxNum ,
                description = $orderDescription ,
                status = '$this->orderStatus' ,
                subTotalPrice = $this->subTotalPrice,
                shippingPrice = $this->shippingPrice,
                totalPrice = $this->totalPrice,
                dateCreated = NOW()";
        $rh = $SM_siteManager->dbH->query($SQL);
        SM_dbErrorCheck($rh, $SQL);

        //COMMIT

        return true;

    }

    /**
    * @param none
    * @return (int) members_idxNum
    */
    function getMembers_idxNum() { 
    
        return $this->members_idxNum;
    }


    /**
    *
    * @param orderID;
    * @return (object) class orders
    */
    function loadOrder($orderID) {
        global $SM_siteManager;
    
        if (empty($orderID)) {
            $this->fatalErrorPage('You can not load an order with out an ID');
        }

        $SQL = "SELECT * FROM orders WHERE orderID = '$orderID' LIMIT 1";
        $rh = $SM_siteManager->dbH->query($SQL);
        SM_dbErrorCheck($rh, $SQL);
        $rr = $rh->fetch();
        
        // DB row number
        $this->order_idxNum = $rr['idxNum'];
        $this->orderID = $orderID;
        $this->shippingMethod_idxNum = $rr['shippingMethod_idxNum'];
        $this->paymentMethod_idxNum = $rr['paymentMethod_idxNum'];
        $this->members_idxNum = $rr['members_idxNum'];
        $this->orderDescription = $rr['description'];
        $this->subTotalPrice = $rr['subTotalPrice'];
        $this->shippingPrice = $rr['shippingPrice'];
        $this->totalPrice = $rr['totalPrice'];
        $this->orderStatus = $rr['status'];
        $this->dateCreated = $rr['dateCreated'];
        $this->lastUpdated = $rr['lastUpdated'];
    
        $rh = null;
        
        // ADD ITEMS TO ORDER
        $this->orderItemList = '';
        $SQL = "SELECT * FROM orderItem WHERE orders_orderID = '$orderID' LIMIT 50";
        $rh = $SM_siteManager->dbH->query($SQL);
        SM_dbErrorCheck($rh, $SQL);
        while($rr = $rh->fetch()) {    
            
            $productID = $rr['product_idxNum'];
            $orderItemID = $rr['itemID'];
            $quantity = $rr['quantity'];
            $options = $rr['productOptions'];

            // List of items in the current order
            $newOrderItem = new orderItem ($productID,$quantity,$options,$orderItemID);

            // add item to the current order's order list
            $this->orderItemList[$newOrderItem->orderItemID] = $newOrderItem; 
        }
    
        $rh = null;
        return true;
    }


    // CHANGE ORDER STATUS METHODS
    function setOrderStatus($status="new") {
        $this->orderStatus = $status;
        $this->saveOrder();
        return true;
    }

    // HOLD ORDER
    function holdOrder() {
        $this->setOrderStatus("hold");
        return true;
    }

    // CANCEL ORDER
    function cancelOrder() {
        $this->setOrderStatus("cancel");
        return true;
    }
    
    // COMPLETE ORDER
    function completeOrder() {
        $this->setOrderStatus("complete");
        return true;
    }
    

    /**
    *
    * @param (possible product categoy type - defaulted to material?)
    * @return boolan
    */
    function clearOrder() { 
    
        // remove all items (of type) from current order
    }

    // SET ORDER SHIPPING METHOD 

    /**
    * SET ORDER PAYMENT METHOD
    * @param (int) payment method
    * @return boolean
    */
    function setPaymentMethod($paymentMethod_idxNum) {
        $this->paymentMethod_idxNum = $paymentMethod_idxNum;
        return true;
    }
    
    // GET ORDER PAYMENT OPTIONS
    // GET ORER SHIPPING OPTIONS


    // FIXME: CLEAN NEW ORDERS  - if order is status NEW/OPEN it has no items, and is more then 
    // (define clean time) hour old delete order when a store item is loaded.


}

?>
