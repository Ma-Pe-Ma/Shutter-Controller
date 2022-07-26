# Shutter Controller - Browser Client
Before reading this through make sure you have read the [project’s ReadMe](/ReadMe.md).

You can log into the site by visiting the a the root of the server's address and entering the [proper credentials](../ESP8266_Server/Configuration.h).

As the certificate was manually generated, you have to confirm in your browser that your site can be trusted.

## Configuring
This client does not need specific configuring.

However the html files in this directory have to be included in the [server's code](../ESP8266_Server/src/BrowserClient.h).
Make sure that the quotation mark characters are escaped and add quotation marks at the start + end of the lines before copying to the ESP's source code. 

The [control page's](./Control.html) controlling javascript section is quite large so it got compressed by [Google's Closure Compiler](https://github.com/google/closure-compiler) it decreased the script's size by ~75% which speeded up loading the site. The aforelinked header contains this compressed version.

These versions are here for human-readers only as the compressed script and the escaped characters makes the header file hard to read (it is basically impossible to understand the compressed script that way).