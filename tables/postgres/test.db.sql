CREATE SEQUENCE news_idxnum_seq;
CREATE TABLE "news" (
	"idxNum" int4 DEFAULT nextval('news_idxnum_seq'::text) NOT NULL,
	"nDate" timestamp NOT NULL,
	"userName" character varying(50),
	"news" text,
	PRIMARY KEY ("idxNum")
);
CREATE SEQUENCE memberSessions_idxNum_seq;
CREATE TABLE "memberSessions" (
	"idxNum" int4 DEFAULT nextval('memberSessions_idxNum_seq'::text) NOT NULL,
	"uID" character(32) NOT NULL,
	"sID" character(32) NOT NULL,
	"dateCreated" timestamp NOT NULL,
	PRIMARY KEY ("idxNum")
);
CREATE UNIQUE INDEX "membersessions_uID_key" on "memberSessions" using btree ( "uID" "bpchar_ops" );
CREATE UNIQUE INDEX "membersessions_sID_key" on "memberSessions" using btree ( "sID" "bpchar_ops" );
CREATE SEQUENCE members_idxnum_seq;
CREATE TABLE "members" (
	"idxNum" int4 DEFAULT nextval('members_idxnum_seq'::text) NOT NULL,
	"uID" character(32) NOT NULL,
	"userName" character varying(30) NOT NULL,
	"passWord" character varying(30) NOT NULL,
	"emailAddress" character varying(60) NOT NULL,
	"firstName" character varying(50),
	"lastName" character varying(50),
	"dateCreated" timestamp NOT NULL,
	PRIMARY KEY ("idxNum")
);
CREATE UNIQUE INDEX "members_uID_key" on "members" using btree ( "uID" "bpchar_ops" );
CREATE UNIQUE INDEX "members_userName_key" on "members" using btree ( "userName" "varchar_ops" );
CREATE UNIQUE INDEX "members_emailAddress_key" on "members" using btree ( "emailAddress" "varchar_ops" );
CREATE SEQUENCE sessions_idxNum_seq;
CREATE TABLE "sessions" (
	"idxNum" int4 DEFAULT nextval('sessions_idxNum_seq'::text) NOT NULL,
	"sessionID" character varying(32) DEFAULT '' NOT NULL,
	"hashID" character varying(32) DEFAULT '' NOT NULL,
	"createStamp" timestamp NOT NULL,
	"remoteHost" character varying(20),
	"dataKey" character varying(255),
	"dataVal" text,
	PRIMARY KEY ("idxNum")
);
CREATE UNIQUE INDEX "sessions_hashID_key" on "sessions" using btree ( "hashID" "varchar_ops" );
CREATE  INDEX "sessionID_idx" on "sessions" using btree ( "sessionID" "varchar_ops" );
CREATE  INDEX "createStamp_idx" on "sessions" using btree ( "createStamp" "timestamp_ops" );
