## unxsRAD utils

Scripts and other tools

### importTemplates.sh

Imports text file templates into unxsRAD database.
Requires that you have template files in a dir like /some/path/templates/<tTtemplateSet.cLabel>/<tTemplateType.cLabel>/
The dir /some/path is the UNXSRAD env var.

#### Security hardening

Even though getuid() is used to restrict CLI to root user:

unxsRAD.cgi executable must not be available to non authorized users:

[root@h1 unxsRAD]# chown root:apache /var/www/cgi-bin/unxsRAD.cgi
[root@h1 unxsRAD]# chmod 750 /var/www/cgi-bin/unxsRAD.cgi

```
usage: importTemplates.sh <unxsRAD dir> <cgi-dir> [<template set>] | --help
        or export CGIDIR=/var/www/cgi-bin && export UNXSRAD=/home/unxs/unxsVZ/unxsRAD
        and then usage is only: ./utils/importTemplates.sh [<template set> <template type>]
        unxsRAD dir e.g. /home/unxs/unxsVZ/unxsRAD
        cgi-dir e.g. /var/www/cgi-bin
        ttemplate set default is default.
```
