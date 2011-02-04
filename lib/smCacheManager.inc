<?php

/**
 *  Roadsend SiteManager
 *******************************************************************
 *  Copyright (c) 2001-2011 Roadsend, Inc.(http://www.roadsend.com)
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


/**
 * a class for accessing a caching system, using memcached
 * @author Shannon Weyrick <weyrick@roadsend.com>
 * @package roadsend_siteManager
 * @subpackage siteManager_inVar
 */
class SM_cacheManager extends SM_object {

    /**
     * @var int timeout for vars stored in memcached
     */
    protected $ttl = 0;
    
    /**
     * the memcached instance
     * @var Memcached
     */
    protected $mc = NULL;

    public function  __construct() {

        $this->_smoConfigure();

        // XXX note, this directive isn't set from anywhere yet
        if ($this->directive['memcachedID'] == '')
            $this->directive['memcachedID'] = 'default';

        $config  = $this->siteConfig->getSection('memcached', $this->directive['memcachedID']);

        if (!isset($config['host'])) {
            $this->fatalErrorPage("memcached host was not defined");
        }

        if (!isset($config['port'])) {
            $this->fatalErrorPage("memcached port was not defined");
        }

        // pull ttl from session config
        $this->ttl = $this->sessionH->directive['gcMaxLifeTime'];

        global $SM_siteID;
        $this->mc = new Memcached($SM_siteID);

        // only add the servers if there aren't any, since we are using persistent connections
        if (!count($this->mc->getServerList())) {
            $hostList = (array)$config['host'];
            $port = $config['port'];
            foreach ($hostList as $host) {
                if (!$this->mc->addServer($host, $port)) {
                    $this->fatalErrorPage('unable to connect to memcached server at '.$host.':'.$port);
                }
            }
            $this->mc->setOption(Memcached::OPT_NO_BLOCK, true);
            $this->mc->setOption(Memcached::OPT_TCP_NODELAY, true);
            //$this->mc->setOption(Memcached::OPT_LIBKETAMA_COMPATIBLE, true);
        }

    }

    public function set($key, $val, $ttl=-1) {
        if ($ttl < 0)
            $ttl = $this->ttl;
        return $this->mc->set($key, $val, $ttl);
    }

    public function get($key) {
        return $this->mc->get($key);
    }

    public function getCacheH() {
        return $this->mc;
    }

}


?>