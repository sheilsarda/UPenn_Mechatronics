/*
 * MEAM510 hacks for web interface
 * Jan 2021
 * Use at your own risk
 * 
 * Mark Yim
 * University of Pennsylvania
 * copyright (c) 2021 All Rights Reserved
 */
#include <WiFi.h>
#include <WiFiClient.h>
#include "html510.h"
 
const char HTMLtext[] PROGMEM = R"===(HTTP/1.1 200 OK
Content-type:text/html

)==="; // note last newline is important  

const char plaintext[] PROGMEM = R"===(HTTP/1.1 200 OK
Content-type:text/plain

)==="; // note last newline is important  

WiFiClient client;

#define MAXHANDLERS 30
int numHandler=0;
void (*handlerptrs[MAXHANDLERS])();
String handlerpars[MAXHANDLERS];

void attachHandler( String key, void (*handler)() ) {
  char lastchar = key[key.length()-1];
  if (lastchar != ' ' && lastchar != '=') 
    key = key + " ";
  handlerptrs[numHandler] = handler;
  handlerpars[numHandler] = "GET "+key;
  numHandler++;
}

//*** HTML text handling routines ****
void sendhtml(String data) {
  String s = HTMLtext;
  s = s+data;
  client.print(s);
}

void sendplain(String data) {
  String s = plaintext;
  s = s+data;
  client.print(s);
}

int getVal(){
  String num = "";
  while (client.connected()) {
    if (client.available())  {
      char d = client.read();
      num += d;
      if (d !='-' && (d<'0' || d>'9') ) break;
    }        
  }
  return num.toInt();
}

//*** web server hack ***
// Could use Webserver.h routines here, but they are 100x  slower, so use a faster hack
// looks for key parameter at beginning of header, throws rest away
void serve(WiFiServer &server, const char *body) { 
  int breakflag=0; 
  client = server.available();
  if (client)  {
    String currentLine = "";

    while (client.connected()) {
      if (client.available())  {
        char c = client.read();
       // Serial.write(c);
        
        currentLine += c;
        if (breakflag || c== '\n') break; // break out of while and close connection 
        for (int i=0;i<numHandler; i++){
          if(currentLine.startsWith(handlerpars[i])) {
            (*handlerptrs[i])(); 
            breakflag=1;
          }
        }
      }
    }
    client.stop();    // Close the connection
  }
}
