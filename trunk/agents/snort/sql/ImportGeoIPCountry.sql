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

/* Modify as required */
LOAD DATA INFILE '/home/unxs/GeoIPCountryWhois.csv' 
        INTO TABLE tGeoIPFromCSV
        FIELDS TERMINATED BY ',' ENCLOSED BY '"' 
        LINES TERMINATED BY '\n' STARTING BY '' 
        ;

INSERT INTO tGeoIPCountryCode SELECT DISTINCT NULL,cCountryCode,cCountryName FROM tGeoIPFromCSV;

INSERT INTO tGeoIP SELECT uStartIP,uEndIP,uGeoIPCountryCode FROM tGeoIPFromCSV NATURAL JOIN tGeoIPCountryCode;

DROP TABLE IF EXISTS tGeoIPFromCSV;
