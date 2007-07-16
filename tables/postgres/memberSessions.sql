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
