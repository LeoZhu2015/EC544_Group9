/* 
http://pulsedlight3d.com

This sketch demonstrates getting distance from multiple LIDAR-Lite sensors by enabling the Power Enable line of the sensor 
to read from, and disabling the others.

It utilizes the 'Arduino I2C Master Library' from DSS Circuits:
http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library 

You can find more information about installing libraries here:
http://arduino.cc/en/Guide/Libraries
*/
#include <Servo.h>
#include <I2C.h>
#include <math.h>


#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.

Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int sensorPins[] = {1,2}; // Array of pins connected to the sensor Power Enable lines
int sensorPinsArraySize = 2; // The length of the array
int distanceA;
int distanceB;

void setup(){
  Serial.begin(9600); //Opens serial connection at 115200bps.     
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
  
  for (int i = 0; i < sensorPinsArraySize; i++){
    pinMode(sensorPins[i], OUTPUT); // Pin to first LIDAR-Lite Power Enable line
    Serial.print(sensorPins[i]);
  }
  
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
  enableDisableSensor(1); // Turn on sensor attached to pin 1 and disable all others
  Serial.print("SensorA ");
  Serial.print(readDistance()); // Read Distance from Sensor
  Serial.println(""); // Print "." to separate readings
  distanceA=readDistance();
  
  enableDisableSensor(2); //Turn on sensor attached to pin 2 and disable all others
  Serial.print("SensorB ");
  Serial.print(readDistance()); // Read Distance from Sensor
  Serial.println(""); // Print "." to separate readings
  distanceB=readDistance();
  
  
 // D_CAR((double)distanceA,(double)distanceB);
  //esc.write(70); // full forwards
}


double D_CAR(double a, double b){
  double w = 13.0;
  double l = 27.0;
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
}


void enableDisableSensor(int sensorPin){
  for (int i = 0; i < sensorPinsArraySize; i++){
      digitalWrite(sensorPins[i], LOW); // Turn off all sensors
  }
  digitalWrite(sensorPin, HIGH); // Turn on the selected sensor
  delay(1); // The sensor takes 1msec to wake
}

int readDistance(){
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
  int distance = (distanceArray[0] << 8) + distanceArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int
  
  return distance;   // Print Sensor Name & Distance
   
}
