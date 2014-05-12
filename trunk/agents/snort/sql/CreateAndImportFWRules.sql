USE snort;

DROP TABLE IF EXISTS tFWRules;
CREATE TABLE IF NOT EXISTS tFWRules (
        uFWRules INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT,
        cRuleName VARCHAR(100) NOT NULL
        );

LOAD DATA INFILE '/home/unxs/snort.rules.csv' 
        INTO TABLE tFWRules
        FIELDS TERMINATED BY ',' ENCLOSED BY '"' 
        LINES TERMINATED BY '\n' STARTING BY '' 
        ;
