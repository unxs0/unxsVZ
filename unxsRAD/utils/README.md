## unxsRAD utils

Scripts and other tools

### importTemplates.sh

Imports text file templates into unxsRAD database.
Requires installed unxsRAD.cgi and correctly configured tTemplateSet and tTemplateType.
Requires that you have template files in templates/<tTtemplateSet.cLabel>/<tTemplateType.cLabel>/

```
usage: importTemplates.sh <unxsRAD dir> <cgi-dir> [<template set> <template type>] | --help
        or export CGIDIR=/var/www/cgi-bin && export UNXSRAD=/home/unxs/unxsVZ/unxsRAD
        and then usage is only: ./utils/importTemplates.sh [<template set> <template type>]
        unxsRAD dir e.g. /home/unxs/unxsVZ/unxsRAD
        cgi-dir e.g. /var/www/cgi-bin
        ttemplate set default is default.
        ttemplate type default is RAD4
```
