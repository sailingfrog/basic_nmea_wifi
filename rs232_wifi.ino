#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>
#include <ArduinoOTA.h>
#include <ESP8266mDNS.h>

#define UART_BAUD 38400
#define SOFT_UART_BAUD 4800

#define MODE_AP
//#define MODE_STA

#define bufferSize 8192

#ifdef MODE_AP
const char *ssid = "AIS_Wifi";  
const char *pw = "0123456789"; 
IPAddress ip(192, 168, 4, 1); 
IPAddress netmask(255, 255, 255, 0);
IPAddress broadcast(192,168,4,255);
const int port = 2000; 
#endif


#ifdef MODE_STA
const char *ssid = "your_ssid";  // Your ROUTER SSID
const char *pw = "your_passwd"; // and WiFi PASSWORD
const int port = 10110;
#endif

char buf[bufferSize];
int i;

WiFiUDP udp;

SoftwareSerial SoftSerial(0,2); // rx/tx

void setup() {

  ArduinoOTA.setHostname(ssid);
  
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  
  Serial.begin(UART_BAUD);
  SoftSerial.begin(SOFT_UART_BAUD);

  #ifdef MODE_AP 
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ip, ip, netmask);
    WiFi.softAP(ssid, pw);
  #endif

  #ifdef MODE_STA
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pw);
    while (WiFi.status() != WL_CONNECTED) {
      delay(100);
    }
  #endif

  ArduinoOTA.begin();
}

void loop() {
   ArduinoOTA.handle();
   if(Serial.available()) {
      i = Serial.readBytesUntil('\n',buf,bufferSize);
      buf[i]='\n';
      udp.beginPacket(broadcast, port);
      udp.write(buf,i+1);
      udp.endPacket();
      //Serial.write(buf,i+1);
    }

    if(SoftSerial.available()) {
      i = SoftSerial.readBytesUntil('\n',buf,bufferSize);
      buf[i]='\n';
      udp.beginPacket(broadcast, port);
      udp.write(buf,i+1);
      udp.endPacket();
      //Serial.write(buf,i+1);
    }
}
