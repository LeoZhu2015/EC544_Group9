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


int pin = 2;

#include <Servo.h>
 
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


}


double D_CAR(){
double a = getDistance();
double b = Get_Lidar();
double w = 13.0;
double l = 22.0;
double c = ((a+b+w)/2);
double __d = (l/(b-a));
double __f = atan(__d);
Serial.print("Ffffff : ");
Serial.println(__f);
double g = sin(__f);
double e = c*g;
delay(1000);
double halfofhall = 45;
if(e < 0){
double f = halfofhall + e;
Serial.print("D_CAR : ");
Serial.println(f);
double h = radToDeg(__f);
double Angle_desired = ((l/30)*60) ;
double Angle_car = h ;
double Angle_error = Angle_car - Angle_desired ;
double Angle_wheel = ((Angle_error/45)*60) ;
Serial.print("Wheel Angle");
Serial.println(Angle_wheel + 90);
wheels.write(Angle_wheel + 90);
}
if(e > 0){
  double f = halfofhall - e;
Serial.print("D_CAR : ");
Serial.println(f);
double h = radToDeg(__f);
double Angle_desired = ((l/30)*60) ;
double Angle_car = h;
double Angle_error = Angle_car - Angle_desired ;
double Angle_wheel = ((Angle_error/30)*60) ;
Serial.print("Wheel Angle");
Serial.println(Angle_wheel + 90);
wheels.write(Angle_wheel + 90);

}
//Serial.print("D_CAR : ");
//Serial.println(f);
Serial.print("LIDAR : ");
Serial.println(b);
Serial.print("IR : ");
Serial.println(a);

//double h = radToDeg(f);
//double Angle_desired = ((l/30)*60) ;
//double Angle_car = __f ;
//double Angle_error = Angle_car - Angle_desired ;
//double Angle_wheel = ((Angle_error/30)*60) ;
//Serial.print("Wheel Angle");
//Serial.println(Angle_wheel);


return e ;

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





double getDistance(){
  //8cm - 20cm
  double d;
  int RawADC = analogRead(pin);
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

