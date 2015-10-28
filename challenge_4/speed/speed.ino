/*
  Analog Input test for cyn70 - drc dec 2012
*/


int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  Serial.begin (9600);
}

void loop() {
  // read the value from the sensor:
  sensorValue = analogRead(sensorPin);    
delay(1000);
Serial.print ("Opto returning: "); 
Serial.print(sensorValue);
Serial.println();
}
