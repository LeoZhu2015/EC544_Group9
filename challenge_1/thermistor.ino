#include <SoftwareSerial.h>
#include <math.h>

int pin = 0;
SoftwareSerial XBee(2, 3); // RX, TX

void setup() {
  // put your setup code here, to run once:
  XBee.begin(9600);
  Serial.begin(9600);
}

void loop() {
  int temperature = getTemp();
  String mytemp = String(temperature);
  String mymessage = String("D," + mytemp);
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

//
int getTemp(){
  int RawADC = analogRead(pin);
  long Resistance;
  double Temp;
  Resistance=((10240000/RawADC) - 10000);
/******************************************************************/
  /* Utilizes the Steinhart-Hart Thermistor Equation:        */
  /*    Temperature in Kelvin = 1 / {A + B[ln(R)] + C[ln(R)]^3}   */
  /*    where A = 0.001129148, B = 0.000234125 and C = 8.76741E-08  */
  /******************************************************************/
  Temp = log(Resistance);
  Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
  Temp = Temp - 273.15;  // Convert Kelvin to Celsius

  return Temp;
  
}

