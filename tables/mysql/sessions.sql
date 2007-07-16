# MySQL dump 8.8
#
# Host: localhost    Database: test
#--------------------------------------------------------
# Server version	3.23.22-beta-log

#
# Table structure for table 'sessions'
#

CREATE TABLE sessions (
  idxNum int(10) unsigned NOT NULL auto_increment,
  sessionID varchar(32) DEFAULT '' NOT NULL,
  hashID varchar(32) DEFAULT '' NOT NULL,
  createStamp datetime DEFAULT '0000-00-00 00:00:00' NOT NULL,
  remoteHost varchar(20),
  dataKey varchar(255) DEFAULT '' NOT NULL,
  dataVal TEXT,
  PRIMARY KEY (idxNum),
  KEY createStamp (createStamp),
  KEY sessionID (sessionID),
  UNIQUE hashID (hashID)
);



