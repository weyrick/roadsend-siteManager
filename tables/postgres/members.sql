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
