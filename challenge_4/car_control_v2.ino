#include <Servo.h>
#include <I2C.h>
#include <math.h>
#include <PID_v1.h>
#include<SoftwareSerial.h>

#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.
#define    THETA                 60            // Max degree turning degree 
#define    Setpoint              80            // Set distance from wall
#define    forwardspeed          80            // Running speed (backward)
#define    backwardspeed         100           // Running speed (forward)
#define    stop_distance         40            // Front IR stop_distance
#define    halfdist_safearea     15            // Half Distance of safe area

SoftwareSerial XBee(2,3);//RX,TX
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int sensorPins[] = {4,6}; // Array of pins connected to the sensor Power Enable lines
int sensorPinsArraySize = 2; // The length of the array
int irPin = 2;
double distanceA;
double distanceB;
double w = 13.0;           //width of the car
double l = 27.0;           //length of the car
//Define Variables we'll be connecting to
//double Setpoint, Input, Output;

//Define the aggressive and conservative Tuning Parameters
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0, consKd=0.25;

//Specify the links and initial tuning parameters
//PID myPID(&Input, &Output, &Setpoint, consKp, consKi, consKd, DIRECT);
void setup(){
  XBee.begin(9600);
  Serial.begin(9600); //Opens serial connection at 115200bps.     
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
  
  for (int i = 0; i < sensorPinsArraySize; i++){
    pinMode(sensorPins[i], OUTPUT); // Pin to first LIDAR-Lite Power Enable line
//    Serial.print(sensorPins[i]);
  }
  
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();
  //myPID.SetMode(AUTOMATIC);
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
  char command;
  command = XBee.read();
  // moving forward
  if(command=='w'){
    esc.write(90);
    esc.write(forwardspeed);
    delay(2000);
  }
  // stop
  else if(command=='s'){
    esc.write(90);
    delay(2000);
  }
  // forward and left
  else if(command=='a'){
    wheels.write(135);
    esc.write(90);
    esc.write(forwardspeed);
    delay(2000);
  }
  //forward and right
  else if(command=='d'){
    wheels.write(45);
    esc.write(90);
    esc.write(forwardspeed);
    delay(1000);
  }
  //backward and left
  else if(command=='z'){
    wheels.write(135);
    esc.write(90);
    esc.write(backwardspeed);
    delay(1000);
  }
  //backward and right
  else if(command=='c'){
    wheels.write(45);
    esc.write(90);
    esc.write(backwardspeed);
    delay(1000);
  }
  //backward
  else if(command=='x'){
    esc.write(90);
    esc.write(backwardspeed);
    delay(2000);
  }
  else{
    enableDisableSensor(sensorPins[0]); // Turn on sensor attached to pin 1 and disable all others
    distanceA=(double)readDistance();  
    enableDisableSensor(sensorPins[1]); //Turn on sensor attached to pin 2 and disable all others
    distanceB=(double)readDistance();
    if(distanceA>0&&distanceB>0){
      if(getDistance(irPin)>stop_distance){
        D_CAR(distanceA,distanceB);       //Call the drive car function
        esc.write(forwardspeed);          // full forwards 
      } 
      else if(abs(distanceA-distanceB)>20){
        if(distanceA>distanceB){  
          wheels.write(135);        
          esc.write(backwardspeed);
          delay(1000);
          Serial.println("Object ahead, stop and back");
        }
        else{
          wheels.write(45);
          esc.write(backwardspeed);
          delay(1000);
          Serial.println("Object ahead, stop and back");         
        }
      }
      else{
        esc.write(90);
        Serial.println("Stop when object in front (not corner)");
      }
    }
  }  
}


void D_CAR(double a, double b){
  double c = ((a+b+w)/2);
  double __d = (l/(a-b));
  double __f = atan(__d);
  double g = sin(__f);
  double e = c*g;            //distance to the wall from center of the car
  double gap1 = Setpoint-a;
  double gap2 = Setpoint-b;
  double gap = Setpoint -abs(e);
  double theta;
  //Input = a;
  // myPID.SetTunings(consKp, consKi, consKd);
  // myPID.Compute();
  //Serial.println(Output);
  //Serial.println(__f);
  Serial.println("");
  Serial.println("SensorA: "+String(a));
  Serial.println("SensorB: "+String(b));
  if(a>250||b>250){                //Handle the long distance
    wheels.write(90);
    Serial.println("Distance >3m, do this");
  }
  else if(abs(a-b)>20){
    //Angle first
    if (__f < 0){                  // Let car go left
      double orientation =  (180 + radToDeg(__f));
      //Serial.print("Orientation : ");
      //Serial.println(orientation);
      double wheelwrite =  (orientation) ;
      //Serial.print("wheelwrite : ");
      //Serial.print(wheelwrite);
      wheels.write(wheelwrite) ;
      Serial.println("Angle first, go left");
    }
    else if( __f > 0){             // Let the car go right
      double orientation = (radToDeg(__f));
      //Serial.print("Orientation : ");
      //Serial.println(orientation); 
      double wheelwrite = (orientation);
      //Serial.print("wheelwrite : ");
      //Serial.print(wheelwrite);
      wheels.write(wheelwrite) ; 
      Serial.println("Angle first, go right");
    }
  }
  else if(abs(a-b)<=20){           //distance first   
    if (abs(gap)<5||abs(gap)<5){
      wheels.write(90);
      Serial.println("Distance first, go straight");
    }
    else if(gap>0){
      if(abs(gap)>double(halfdist_safearea)){
        theta = THETA;
      }else{
        theta = (gap/(double)halfdist_safearea)*THETA;
      } 
      wheels.write(90-theta);
      Serial.println("Distance first, go right");
    }
    else if(gap<0){
      if(abs(gap)>double(halfdist_safearea)){
        theta = THETA;
      }else{
        theta = (abs(gap)/(double)halfdist_safearea)*THETA;
      } 
      wheels.write(90+theta);
      Serial.println("Distance first, go left");
    }
  }
  else{    
    wheels.write(90);
  }
}

/*LIDAR 2 functions*/
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

//*IR sensor 1 function*/
double getDistance(int pinNum){
  double d;
  int RawADC = analogRead(pinNum);
  double RawADC_do = (double)RawADC;
  double Raw = (5.0/1023)*RawADC; 
  //Serial.print("Input: ");
  //Serial.print(Raw);
  //Serial.println(" V");
  if(Raw<1.25){
  //double distance = exp(8.5841-log(RawADC_do));
  //double distance_cm= 2.54*distance;     
  //return distance_cm;
    d = 509.6*exp((-4.652)*Raw)+118.6*exp((-0.7075)*Raw);
    return d; 
  }
  else if(1.25<=Raw<=2.0){
    d = (Raw-0.25)/50; 
    return (1.0/d); 
  }
  else if(2.0<Raw<2.52){
    d = (Raw-0.99053)/30.59; 
    return (1.0/d);  
  }
  else if(Raw>=2.52){
    d = (Raw-1.7555)/15.29;
    return ((1.0/d)); 
  } 
}
