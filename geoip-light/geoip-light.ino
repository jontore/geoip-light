/**
 * \file
 *       ESP8266 RESTful Bridge example
 * \author
 *       Tuan PM <tuanpm@live.com>
 */

#include <SoftwareSerial.h>
#include <espduino.h>
#include <rest.h>

#define LIGHTPIN 13

#define SSID ""
#define PASSWORD ""

#define DOMAIN "freegeoip.net"
#define PATH "/json/"
#define COUNTRY_CODE "CH"
SoftwareSerial debugPort(2, 3); // RX, TX

ESP esp(&Serial, &debugPort, 4);

REST rest(&esp);

boolean wifiConnected = false;

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if (res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if (status == STATION_GOT_IP) {
      debugPort.println("WIFI CONNECTED");
      wifiConnected = true;
    } else {
      wifiConnected = false;
    }

  }
}

void turnOnLedIfNoMatch(String str, String matchStr) {
  int matchIndex = str.indexOf(matchStr);  
  debugPort.println(str);
  debugPort.println(matchIndex);
  if (matchIndex == -1) {
    digitalWrite(LIGHTPIN, HIGH); 
  } else {
    digitalWrite(LIGHTPIN, LOW); 
  }
}

void setup() {
  Serial.begin(19200);
  debugPort.begin(19200);
  debugPort.println("INIT");
  pinMode(LIGHTPIN, OUTPUT);
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while (!esp.ready());

  debugPort.println("ARDUINO: setup rest client");
  if (!rest.begin(DOMAIN)) {
    debugPort.println("ARDUINO: failed to setup rest client");
    while (1);
  }

  /*setup wifi*/
  debugPort.println("ARDUINO: setup wifi");
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect(SSID, PASSWORD);
  debugPort.println("ARDUINO: system started");
}
char response[50] = "{country_code: CH}";
void loop() { 
  esp.process();
  if (wifiConnected) {
    rest.get(PATH);
    if (rest.getResponse(response, 50) == HTTP_STATUS_OK) {
      debugPort.println("ARDUINO: GET successful");
      turnOnLedIfNoMatch(response, COUNTRY_CODE);
    }
    delay(10000);
  }
}
