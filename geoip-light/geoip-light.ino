#include <SoftwareSerial.h>
#include <espduino.h>
#include <rest.h>


#define DEBUG

#define LIGHTPIN 13

#define SSID ""
#define PASSWORD ""

#define DOMAIN "freegeoip.net"
#define PATH "/json/"
#define COUNTRY_CODE "CH"
#ifdef DEBUG
  SoftwareSerial debugPort(2, 3); // RX, TX
#endif

#ifdef DEBUG
ESP esp(&Serial, &debugPort, 4);
#else
ESP esp(&Serial, 4);
#endif

REST rest(&esp);

boolean wifiConnected = false;

void wifiCb(void* response)
{
  uint32_t status;
  RESPONSE res(response);

  if (res.getArgc() == 1) {
    res.popArgs((uint8_t*)&status, 4);
    if (status == STATION_GOT_IP) {
      #ifdef DEBUG
        debugPort.println("WIFI CONNECTED");
      #endif
      wifiConnected = true;
    } else {
      wifiConnected = false;
    }

  }
}

void turnOnLedIfNoMatch(String str, String matchStr) {
  int matchIndex = str.indexOf(matchStr);
  #ifdef DEBUG
    debugPort.println(str);
    debugPort.println(matchIndex);
  #endif
  if (matchIndex == -1) {
    digitalWrite(LIGHTPIN, HIGH);
  } else {
    digitalWrite(LIGHTPIN, LOW);
  }
}

void setup() {
  Serial.begin(19200);
  #ifdef DEBUG
    debugPort.begin(19200);
    debugPort.println("INIT");
  #endif
  pinMode(LIGHTPIN, OUTPUT);
  esp.enable();
  delay(500);
  esp.reset();
  delay(500);
  while (!esp.ready());

  #ifdef DEBUG
    debugPort.println("ARDUINO: setup rest client");
  #endif
  if (!rest.begin(DOMAIN)) {
    #ifdef DEBUG
      debugPort.println("ARDUINO: failed to setup rest client");
    #endif
    while (1);
  }

  /*setup wifi*/
  #ifdef DEBUG
    debugPort.println("ARDUINO: setup wifi");
  #endif
  esp.wifiCb.attach(&wifiCb);
  esp.wifiConnect(SSID, PASSWORD);
  #ifdef DEBUG
    debugPort.println("ARDUINO: system started");
  #endif
}

char response[50] = "";
void loop() {
  esp.process();
  if (wifiConnected) {
    rest.get(PATH);
    if (rest.getResponse(response, 50) == HTTP_STATUS_OK) {
      #ifdef DEBUG
        debugPort.println("ARDUINO: GET successful");
      #endif
      turnOnLedIfNoMatch(response, COUNTRY_CODE);
    }
    delay(10000);
  }
}
