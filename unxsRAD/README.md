## unxsRAD 

Very preliminary version being built based on old MySQL C app code generator
that was laying around.

### MySQL Data Driven Rapid Application Developer

Paste in your MySQL schema, type in some other details and create your app code base.

Multi-tenant async job queue builds source code directories and ready to run apps
directly from unxsRAD data.

The job queue handler can be on same or any one of many remote servers.

You can paste in your MySQL CREATE TABLE code for quick start.

You can start coding right away in the per table {{table-name}}func.c
app files. 

Simple/fast: One shot compiled C cgi executable, for no memory issues
and interpreted language requirement/version/lib headaches.

You apps can be in node.js/python/go/ruby etc by extending the template
code accordingly to handle the output file extensions.

### Backend Nomenclature and Quick Notes

*t* prefix is for a table.
*u* prefix is for an unsigned int or the FK of a table (may show up as the FK.cLabel char string, or a unix timestamp (shown as a date).
*c* prefix is for a char string.

Main tab provides access to all schema.

### unxsRAD Installation

#### Non standard required libs

mysql-devel: libmysqlclient.a
unxsTemplateLib: libtemplate.a

#### Straightforward standalone C

```
make
vi local.h #set MySQL server items
vi Makefile #set cgi-bin and html paths
make install
```

#### httpd content notes

You must use unxsVZ/unxsAdmin images/css and js content
in your html dir

### User Guide

Nothing yet.

