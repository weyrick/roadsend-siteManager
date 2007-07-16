#
# Table structure for table 'members'
#
CREATE TABLE members (
  idxNum int(10) unsigned DEFAULT '0' NOT NULL auto_increment,
  uID	char(32) NOT NULL,
  userName varchar(30) DEFAULT '' NOT NULL,
  passWord varchar(30) DEFAULT '' NOT NULL,
  emailAddress varchar(60) DEFAULT '' NOT NULL,
  firstName varchar(50),
  lastName varchar(50),
  dateCreated datetime DEFAULT '0000-00-00 00:00:00' NOT NULL,
  PRIMARY KEY (idxNum),
  UNIQUE emailAddress (emailAddress),
  UNIQUE uid (uid),
  UNIQUE userName (userName)
);
