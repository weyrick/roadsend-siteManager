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
 *  a select box which is a list of a countries.
 *  INHERITS selectEntity
 *             
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm 
 */ 
class countryListEntity extends selectEntity {

    /** 
    * populate the list 
    */

    function populateList() {
            
        $this->addOption("USA");
        $this->addOption("Canada");
        $this->addOption("Afghanistan");
        $this->addOption("Albania");
        $this->addOption("Algeria");
        $this->addOption("American Samoa");
        $this->addOption("Andorra");
        $this->addOption("Angola");
        $this->addOption("Anguilla");
        $this->addOption("Antarctica");
        $this->addOption("Antigua and Barbuda");
        $this->addOption("Argentina");
        $this->addOption("Armenia");
        $this->addOption("Aruba");
        $this->addOption("Australia");
        $this->addOption("Austria");
        $this->addOption("Azerbaijan");
        $this->addOption("Bahamas");
        $this->addOption("Bahrain");
        $this->addOption("Bangladesh");
        $this->addOption("Barbados");
        $this->addOption("Belarus");
        $this->addOption("Belgium");
        $this->addOption("Belize");
        $this->addOption("Benin");
        $this->addOption("Bermuda");
        $this->addOption("Bhutan");
        $this->addOption("Bolivia");
        $this->addOption("Bosnia and Herzegovina");
        $this->addOption("Botswana");
        $this->addOption("Bouvet Island");
        $this->addOption("Brazil");
        $this->addOption("British Indian Ocean Territory");
        $this->addOption("British Virgin Islands");
        $this->addOption("Brunei");
        $this->addOption("Bulgaria");
        $this->addOption("Burkina Faso");
        $this->addOption("Burundi");
        $this->addOption("Cambodia");
        $this->addOption("Cameroon");
        $this->addOption("Cape Verde");
        $this->addOption("Cayman Islands");
        $this->addOption("Central African Republic");
        $this->addOption("Chad");
        $this->addOption("Chile");
        $this->addOption("China");
        $this->addOption("Christmas Island");
        $this->addOption("Cocos Islands");
        $this->addOption("Colombia");
        $this->addOption("Comoros");
        $this->addOption("Congo");
        $this->addOption("Cook Islands");
        $this->addOption("Costa Rica");
        $this->addOption("Croatia");
        $this->addOption("Cuba");
        $this->addOption("Cyprus");
        $this->addOption("Czech Republic");
        $this->addOption("Denmark");
        $this->addOption("Djibouti");
        $this->addOption("Dominica");
        $this->addOption("Dominican Republic");
        $this->addOption("East Timor");
        $this->addOption("Ecuador");
        $this->addOption("Egypt");
        $this->addOption("El Salvador");
        $this->addOption("Equatorial Guinea");
        $this->addOption("Eritrea");
        $this->addOption("Estonia");
        $this->addOption("Ethiopia");
        $this->addOption("Falkland Islands");
        $this->addOption("Faroe Islands");
        $this->addOption("Fiji");
        $this->addOption("Finland");
        $this->addOption("France");
        $this->addOption("French Guiana");
        $this->addOption("French Polynesia");
        $this->addOption("French Southern Territories");
        $this->addOption("Gabon");
        $this->addOption("Gambia");
        $this->addOption("Georgia");
        $this->addOption("Germany");
        $this->addOption("Ghana");
        $this->addOption("Gibraltar");
        $this->addOption("Greece");
        $this->addOption("Greenland");
        $this->addOption("Grenada");
        $this->addOption("Guadeloupe");
        $this->addOption("Guam");
        $this->addOption("Guatemala");
        $this->addOption("Guinea");
        $this->addOption("Guinea-Bissau");
        $this->addOption("Guyana");
        $this->addOption("Haiti");
        $this->addOption("Heard and McDonald Islands");
        $this->addOption("Honduras");
        $this->addOption("Hong Kong");
        $this->addOption("Hungary");
        $this->addOption("Iceland");
        $this->addOption("India");
        $this->addOption("Indonesia");
        $this->addOption("Iran");
        $this->addOption("Iraq");
        $this->addOption("Ireland");
        $this->addOption("Israel");
        $this->addOption("Italy");
        $this->addOption("Ivory Coast");
        $this->addOption("Jamaica");
        $this->addOption("Japan");
        $this->addOption("Jordan");
        $this->addOption("Kazakhstan");
        $this->addOption("Kenya");
        $this->addOption("Kiribati");
        $this->addOption("Korea, North");
        $this->addOption("Korea, South");
        $this->addOption("Kuwait");
        $this->addOption("Kyrgyzstan");
        $this->addOption("Laos");
        $this->addOption("Latvia");
        $this->addOption("Lebanon");
        $this->addOption("Lesotho");
        $this->addOption("Liberia");
        $this->addOption("Libya");
        $this->addOption("Liechtenstein");
        $this->addOption("Lithuania");
        $this->addOption("Luxembourg");
        $this->addOption("Macau");
        $this->addOption("Macedonia");
        $this->addOption("Madagascar");
        $this->addOption("Malawi");
        $this->addOption("Malaysia");
        $this->addOption("Maldives");
        $this->addOption("Mali");
        $this->addOption("Malta");
        $this->addOption("Marshall Islands");
        $this->addOption("Martinique");
        $this->addOption("Mauritania");
        $this->addOption("Mauritius");
        $this->addOption("Mayotte");
        $this->addOption("Mexico");
        $this->addOption("Micronesia, Federated States of");
        $this->addOption("Moldova");
        $this->addOption("Monaco");
        $this->addOption("Mongolia");
        $this->addOption("Montserrat");
        $this->addOption("Morocco");
        $this->addOption("Mozambique");
        $this->addOption("Myanmar");
        $this->addOption("Namibia");
        $this->addOption("Nauru");
        $this->addOption("Nepal");
        $this->addOption("Netherlands");
        $this->addOption("Netherlands Antilles");
        $this->addOption("New Caledonia");
        $this->addOption("New Zealand");
        $this->addOption("Nicaragua");
        $this->addOption("Niger");
        $this->addOption("Nigeria");
        $this->addOption("Niue");
        $this->addOption("Norfolk Island");
        $this->addOption("Northern Mariana Islands");
        $this->addOption("Norway");
        $this->addOption("Oman");
        $this->addOption("Pakistan");
        $this->addOption("Palau");
        $this->addOption("Panama");
        $this->addOption("Papua New Guinea");
        $this->addOption("Paraguay");
        $this->addOption("Peru");
        $this->addOption("Philippines");
        $this->addOption("Pitcairn Island");
        $this->addOption("Poland");
        $this->addOption("Portugal");
        $this->addOption("Puerto Rico");
        $this->addOption("Qatar");
        $this->addOption("Reunion");
        $this->addOption("Romania");
        $this->addOption("Russia");
        $this->addOption("Rwanda");
        $this->addOption("S. Georgia/S. Sandwich Isls.");
        $this->addOption("Saint Kitts & Nevis");
        $this->addOption("Saint Lucia");
        $this->addOption("Saint Vincent/The Grenadines");
        $this->addOption("Samoa");
        $this->addOption("San Marino");
        $this->addOption("Sao Tome and Princip");
        $this->addOption("Saudi Arabia");
        $this->addOption("Senegal");
        $this->addOption("Seychelles");
        $this->addOption("Sierra Leone");
        $this->addOption("Singapore");
        $this->addOption("Slovakia");
        $this->addOption("Slovenia");
        $this->addOption("Somalia");
        $this->addOption("South Africa");
        $this->addOption("Spain");
        $this->addOption("Sri Lanka");
        $this->addOption("St. Helena");
        $this->addOption("St. Pierre and Miquelon");
        $this->addOption("Sudan");
        $this->addOption("Suriname");
        $this->addOption("Svalbard/Jan Mayen Islands");
        $this->addOption("Swaziland");
        $this->addOption("Sweden");
        $this->addOption("Switzerland");
        $this->addOption("Syria");
        $this->addOption("Taiwan");
        $this->addOption("Tajikistan");
        $this->addOption("Tanzania");
        $this->addOption("Thailand");
        $this->addOption("Togo");
        $this->addOption("Tokelau");
        $this->addOption("Tonga");
        $this->addOption("Trinidad and Tobago");
        $this->addOption("Tunisia");
        $this->addOption("Turkey");
        $this->addOption("Turkmenistan");
        $this->addOption("Turks and Caicos Islands");
        $this->addOption("Tuvalu");
        $this->addOption("U.S. Minor Outlying Islands");
        $this->addOption("Uganda");
        $this->addOption("Ukraine");
        $this->addOption("United Arab Emirates");
        $this->addOption("United Kingdom");
        $this->addOption("Uruguay");
        $this->addOption("Uzbekistan");
        $this->addOption("Vanuatu");
        $this->addOption("Vatican City");
        $this->addOption("Venezuela");
        $this->addOption("Vietnam");
        $this->addOption("Virgin Islands");
        $this->addOption("Wallis and Futuna Islands");
        $this->addOption("Western Sahara");
        $this->addOption("Yemen");
        $this->addOption("Yugoslavia (Former)");
        $this->addOption("Zaire");
        $this->addOption("Zambia");
        $this->addOption("Zimbabwe");
                
    }

}

?>
