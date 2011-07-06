<?php
/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001-2010 Roadsend, Inc.(http://www.roadsend.com)
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
 * simple DB wrapper for PDO DB classes
 * allows us to view SQL queries and do bench marking
 */
class SM_dbWrapper
{

    /**
     * are we benchmarking?
     */
    public $benchMarking = false;
    
    /**
     * last query we made
     */
    public $lastQuery = '';

    /**
     * query tracing?
     */
    private $queryTrace = false;
    
    /**
     * query runtimes, indexed by query SQL
     */
    private $queryTimes = array();

    /**
     * database handle
     */
    private $dbH = NULL;
    
    /**
     * my sitemanager db id
     */
    private $SM_id = 'unknown';
    
    /**
     * connect config
     */
    public $config = array();

    /**
     * simple counter
     */
    public $queryCount = 0;
    
    function __construct($id, $dsn, $user = NULL, $pass = NULL, $options = NULL) {
        $this->SM_id = $id;
        $this->config['dsn'] = $dsn;
        $this->config['userName'] = $user;
        $this->config['passWord'] = $pass;
        if (preg_match('/host=([^;]+);*/',$dsn,$m))
            $this->config['hostName'] = $m[1];
        if (preg_match('/dbname=([^;]+);*/',$dsn,$m))
            $this->config['dbName'] = $m[1];
        try {
            $this->dbH = new PDO($dsn,
                                 $user,
                                 $pass,
                                 $options);
        } catch (PDOException $e) {
            throw $e;
        }
        return $this->dbH;
    }

    function startBenchmarking() {
        require_once("Benchmark/Timer.php");
        $this->benchMarking = true;
    }

    function startQueryTrace() {
        $this->queryTrace = true;
    }

    // PDO wrapped functions ////////////////////////////////////////////////////////////
    // XXX had to wrap instead of extend to get around some strange reference bug
    function setAttribute ($attribute, $value) {
        return $this->dbH->setAttribute($attribute, $value);
    }

    function beginTransaction() {
        //SM_debugLog("DB [{$this->SM_id}]: started transaction");
        return $this->dbH->beginTransaction();
    }

    function commit() {
        //SM_debugLog("DB [{$this->SM_id}]: committed transaction");
        return $this->dbH->commit();
    }
    
    function rollBack() {
        SM_debugLog("DB [{$this->SM_id}]: rolled back transaction");
        return $this->dbH->rollBack();
    }

    function errorCode() {
        return $this->dbH->errorCode();
    }

    function errorInfo() {
        return $this->dbH->errorInfo();
    }

    function exec() {
        return $this->dbH->exec();
    }

    function getAttribute($atr) {
        return $this->dbH->getAttribute($atr);
    }

    function lastInsertId($name=NULL) {
        return $this->dbH->lastInsertId($name);
    }

    // NOTE: this is ripped from PEAR DB for compatibility
    // notably, that it returns 'NULL' when null
    function quote($in)
    {
        if (is_int($in) || is_double($in)) {
            return $in;
        } elseif (is_bool($in)) {
            return $in ? 1 : 0;
        } elseif (is_null($in)) {
            return 'NULL';
        } else {
            return $this->escapeSimple($in);
        }
    }

    // this calls the PDO version
    function escapeSimple($string) {
        return $this->dbH->quote($string);
    }

    ////////////////////////////////////////////////////////////////////////////////////

    /**
     * performa query
     * @param $query string SQL statement
     * @return PDOStatement statement handle, or null on error
     */
    function query($query)
    {

        // so that siteManager knows the last ID in use
        $GLOBALS['SM_LAST_DB_ID'] = $this->SM_id;

        $this->queryCount++;
        
        // unless we're benchmarking, just record the last query (for errors) and go to parent
        if (!$this->benchMarking) {
            $this->lastQuery = $query;
            if ($this->queryTrace)
                SM_debugLog("DB [{$this->SM_id}]: $query");
            $rh = $this->dbH->query($query);
            if ($rh)
                $rh->setFetchMode(PDO::FETCH_ASSOC);
            return $rh;
        }
        
        $bt = debug_backtrace(false);
        foreach ($bt as $btid => $btdata) {
            // chop from admin/ or from smPortal/
            $file = strstr($btdata['file'],'admin/');
            if (empty($file))
                $file = strstr($btdata['file'],'smPortal/');
            if (empty($file))
                $file = $btdata['file'];
            $qBT[$btid]['file'] = $file;
            $qBT[$btid]['line'] = $btdata['line'];
            $qBT[$btid]['function'] = $btdata['function'];
            $qBT[$btid]['class'] = $btdata['class'];
        }

        $this->queryTimes[$query]['backTrace'] =  $qBT;
        $this->queryTimes[$query]['SQL'] = $query;
        $this->queryTimes[$query]['id'] = ++$GLOBALS['DB_PROFILE_ID'];

        if ($this->queryTimes[$query]['id'] == $_GET['SM_dbpHalt']) {
            SM_fatalErrorPage("halt on ID:".$query);
        }
        
        // get start byte count
        if (stristr($query,'select')) {
            $Brh = $this->dbH->query('SHOW SESSION STATUS LIKE "Bytes%"');
            while ($rr = $Brh->fetch()) {
                if ($rr['Variable_name'] == 'Bytes_received')
                    $this->queryTimes[$query]['br_start'] = $rr['Value'];
                if ($rr['Variable_name'] == 'Bytes_sent')
                    $this->queryTimes[$query]['bs_start'] = $rr['Value'];
            }
        }
        
        $timer = new Benchmark_Timer();
        $timer->start();
        $rh = $this->dbH->query($query);
        $timer->stop();
        $td = $timer->getProfiling();
        $this->queryTimes[$query]['total'] = $td[1]['total'];
        if (is_object($rh))
            $this->queryTimes[$query]['numRows'] = $rh->rowCount();
        
        if (stristr($query,'select')) {
            $Brh = $this->dbH->query('SHOW SESSION STATUS LIKE "Bytes%"');
            while ($rr = $Brh->fetch()) {
                if ($rr['Variable_name'] == 'Bytes_received')
                    $this->queryTimes[$query]['br_stop'] = $rr['Value'];
                if ($rr['Variable_name'] == 'Bytes_sent')
                    $this->queryTimes[$query]['bs_stop'] = $rr['Value'];
            }
        }

        if ($rh)
            $rh->setFetchMode(PDO::FETCH_ASSOC);
        return $rh;
        
    }

    
    function getProfileReport() {

        // do byte counts
        $totalSent = 0;
        foreach ($this->queryTimes as $SQL => $data) {
            $bytes_sent = $data['bs_stop']-$data['bs_start'];
            $bytes_recv = $data['br_stop']-$data['br_start'];
            $this->queryTimes[$SQL]['sent'] = $bytes_sent;
            $this->queryTimes[$SQL]['recv'] = $bytes_recv;
            $totalSent += $bytes_sent;
        }

        // sort
        switch ($_GET['SM_dbpSort']) {
            case 'sql';
                $sortby = 'SQL';
                break;
            case 'bytes':
                $sortby = 'bytes sent';
                break;
            case 'rows':
                $sortby = 'number of results';
                break;
            default:
            case 'slow':
                $sortby = 'slowest query';
                break;
        }

        // do sort
        uasort($this->queryTimes, '_pdataSort');

        if ($_GET['SM_dbpSort'] == 'bytes') {
            $top = 20;
            foreach ($this->queryTimes as $SQL => $data) {
                if ($top--) {
                    $topcnt += $data['sent'];
                }
                else {
                    break;
                }
            }
        }
        
        // get total DB access time so we can do percents
        $totalDBtime = 0.0;
        foreach ($this->queryTimes as $SQL => $data) {
            $totalDBtime += $data['total'];
        }
        
        $msg = 'total queries: '.sizeof($this->queryTimes)." in {$totalDBtime} secs of DB time<br>\n";
        $msg .= 'total bytes sent: '.$totalSent."<br>\n";
        if ($_GET['SM_dbpSort'] == 'bytes') {
            $msg .= 'largest 20 queries comprise '.$topcnt." bytes<br>\n";
        }    
        $msg .= "sorted by $sortby:<br>\n";
        foreach ($this->queryTimes as $SQL => $data) {
            
            //$SQL = preg_replace("/\s+/",' ',$SQL);
            if ($_GET['SM_dbpTrunc'] == 1)
                $SQL = substr($SQL,0,50);                
            $SQL = str_replace('INSERT','<b>INSERT</b>',$SQL);
            $SQL = str_replace('UPDATE','<b>UPDATE</b>',$SQL);
            $SQL = str_replace('UPDATE','<b>REPLACE INTO</b>',$SQL);
            
            $pot = round(($data['total']/$totalDBtime) * 100,3);
            
            $bt = '';
            array_reverse($btData);
            foreach ($data['backTrace'] as $btData) {
                $bt .= $btData['file'].':'.$btData['line'].' calls '.$btData['class'].'::'.$btData['function']."\n";
            }
            
            $msg .= "<a name='bt{$data['id']}'><div class='sm_querybox'><div class='sm_sqlstats'>query #{$data['id']} ".$data['total']." secs ({$pot}%), {$data['numRows']} rows, {$data['sent']}b sent/{$data['recv']}b received\n";
            $msg .= "<a href='#bt{$data['id']}' onClick='document.getElementById(\"bt{$data['id']}\").style.display=\"inline\"'>show backtrace</a></div>";
            $msg .= "<div class='sm_sqlbt' id='bt{$data['id']}' style='display: none;'>$bt</div>";
            $msg .= "<div class='sm_sql'>$SQL</div></div>\n";
            
        }
        return $msg;
        
    }
    
}

function _pdataSort($a, $b) {
    switch ($_GET['SM_dbpSort']) {
        case 'sql';
            return strcasecmp($a['SQL'],$b['SQL']);
            //return levenshtein($a['SQL'],$b['SQL']);
            //return soundex($a['SQL'],$b['SQL']);
            break;
        case 'bytes':
            $sortby = 'bytes sent';
            return $a['sent'] < $b['sent'];
            break;
        case 'rows':
            $sortby = 'number of results';
            return $a['numRows'] < $b['numRows'];
            break;
        default:
        case 'slow':
            $sortby = 'slowest query';
            return $a['total'] < $b['total'];
            break;
    }
}

?>
