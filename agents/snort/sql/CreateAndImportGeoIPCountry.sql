USE snort;

DROP TABLE IF EXISTS tGeoIPFromCSV;
CREATE TABLE tGeoIPFromCSV (
        cStartIP CHAR(15) NOT NULL,
        cEndIP CHAR(15) NOT NULL,
        uStartIP INT UNSIGNED NOT NULL,
        uEndIP INT UNSIGNED NOT NULL,
        cCountryCode CHAR(2) NOT NULL,
        cCountryName VARCHAR(50) NOT NULL
        );

DROP TABLE IF EXISTS tGeoIPCountryCode;
CREATE TABLE IF NOT EXISTS tGeoIPCountryCode (
        uGeoIPCountryCode TINYINT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
        /* uGeoIPCountryCode TINYINT UNSIGNED NOT NULL, */
        cCountryCode CHAR(2) NOT NULL,
        cCountryName VARCHAR(50) NOT NULL
        );

DROP TABLE IF EXISTS tGeoIP;
CREATE TABLE tGeoIP (
        uStartIP INT UNSIGNED NOT NULL,
        uEndIP INT UNSIGNED NOT NULL, UNIQUE (uEndIP),
        uGeoIPCountryCode TINYINT UNSIGNED NOT NULL
        );

LOAD DATA INFILE '/home/unxs/GeoIPCountryWhois.csv' 
        INTO TABLE tGeoIPFromCSV
        FIELDS TERMINATED BY ',' ENCLOSED BY '"' 
        LINES TERMINATED BY '\n' STARTING BY '' 
        ;

SET @uCount=0;
INSERT INTO tGeoIPCountryCode SELECT DISTINCT (@uCount:=@uCount+1),cCountryCode,cCountryName FROM tGeoIPFromCSV;

INSERT INTO tGeoIP SELECT uStartIP,uEndIP,uGeoIPCountryCode FROM tGeoIPFromCSV NATURAL JOIN tGeoIPCountryCode;

DROP TABLE IF EXISTS tGeoIPFromCSV;
