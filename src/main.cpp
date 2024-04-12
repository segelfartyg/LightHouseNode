#include <SPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoHttpClient.h>

const char* ssid_router = "WIFIHUB_273D15";
const char* password_router = "26z2zpes";


const int ledPin1 = 22; 

char serverAddress[] = "192.168.0.33";  // server address
int port = 3000;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;

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
  Serial.println("making GET request");
  client.get("/interval");

  // read the status code and body of the response
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();

  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  Serial.println("Wait five seconds");
  delay(5000);
}