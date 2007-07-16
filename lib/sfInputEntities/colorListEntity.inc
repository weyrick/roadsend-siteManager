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
 * Author(s): Jonathan Michel (pym@roadsend.com)
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
 *
 * @author Jon Michel <pym@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_smartForm
 *
 */ 
class colorListEntity extends selectEntity {

    
    /**
    * populate the list 
    */

    function populateList() {
        $this->addOption("None",'#FFFFFF');
        $this->addOption("Aliceblue","#F0F8FF");
        $this->addOption("Antiquewhite","#FAEBD7");
        $this->addOption("Aqua","#00FFFF");
        $this->addOption("Aquamarine","#7FFFD4");
        $this->addOption("Azure","#F0FFFF");
        $this->addOption("Beige","#F5F5DC");
        $this->addOption("Bisque","#FFE4C4");
        $this->addOption("Black","#000000");
        $this->addOption("Blanchedalmond","#FFEBCD");
        $this->addOption("Blue","#0000FF");
        $this->addOption("Blueviolet","#8A2BE2");
        $this->addOption("Brown","#A52A2A");
        $this->addOption("Burlywood","#DEB887");
        $this->addOption("Cadetblue","#5F9EA0");
        $this->addOption("Chartreuse","#7FFF00");
        $this->addOption("Chocolate","#D2691E");
        $this->addOption("Coral","#FF7F50");
        $this->addOption("Cornflowerblue","#6495ED");
        $this->addOption("Cornsilk","#FFF8DC");
        $this->addOption("Crimson","#DC143C");
        $this->addOption("Cyan","#00FFFF");
        $this->addOption("Darkblue","#00008B");
        $this->addOption("Darkcyan","#008B8B");
        $this->addOption("Darkgoldenrod","#B8860B");
        $this->addOption("Darkgray","#A9A9A9");
        $this->addOption("Darkgreen","#006400");
        $this->addOption("Darkkhaki","#BDB76B");
        $this->addOption("Darkmagenta","#8B008B");
        $this->addOption("Darkolivegreen","#556B2F");
        $this->addOption("Darkorange","#FF8C00");
        $this->addOption("Darkorchid","#9932CC");
        $this->addOption("Darkred","#8B0000");
        $this->addOption("Darksalmon","#E9967A");
        $this->addOption("Darkseagreen","#8FBC8F");
        $this->addOption("Darkslateblue","#483D8B");
        $this->addOption("Darkslategray","#2F4F4F");
        $this->addOption("Darkturquoise","#00CED1");
        $this->addOption("Darkviolet","#9400D3");
        $this->addOption("deeppink","#FF1493");
        $this->addOption("Deepskyblue","#00BFFF");
        $this->addOption("Dimgray","#696969");
        $this->addOption("Dodgerblue","#1E90FF");
        $this->addOption("Firebrick","#B22222");
        $this->addOption("Floralwhite","#FFFAF0");
        $this->addOption("Forestgreen","#228B22");
        $this->addOption("Fuchsia","#FF00FF");
        $this->addOption("Gainsboro","#DCDCDC");
        $this->addOption("Ghostwhite","#F8F8FF");
        $this->addOption("Gold","#FFD700");
        $this->addOption("Goldenrod","#DAA520");
        $this->addOption("Gray","#808080");
        $this->addOption("Green","#008000");
        $this->addOption("Greenyellow","#ADFF2F");
        $this->addOption("Honeydew","#F0FFF0");
        $this->addOption("Hotpink","#FF69B4");
        $this->addOption("Indianred","#CD5C5C");
        $this->addOption("Indigo","#4B0082");
        $this->addOption("Ivory","#FFFFF0");
        $this->addOption("Khaki","#F0E68C");
        $this->addOption("Lavender","#E6E6FA");
        $this->addOption("Lavenderblush","#FFF0F5");
        $this->addOption("Lawngreen","#7CFC00");
        $this->addOption("Lemonchiffon","#FFFACD");
        $this->addOption("Lightblue","#ADD8E6");
        $this->addOption("Lightcoral","#F08080");
        $this->addOption("Lightcyan","#E0FFFF");
        $this->addOption("Lightgoldenrodyellow","#FAFAD2");
        $this->addOption("Lightgreen","#90EE90");
        $this->addOption("Lightgrey","#D3D3D3");
        $this->addOption("Lightpink","#FFB6C1");
        $this->addOption("Lightsalmon","#FFA07A");
        $this->addOption("Lightseagreen","#20B2AA");
        $this->addOption("Lightskyblue","#87CEFA");
        $this->addOption("Lightslategray","#778899");
        $this->addOption("Lightsteelblue","#B0C4DE");
        $this->addOption("Lightyellow","#FFFFE0");
        $this->addOption("Lime","#00FF00");
        $this->addOption("Limegreen","#32CD32");
        $this->addOption("Linen","#FAF0E6");
        $this->addOption("Magenta","#FF00FF");
        $this->addOption("Maroon","#800000");
        $this->addOption("Mediumauqamarine","#66CDAA");
        $this->addOption("Mediumblue","#0000CD");
        $this->addOption("Mediumorchid","#BA55D3");
        $this->addOption("Mediumpurple","#9370D8");
        $this->addOption("Mediumseagreen","#3CB371");
        $this->addOption("Mediumslateblue","#7B68EE");
        $this->addOption("Mediumspringgreen","#00FA9A");
        $this->addOption("Mediumturquoise","#48D1CC");
        $this->addOption("Mediumvioletred","#C71585");
        $this->addOption("Midnightblue","#191970");
        $this->addOption("Mintcream","#F5FFFA");
        $this->addOption("Mistyrose","#FFE4E1");
        $this->addOption("Moccasin","#FFE4B5");
        $this->addOption("Navajowhite","#FFDEAD");
        $this->addOption("Navy","#000080");
        $this->addOption("Oldlace","#FDF5E6");
        $this->addOption("Olive","#808000");
        $this->addOption("Olivedrab","#688E23");
        $this->addOption("Orange","#FFA500");
        $this->addOption("Orangered","#FF4500");
        $this->addOption("Orchid","#DA70D6");
        $this->addOption("Palegoldenrod","#EEE8AA");
        $this->addOption("Palegreen","#98FB98");
        $this->addOption("Paleturquoise","#AFEEEE");
        $this->addOption("Palevioletred","#D87093");
        $this->addOption("Papayawhip","#FFEFD5");
        $this->addOption("Peachpuff","#FFDAB9");
        $this->addOption("Peru","#CD853F");
        $this->addOption("Pink","#FFC0CB");
        $this->addOption("Plum","#DDA0DD");
        $this->addOption("Powderblue","#B0E0E6");
        $this->addOption("Purple","#800080");
        $this->addOption("Red","#FF0000");
        $this->addOption("Rosybrown","#BC8F8F");
        $this->addOption("Royalblue","#4169E1");
        $this->addOption("Saddlebrown","#8B4513");
        $this->addOption("Salmon","#FA8072");
        $this->addOption("Sandybrown","#F4A460");
        $this->addOption("Seagreen","#2E8B57");
        $this->addOption("Seashell","#FFF5EE");
        $this->addOption("Sienna","#A0522D");
        $this->addOption("Silver","#C0C0C0");
        $this->addOption("Skyblue","#87CEEB");
        $this->addOption("Slateblue","#6A5ACD");
        $this->addOption("Slategray","#708090");
        $this->addOption("Snow","#FFFAFA");
        $this->addOption("Springgreen","#00FF7F");
        $this->addOption("Steelblue","#4682B4");
        $this->addOption("Tan","#D2B48C");
        $this->addOption("Teal","#008080");
        $this->addOption("Thistle","#D8BFD8");
        $this->addOption("Tomato","#FF6347");
        $this->addOption("Turquoise","#40E0D0");
        $this->addOption("Violet","#EE82EE");
        $this->addOption("Wheat","#F5DEB3");
        $this->addOption("White","#FFFFFF");
        $this->addOption("Whitesmoke","#F5F5F5");
        $this->addOption("Yellow","#FFFF00");
        $this->addOption("YellowGreen","#9ACD32");
        
    }

}

?>
