#include <SoftwareSerial.h>
SoftwareSerial XBee(2, 3); // RX, TX
const int ledPin1=8;
const int ledPin2=9;
const int ledPin3=10;
void setup() {
  XBee.begin(9600);
  Serial.begin(9600);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  digitalWrite(ledPin1,LOW);
  digitalWrite(ledPin2,LOW);
  digitalWrite(ledPin3,LOW);
}

// the loop function runs over and over again forever
void loop() {
  char brightness;

  //XBee.write("Hello world!\n");

  //if (XBee.available()) {
    // read the most recent byte (which will be from 0 to 255):
    brightness = XBee.read();
    // set the brightness of the LED:
    if(brightness=='H'){
      digitalWrite(ledPin1, HIGH);
      XBee.write("on\n");
    }
    else if(brightness=='L'){
      digitalWrite(ledPin1,LOW);     
      XBee.write("off\n");
    }

 // } // wait for a second
}
