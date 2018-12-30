## Quick Install Instructions

### Brief Introduction

The bootstrap web app interface to a unxsRAD RAD4 created backend app
will be programatically generated from unxsRAD bootstrap source code templates.

This will happen at the same time the backend app source code is generated if unxsRAD
tTable has been populated with the required source code dummy tables.

For the app to work correctly, the css, js, image and font content will
need to  be copied from unxsRAD/interfaces/bootstrap dirs: js/ css/ images/ fonts/
into the web server content dirs of the same name in a root bs/ dir as per the
provided template files.

### Prerequisites/Notes

The provided template web apps are simple C cgi's that require an Apache style
https server that supports .cgi executables.

### Web server config

In order to provide multi app hosting on dev httpd servers, we use the simple
aliasing scheme (apache example):

```
#backend
ScriptAlias "/unxsAK/" "/var/www/cgi-bin/unxsAK.cgi"
#mobile app
ScriptAlias "/unxsAKApp/" "/var/www/unxs/html/unxsAK/mobile.cgi"
```

### Development Workflow Notes

#### Basics

unxsRAD creates files for this bootstrap app in this directory (*interfaces/bootstrap*) and in the *../../templates/default/bootstrap* dir.
You will need to _make install_ in the boostrap dir of the app as well as import into the app MySQL db the bootstrap templates.
You can do this for example via:

```
user@hosting bootstrap]# export unxsSalud=/var/local/unxsRAD/apps/unxsSalud 
[user@hosting bootstrap]# ../../utils/importTemplates.sh 
/var/www/cgi-bin
/var/local/unxsRAD/apps/unxsSalud

ImportTemplateFile(): Start
Updating tTemplate for Default.Header

Done

ImportTemplateFile(): Start
Updating tTemplate for Sangre.Body

Done

ImportTemplateFile(): Start
Updating tTemplate for Login.Body

Done

ImportTemplateFile(): Start
Updating tTemplate for Default.Footer

Done

ImportTemplateFile(): Start
Updating tTemplate for User.Body

Done
```

In some cases you may have to _MySQL ALTER_ some app db fields, or just drop tables and then use the backend to create them
with the new fields or field specs.

#### Workflow

 1. You need to create/edit *unxsRAD/templates/default/* template files.
 1. You then import the templates into the db via: *unxsVZ/unxsRAD/utils/importTemplates.sh*.
 1. You may have to create special functions in unxsVZ/unxsRAD/unxsrad.c to handle complex template sections.

#### unxsRAD template function development

unxsRAD/unxrad.c that is the main source for the /sbin/unxsRAD program contains special template expansion handler functions.
These work as follows:

 1. The template has to include a text expansion function e.g. *[[funcBootstrapEditorFields]]*.
 1. The unxsrad.c file has to then have a function declared _void funcBootstrapEditorFields(FILE *fp)_ and that is also included in the _void AppFunctions(FILE *fp,char *cFunction)_ call back hook.

#### Preloaded New Jobs

SQL for adding pre made jobs for use via QR code stickers on kites or bars.

```
insert into tJobOffer set uJobOffer=11321,uOwner=3,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW());
insert into tJobOffer set uJobOffer=11322,uOwner=3,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW());
insert into tJobOffer set uJobOffer=11323,uOwner=3,uCreatedBy=1,uCreatedDate=UNIX_TIMESTAMP(NOW());
update tJobOffer set cLabel=CONCAT('Preloaded New Job ',uJobOffer) where uOwner=3;

```
