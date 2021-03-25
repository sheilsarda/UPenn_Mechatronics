/*
 * header for MEAM510 hacks for web interface
 * Jan 2021
 * Use at your own risk
 * 
 */

// attach handler routine to URL parameter identifier
void attachHandler(String key, void (*handler)());

// check if client has submitted any requests and if so, execute handler
void serve(WiFiServer &server, const char *);


// routines to support HTTP messages
void sendhtml(String data) ;
void sendplain(String data);

// if the parameter includes values following "=" then return the next int value, can be comma separated
int getVal();
