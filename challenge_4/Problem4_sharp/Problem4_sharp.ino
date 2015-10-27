#include <math.h>

int pin = 2;

void setup() {
  Serial.begin(9600);
}

void loop() {
  double distance = getDistance();
Serial.print(distance);
Serial.println(" cm");
delay(1000);
}

//
double getDistance(){
  //8cm - 20cm
  double d;
  int RawADC = analogRead(pin);
  double Raw = (5.0/1023)*RawADC; 
  Serial.print("Input: ");
  Serial.print(Raw);
  Serial.println(" V");
  if(Raw<=2.0){
    d = (Raw-0.25)/50; 
  }
  else if(2.0<Raw<2.52){
    d = (Raw-0.99053)/30.59;  
  }
  else if(Raw>=2.52){
    d = (Raw-1.7555)/15.29;
  } 
  else{
    
  }
  return (1.0/d);  
}
