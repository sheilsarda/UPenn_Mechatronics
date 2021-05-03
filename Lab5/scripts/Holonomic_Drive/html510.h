/*
 * header for MEAM510 hacks for web interface
 * Jan 2021
 * Use at your own risk
 * 
 */
#define ssid     "Fios-rTWv7"
#define password "Govinda1!"
void attachHandler(String key, void (*handler)());
void serve(WiFiServer &server, const char *);
void sendhtml(String data) ;
void sendplain(String data);
int getVal();
