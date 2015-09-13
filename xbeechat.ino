#include <SoftwareSerial.h>
#include <Thermistor.h>
Thermistor temp(0);
SoftwareSerial XBee(2, 3); // RX, TX

void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
}

void loop() {
  int temperature = temp.getTemp();
  String mytemp = String(temperature);
  String mymessage = String("A" + mytemp);
XBee.print(mymessage);
//XBee.print(temperature);
XBee.write("\n");
Serial.print(temperature);
Serial.println("*C");
delay(10000);

  // put your main code here, to run repeatedly:
//  XBee.write("Hello, World!\n");
//  Serial.write("Sent a message...\n");
//  delay(2000);
}
