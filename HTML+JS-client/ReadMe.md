# Shutter Controller - HTML + JS Client
Before reading this through make sure you have read the [project’s ReadMe](/ReadMe.md).

You can log into the site by visiting the a the root of the server's address and entering the [proper credentials](../ESP8266_Server/Configuration.h).

As the certificate was generated manually, you have to confirm in your browser that your site can be trusted.

## Configuration
This client does not need specific configuring.

However the html files in this directory have to be included in the [server's code](../ESP8266_Server/src/BrowserClient.h).
Make sure that the quotation mark characters are escaped and add quotation marks at the start + end of the lines before copying to the ESP's source code. 

The resources for this page have to be served from an external server (see the [server's ReadMe](../ESP8266_Server/ReadMe.md#Setting-up-the-external-resource-provider-server))