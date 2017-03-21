## unxsRAD Data
Install sample backend data including medical app example that compiles.

### Instructions

We need to create a database, and then import the data.
Do not accidently destroy important data!

```
mysql -pYourPasswd -uroot mysql -e "create database unxsrad";
mysql -pYourPasswd -uroot mysql -e "grant all on *.unxsrad to unxsrad@localhost identified by 'wsxedc'";
zcat /...somepath.../unxsRAD/data/unxsrad.mysqldump.gz | mysql -pwsxedc -uunxsrad unxsrad;
```

### Default App Login

Login to cgi app with user/passwd: Root/wsxedc
