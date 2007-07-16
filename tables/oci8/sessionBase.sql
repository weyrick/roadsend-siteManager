-- 
--  Table structure for table 'sessions'
-- 
CREATE TABLE sessions (
idxNum number(10)  NOT NULL ,
sessionID varchar2(32)  NOT NULL,
hashID varchar2(32)  NOT NULL,
createStamp number NOT NULL,
remoteHost varchar2(20),
dataKey varchar2(255),
dataVal varchar2(4000),
PRIMARY KEY (idxNum)
);

create index sessions_createStamp 
on sessions (createStamp);
create index sessions_sessionID 
on sessions (sessionID);
create unique index sessions_hashID 
on sessions (hashID);

CREATE SEQUENCE seq_idxNum_3958;
CREATE OR REPLACE TRIGGER autoinc_idxNum_3958
BEFORE INSERT ON sessions
FOR EACH ROW WHEN (new.idxNum is null)
BEGIN
   SELECT seq_idxNum_3958.nextval
   INTO :new.idxNum
       FROM dual;
	END autoinc_idxNum_3958;
/


-- 
--  Table structure for table 'members'
-- 
CREATE TABLE members (
idxNum number(10)  NOT NULL ,
userID varchar2(32)  NOT NULL,
userName varchar2(30)  NOT NULL,
passWord varchar2(30)  NOT NULL,
emailAddress varchar2(60)  NOT NULL,
firstName varchar2(50),
lastName varchar2(50),
dateCreated number NOT NULL,
PRIMARY KEY (idxNum)
);

create unique index members_userID
on members (userID);
create unique index members_emailAddress
on members (emailAddress);

CREATE SEQUENCE seq_idxNum_3960;
CREATE OR REPLACE TRIGGER autoinc_idxNum_3960
BEFORE INSERT ON members
FOR EACH ROW WHEN (new.idxNum is null)
BEGIN
   SELECT seq_idxNum_3960.nextval
   INTO :new.idxNum
       FROM dual;
	END autoinc_idxNum_3960;
/

--
--  Table structure for table 'memberSessions'
--

CREATE TABLE memberSessions (
idxNum	number(10)  NOT NULL,
USERID	varchar(32) NOT NULL,
SESSIONID	varchar(32) NOT NULL,
DATECREATED number not null,
PRIMARY KEY (idxNum)
);

create unique index memberSessions_USERID on memberSessions (USERID);
create unique index memberSessions_SESSIONID on memberSessions (SESSIONID);

CREATE SEQUENCE seq_idxNum_3959;
CREATE OR REPLACE TRIGGER autoinc_idxNum_3959
BEFORE INSERT ON memberSessions
FOR EACH ROW WHEN (new.idxNum is null)
BEGIN
   SELECT seq_idxNum_3959.nextval
   INTO :new.idxNum
       FROM dual;
	END autoinc_idxNum_3959;
/

