#include <SPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <secrets.h>
#include <HttpClient.h>
#include <Ethernet.h>



const char* ssid_router = "WIFIHUB_273D15";
const char* password_router = "26z2zpes";


const int ledPin1 = 22; 


void setup() {
Serial.begin(9600); 

  Serial.println("BEGINNING WIFI CONNECTION");

WiFi.disconnect();
 WiFi.begin(ssid_router, password_router);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(WiFi.status());
    Serial.println("Connecting...");
    delay(500);
  }
  Serial.println("CONNECTED IP:");
  Serial.println(WiFi.localIP());

  pinMode(ledPin1, OUTPUT);

}

void loop() {

  String serverPath = SERVER_IP + "/interval";

  EthernetClient c;
  HttpClient http(c);

  const char kHostname[] = SERVER_IP;
  const char kPath[] = "/interval";
  int err = 0;
  err = http.get(kHostname, kPath);

  Serial.println(err);

  digitalWrite(ledPin1, HIGH); 
  delay(1000);
  digitalWrite(ledPin1, LOW); 
  delay(1000);

}
