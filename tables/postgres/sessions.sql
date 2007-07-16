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
