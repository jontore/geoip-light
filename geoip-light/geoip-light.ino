#include <ESP8266WiFi.h>

#define SSID ""
#define PASSWORD ""

#define DOMAIN_URL "www.telize.com"
#define PATH "/geoip/"
#define COUNTRY_CODE_CH "\"country_code\":\"CH\""
#define COUNTRY_CODE_US "\"country_code\":\"US\""
#define LIGHT_PIN_CH 2
#define LIGHT_PIN_US 0

const char* host = DOMAIN_URL ;

void turnOnLedIfNoMatch(String str, String matchStr, int pinN) {
  int matchIndex = str.indexOf(matchStr);
  if (matchIndex == -1) {
    digitalWrite(pinN, LOW);
  } else {
    digitalWrite(pinN, HIGH);
  }
}


void connectToWifi (const char* ssid, const char* password) {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
}

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(LIGHT_PIN_CH, OUTPUT);
  pinMode(LIGHT_PIN_US, OUTPUT);

  // We start by connecting to a WiFi network
  connectToWifi(SSID, PASSWORD);
}

void loop() {
  delay(5000);

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = PATH;
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  delay(10);

  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\n');
    //Only check json object
    if(line.startsWith("{")) {
      turnOnLedIfNoMatch(line, COUNTRY_CODE_CH, LIGHT_PIN_CH);
      turnOnLedIfNoMatch(line, COUNTRY_CODE_US, LIGHT_PIN_US);
      
    }
  }
  
  Serial.println();
  Serial.println("closing connection");
}

