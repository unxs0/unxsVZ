## unxsVZ

Internet service provider eco system based on async job queues simply and effectively provided by MySQL itself.

Most of the software was generated by unxsRAD directly from MySQL schema. 
It is simple C with libmysql for small one shot cgi programs (no fastcgi style daemons required, no memory leak issues),
therefore Apache is used instead of nginx.

Slowly moving from these cost effective old school backends by unxsRAD to JQuery/Bootstrap client side interfaces.

### Project Status

Many of the sub dirs are standalone projects that are no longer being actively developed.

Recently moved all development and tracking to GitHub this will help setup the sub dir projects as individual projects as soon as time permits.

The unxsVZ multi datacenter virtualization and DevOps tool is not in a production ready state.
Other VMs (and container) support for GCE, DigitalOcean, AWS and Docker (direct and via Rancher) is being added slowly
and has broken most everything.

### Simple Install Instructions

#### 1-. Clone unxs0/unxsVZ GitHub repository on an Ubuntu or CentOS server.


#### 2-. Install required libs.

```
[root@hosting1 unxsVZ]# cd unxsTemplateLib
[root@hosting1 unxsTemplateLib]# make install

[root@hosting1 unxsTemplateLib]# cd ../unxsCIDRLib
[root@hosting1 unxsCIDRLib]# make install

[root@hosting1 unxsTemplateLib]# cd ../unxsAdmin
[root@hosting1 unxsAdmin]# make install

[root@hosting1 unxsTemplateLib]# cd ..
[root@hosting1 unxsVZ]# make install-libunxsvz
```

#### 3-. Install and harden MySQL.

#### 4-. Install Apache items.

You need an SSL server with content at /var/www/unxs/cgi-bin and /var/www/unxs/html

```
[root@hosting1 unxsVZ]# cd unxsAdmin
[root@hosting1 unxsAdmin]# mkdir /var/www/unxs/html/css
[root@hosting1 unxsAdmin]# mkdir /var/www/unxs/html/images
[root@hosting1 unxsAdmin]# mkdir /var/www/unxs/html/images/buttons
[root@hosting1 unxsAdmin]# mkdir /var/www/unxs/html/js
[root@hosting1 unxsAdmin]# cp -i css/*.css /var/www/unxs/html/css/
[root@hosting1 unxsAdmin]# cp -i js/*.js /var/www/unxs/html/js/
[root@hosting1 unxsAdmin]# cp -i images/* /var/www/unxs/html/images/
[root@hosting1 unxsAdmin]# cp -i images/buttons/* /var/www/unxs/html/images/buttons/

[root@hosting1 unxsAdmin]# cd ..
[root@hosting1 unxsVZ]# make install-all

[root@hosting1 unxsVZ]# mysql -e "create database unxsvz;"
[root@hosting1 unxsVZ]# mysql -e "grant all on unxsvz.* to unxsvz@localhost identified by 'wsxedc';"
[root@hosting1 unxsVZ]# zcat data/unxsvz.mysqldump.gz | mysql unxsvz
[root@hosting1 unxsVZ]# /var/www/unxs/cgi-bin/unxsVZ.cgi UpdateSchema
```

You should now be ready to access the unxsVZ backend at https://yoursite.io/cgi-bin/unxsVZ.cgi

User: Root

Passwd: as supplied in the data/unxsvz.mysqldump file: ThisIsAReallyBadPasswd

Login and click on the Main tab. Click on all the table links, if any have not been created they will the first time
you load them via the GET link.

### Production instructions

You should only use encrypted tunnels for MySQL traffic between remote servers and the MySQL db cluster.
