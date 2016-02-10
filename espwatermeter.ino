/*
Check maredana.nl for my domotica projects.
*/

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
  prevMillis = millis();
  Serial.print(curL);
  Serial.println(" L/m");
  Serial.print(totalL);
  Serial.println(" liter");
  Serial.println();
  pulseCountW = 0;
}
  // Serial debugging to determine trigger values
  /*
  Serial.print(sensorWater);
  Serial.println();
  delay(100);
  */
}



