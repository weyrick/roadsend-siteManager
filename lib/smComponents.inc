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
 * @package roadsend_siteManager
 */
/** 
 * register a list of loadable components. these are non-critical
 * sections of code that will be loaded on the fly as needed      
 */
SM_registerComponent('SM_smartForm',    'smSmartForm.inc');
SM_registerComponent('SM_codePlate' ,   'smCodePlate.inc');
SM_registerComponent('SM_inVarClient',  'smInVarClient.inc');
SM_registerComponent('SM_memberSystem', 'smMembers.inc');

?>
