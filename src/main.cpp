#include <SPI.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoHttpClient.h>
#include <WebSocketsClient.h>
#include <secrets.h>

const char* ssid_router = SECRET_WIFI_SSID;
const char* password_router = SECRET_WIFI_PASSWORD;
const char *server = SERVER_IP;

const int ledPin1 = 22; 

int interval = 0;
int tick = 0;

char serverAddress[] = "192.168.0.33";  // server address
int port = 3000;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
int status = WL_IDLE_STATUS;
WebSocketsClient webSocket;


String dataReader(WiFiClient _httpclient)
{

  String response;
  bool responseReceived = false;
  bool timeOut = false;
  int retries = 0;

  while (!responseReceived && !timeOut)
  {

    while (_httpclient.available())
    {
      char c = _httpclient.read();
      response += c;
    }

    if (response.length() > 0)
    {
      responseReceived = true;
    }
    else
    {
      if (retries < 5)
      {
        delay(1000);
        retries++;
      }
      else
      {
        response = "INTERNAL READING ERROR (001)";
        ESP.restart();
        return response;
      }
    }
  }

  Serial.println(response);
  return response;
}

void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
	const uint8_t* src = (const uint8_t*) mem;
	Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
	for(uint32_t i = 0; i < len; i++) {
		if(i % cols == 0) {
			Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
		}
		Serial.printf("%02X ", *src);
		src++;
	}
	Serial.printf("\n");
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length){

	switch(type) {
		case WStype_DISCONNECTED:
			Serial.println("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED:
			Serial.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
			webSocket.sendTXT("Connected");
			break;
		case WStype_TEXT:
			Serial.printf("[WSc] get text: %s\n", payload);
      Serial.println(String((char *)payload));
      if(String((char *)payload) == "light"){
        Serial.print("HEU");
        digitalWrite(ledPin1, HIGH);
      }
      else{
        digitalWrite(ledPin1, LOW);
      }

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			Serial.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
		case WStype_ERROR:			
		case WStype_FRAGMENT_TEXT_START:
		case WStype_FRAGMENT_BIN_START:
		case WStype_FRAGMENT:
		case WStype_FRAGMENT_FIN:
			break;
	}

}

void setupWifi(){
  
  WiFi.disconnect();

  Serial.println("BEGINNING WIFI CONNECTION");
  WiFi.begin(ssid_router, password_router);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(WiFi.status());
    Serial.println("Connecting...");
    delay(500);
  }
  Serial.println("CONNECTED IP:");
  Serial.println(WiFi.localIP());
}


void setup() {



  Serial.begin(9600); 
  setupWifi();
  pinMode(ledPin1, OUTPUT);

    // TODO: ADD WS SUPPORT IF NECESSARY
  webSocket.begin(server, 3000, "/ws");
  webSocket.onEvent(webSocketEvent);
}

void runWsLoop(){

  while(true){
    webSocket.loop();
  }
}

void loop() {
runWsLoop();

tick++;
Serial.println(tick);


  if(interval == 0){
  Serial.println("making GET request");
  client.get("/interval");
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  interval = response.toInt();

  }

  if(tick >= interval * 50){
    digitalWrite(ledPin1, LOW);
  }

  if(tick >= interval * 100){
    tick = 0;
    digitalWrite(ledPin1, HIGH);
  }

}