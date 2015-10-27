/* 

http://pulsedlight3d.com

This sketch demonstrates getting distance with the LIDAR-Lite Sensor

It utilizes the 'Arduino I2C Master Library' from DSS Circuits:
http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library 

You can find more information about installing libraries here:
http://arduino.cc/en/Guide/Libraries


*/

#include <I2C.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
#include <math.h>

#include <Servo.h>

int pin1 = 1;
int pin2 = 2;


 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'








void setup(){
  Serial.begin(9600); //Opens serial connection at 9600bps.     
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails

  ////////////////////////////////////////////////

  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();
  
}


/* Convert degree value to radians */
double degToRad(double degrees){
  return (degrees * 71) / 4068;
}


/* Convert radian value to degrees */
double radToDeg(double radians){
  return (radians * 4068) / 71;
}

/* Calibrate the ESC by sending a high signal, then a low, then middle.*/
void calibrateESC(){
    esc.write(180); // full backwards
    delay(startupDelay);
    esc.write(0); // full forwards
    delay(startupDelay);
    esc.write(90); // neutral
    delay(startupDelay);
    esc.write(90); // reset the ESC to neutral (non-moving) value
}



void loop(){
  

D_CAR();
esc.write(70); // full forwards



}


double D_CAR(){
double a = getDistance_1();
double b = getDistance_2();
double w = 13.0;
double l = 22.0;
double c = ((a+b+w)/2);
double __d = (l/(b-a));
double __f = atan(__d);

if (abs(b-a) < 1){
  wheels.write(90);
}
else{
if (__f < 0){  // The car is going right
double orientation =  (90 - radToDeg(__f));
Serial.print("Orientation : ");
Serial.println(orientation);
double wheelwrite =  (orientation) ;
Serial.print("wheelwrite : ");
Serial.print(wheelwrite);
wheels.write(wheelwrite) ;

}

if( __f > 0){
double orientation = (90 - radToDeg(__f));
Serial.print("Orientation : ");
Serial.println(orientation); 
double wheelwrite = (orientation);
Serial.print("wheelwrite : ");
Serial.print(wheelwrite);
wheels.write(wheelwrite) ; 
}
  
}



Serial.print("IR_1 : ");
Serial.println(b);
Serial.print("IR_2 : ");
Serial.println(a);


}


double Get_Lidar(){
   // Write 0x04 to register 0x00
  uint8_t nackack = 100; // Setup variable to hold ACK/NACK resopnses     
  while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.write(LIDARLite_ADDRESS,RegisterMeasure, MeasureValue); // Write 0x04 to 0x00
    delay(1); // Wait 1 ms to prevent overpolling
  }

  byte distanceArray[2]; // array to store distance bytes from read function
  
  // Read 2byte distance from register 0x8f
  nackack = 100; // Setup variable to hold ACK/NACK resopnses     
  while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.read(LIDARLite_ADDRESS,RegisterHighLowB, 2, distanceArray); // Read 2 Bytes from LIDAR-Lite Address and store in array
    delay(1); // Wait 1 ms to prevent overpolling
  }
  int distanceLIDAR = (distanceArray[0] << 8) + distanceArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int
  
//Serial.print(distanceLIDAR);

  return distanceLIDAR ;
}





double getDistance_1(){
  //8cm - 20cm
  double d;
  int RawADC = analogRead(pin1);
  double Raw = (5.0/1023)*RawADC; 
//  Serial.print("Input: ");
//  Serial.print(Raw);
//  Serial.println(" V");
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

double getDistance_2(){
  //8cm - 20cm
  double d;
  int RawADC = analogRead(pin2);
  double Raw = (5.0/1023)*RawADC; 
//  Serial.print("Input: ");
//  Serial.print(Raw);
//  Serial.println(" V");
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


