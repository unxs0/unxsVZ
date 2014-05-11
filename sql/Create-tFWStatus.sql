/* create this table and load default data */

DROP TABLE IF EXISTS tFWStatus;

CREATE TABLE IF NOT EXISTS tFWStatus ( 
	uFWStatus INT UNSIGNED PRIMARY KEY AUTO_INCREMENT,
	cLabel VARCHAR(32) NOT NULL DEFAULT '',
	uOwner INT UNSIGNED NOT NULL DEFAULT 0, INDEX (uOwner),
	uCreatedBy INT UNSIGNED NOT NULL DEFAULT 0,
	uCreatedDate INT UNSIGNED NOT NULL DEFAULT 0,
	uModBy INT UNSIGNED NOT NULL DEFAULT 0,
	uModDate INT UNSIGNED NOT NULL DEFAULT 0 );

LOAD DATA INFILE '/home/unxs/tFWStatus.csv' 
        INTO TABLE tFWStatus;
        FIELDS TERMINATED BY ',' ENCLOSED BY '"' 
        LINES TERMINATED BY '\n' STARTING BY '' 
        ;

UPDATE tFWStatus SET uOwner=1,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW());
