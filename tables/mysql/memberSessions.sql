CREATE TABLE memberSessions (
idxNum	INT UNSIGNED NOT NULL AUTO_INCREMENT PRIMARY KEY,
uID	char(32) NOT NULL,
sID	char(32) NOT NULL,
dateCreated datetime not null,
UNIQUE(uID),
UNIQUE(sID)
)