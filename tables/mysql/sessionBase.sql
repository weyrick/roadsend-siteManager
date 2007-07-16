
#
# Table structure for table 'memberSessions'
#

CREATE TABLE memberSessions (
idxNum	INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
uID	char(32) NOT NULL,
sID	char(32) NOT NULL,
dateCreated datetime not null,
UNIQUE(uID),
UNIQUE(sID)
);

#
# Table structure for table 'members'
#
CREATE TABLE members (
  idxNum int unsigned NOT NULL auto_increment primary key,
  uID	char(32) NOT NULL,
  userName varchar(30) DEFAULT '' NOT NULL,
  passWord varchar(30) DEFAULT '' NOT NULL,
  emailAddress varchar(60) DEFAULT '' NOT NULL,
  firstName varchar(50),
  lastName varchar(50),
  dateCreated datetime DEFAULT '0000-00-00 00:00:00' NOT NULL,
  UNIQUE emailAddress (emailAddress),
  UNIQUE uid (uid),
  UNIQUE userName (userName)
);

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

