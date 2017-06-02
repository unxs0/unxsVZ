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
