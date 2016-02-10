/*
Check maredana.nl for my domotica projects.
*/
#include "ESP8266WiFi.h"

// WiFi parameters
const char* ssid = "SSID";
const char* password = "PASSWORD";

// pimatic parameters
const char* PimaticUser = "admin";
const char* PimaticPassword = "admin";
const char* PimaticHost = "192.168.0.20";
const int PimaticPort = 8080;

// Sensor data
const int Water_C = 1;            // Rounds per liter
const int drempelWater = 350;     // trigger value watersensor ((high - low)/ 2 + low)
int sensorWater = 0;              // value of IR sensor
boolean pulseWater = false;       // water detection pulse
unsigned long pulseCountW = 0;    // pulse counter
unsigned long prevMillisW = 0;    // Water start time
unsigned long pulseTimeW = 0;     // Time elapsed for 1 pulse
int curL = 0;                     // Calculate current water usage
int totalL = 0;                   // Total amount of water used

unsigned long prevMillis = 0; 

void setup()  {
  Serial.begin(115200);             
  Serial.println("START"); 
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());      
}

void loop() {

  sensorWater = analogRead(1); // Analog pin with sensor attached

  if (sensorWater > drempelWater) {pulseWater = true;}

  if (sensorWater < drempelWater && pulseWater) { 
  pulseWater = false;
  pulseCountW++;
  pulseTimeW = (millis() - prevMillisW);
  prevMillisW = millis();
  curL = ( 60000 / pulseTimeW ) * Water_C;  // l/min
  totalL = pulseCountW * Water_C;
}

// Dump data every 5 minutes
if ((millis() - prevMillis) >= 300000) {
  Serial.print("Connecting to ");
  Serial.println(PimaticHost);
  WiFiClient client;
  if (!client.connect(PimaticHost, PimaticPort)) {
    Serial.println("connection failed");
    return;
  prevMillis = millis();
  Serial.print(curL);
  Serial.println(" L/m");
  Serial.print(totalL);
  Serial.println(" liter");
  Serial.println();
  
  String liter;
  liter = "{\"type\": \"value\", \"valueOrExpression\": \"" + String(curL) + "\"}";
    client.print("PATCH /api/variables/test1");
    client.print(" HTTP/1.1\r\n");
    client.print("Authorization: Basic ");
    client.print("YWRtaW46YWRtaW4="); //https://www.base64decode.org/
    client.print("\r\n");
    client.print("PimaticHost: mvegte.myqnapcloud.com\r\n");
    client.print("Content-Type:application/json\r\n");
    client.print("Content-Length: ");
    client.print(liter.length());
    client.print("\r\n\r\n");
    client.print(liter);
    delay(500);
    
  String totalliter;
  totalliter = "{\"type\": \"value\", \"valueOrExpression\": \"" + String(totalL) + "\"}";
    client.print("PATCH /api/variables/test2");
    client.print(" HTTP/1.1\r\n");
    client.print("Authorization: Basic ");
    client.print("YWRtaW46YWRtaW4="); //https://www.base64decode.org/
    client.print("\r\n");
    client.print("PimaticHost: mvegte.myqnapcloud.com\r\n");
    client.print("Content-Type:application/json\r\n");
    client.print("Content-Length: ");
    client.print(totalliter.length());
    client.print("\r\n\r\n");
    client.print(totalliter);
   
   delay(500);
  
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  pulseCountW = 0;
}
}
  // Serial debugging to determine trigger values
  /*
  Serial.print(sensorWater);
  Serial.println();
  delay(100);
  */
}



