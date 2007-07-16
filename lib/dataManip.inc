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
 *
 * Author(s): Shannon Weyrick (weyrick@roadsend.com)
 * 
 * @package roadsend_siteManager 
 */
/**
 * verify that the passed string is a valid date format
 * a valid date format is (roughly):
 * 
 * if $reqAfterToday is true, it also makes sure that the
 * date passed falls AFTER today.
 * 
 * @param string $varDate the date to check
 * @param bool $reqAfterToday when true, verifies that the date passed falls after today
 * @return bool true if a valid date was passed, false otherwise
 */
function SM_verifyDate($varDate, $reqAfterToday=false) {

    if (ereg("^([0-9]{1,2}).([0-9]{1,2}).([0-9]{4})$", $varDate, $regs)) {
        if (!checkdate($regs[1],$regs[2],$regs[3])) {
            return false;
        }
        if ($reqAfterToday) {
            
            // date is valid date, but make sure it's today or after 
            $tDate = getDate(time());
            
            // current m/d/y 
            $month = $tDate["mon"];
            $day   = $tDate["mday"];
            $year  = $tDate["year"];
            
            // m/d/y passed 
            $pMonth = $regs[1]; 
            $pDay   = $regs[2];
            $pYear  = $regs[3];
            if ($pYear < $year) {
                return false;
            }
            if ($pYear == $year) {
                if ($pMonth < $month) {
                    return false;
                }
                if ($pMonth == $month) {
                    if ($pDay < $day) {
                        return false;
                    }
                    else {
                        return true;
                    }
                }
                else {
                    // it's this year, but some month in the future
                    return true;
                }
            }
            else {
                // it's a year in the future 
                return true;
            }
        }
        else {
            // date is valid, dont check whether its after today 
            return true;
        }
    }
    else {
        return false;
    }

}

/**
 * swap MM-DD-YYYY to YYYY-MM-DD for MySQL's pleasure
 * 
 * @param string $varDate the date string to convert
 * @return string the newly formated date
 */
function SM_toMysqlDate($varDate) {

    if (ereg("^([0-9]{1,2}).([0-9]{1,2}).([0-9]{4})$", $varDate, $regs)) {
        return $regs[3] . '-' . $regs[1] . '-' . $regs[2];
    }
    else
        return "(".$varDate.")";

}

/**
 * convert from MySQL date format (YYYY-DD-MM to MM-DD-YYYY) to US style
 * 
 * @param string $varDate the date to convert, in MySQL format
 * @return string the newly formatted date
 */
function SM_fromMysqlDate($varDate) {

    if (ereg("([0-9]{4}).([0-9]{1,2}).([0-9]{1,2})", $varDate, $regs)) {
        return $regs[2] . '/' . $regs[3] . '/' . $regs[1];
    }
    else
        return "(".$varDate.")";

}

/**
 * swap MySQL timestamp fomart (YYYYMMDDHHMMSS) to MM/DD/YYYY<br>HH:MM:SS (type=1) or MM/DD/YYYY (type=0)
 * 
 * @param string $tStamp MySQL timestamp value
 * @param string $type  flag which denotes how to format the return string. see description.
 * @return string newly formatted date string
 */
function SM_fromTimeStamp($tStamp, $type=1) {

    $year = substr($tStamp, 0, 4);
    $mon = substr($tStamp, 4, 2);
    $day = substr($tStamp, 6, 2);
    $hour = substr($tStamp, 8, 2);
    $min = substr($tStamp, 10, 2);
    $sec = substr($tStamp, 12, 2);

    $time = " am";

    if ($hour > 12) {
        $hour = $hour - 12;
        $time = " pm";
    }

    if ($type == 1)
        return $mon . "/" . $day . "/" . $year . "<br />" . $hour . ":" . $min . ":" . $sec . $time;
    else
        return $mon . "/" . $day . "/" . $year;

}


?>
