#include <Servo.h>
#include <I2C.h>
#include <math.h>
#include <PID_v1.h>
#include<SoftwareSerial.h>
#include <XBee.h>

#define    LIDARLite_ADDRESS     0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure       0x00          // Register to write to initiate ranging.
#define    MeasureValue          0x04          // Value to initiate ranging.
#define    RegisterHighLowB      0x8f          // Register to get both High and Low bytes in 1 call.
#define    THETA                 60            // Max degree turning degree 
#define    Set_point              60            // Set distance from wall
#define    forwardspeed          77            // Running speed (backward)
#define    backwardspeed         100           // Running speed (forward)
#define    stop_distance         50            // Front IR stop_distance
#define    halfdist_safearea     10            // Half Distance of safe area
#define    NEUTRAL                95           //Neutral angle for the crawler
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle
ZBRxResponse rx = ZBRxResponse();
//ModemStatusResponse msr = ModemStatusResponse();

SoftwareSerial xbeeSerial(2,3);//RX,TX
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'
int sensorPins[] = {4,6}; // Array of pins connected to the sensor Power Enable lines
int sensorPinsArraySize = 2; // The length of the array
int irPin = 2;
int irPin1 = 3;
boolean autoMode_OR_ControlMode = true;
double irValue1;
double distanceA;
double distanceB;
double distanceAA;
double distanceBB;
double formerAA;
double formerBB;
double w = 13.0;           //width of the car
double l = 27.0;           //length of the car
//Define Variables we'll be connecting to
double Setpoint_Distance, Input_Distance, Output_Distance;

//Define the aggressive and conservative Tuning Parameters
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=0.9, consKi=0, consKd=0.1;

//Specify the links and initial tuning parameters
PID myPID_Distance(&Input_Distance, &Output_Distance, &Setpoint_Distance, consKp, consKi, consKd, DIRECT);
void setup(){
  Serial.begin(9600); //Opens serial connection at 115200bps.     
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
  
  for (int i = 0; i < sensorPinsArraySize; i++){
    pinMode(sensorPins[i], OUTPUT); // Pin to first LIDAR-Lite Power Enable line
//    Serial.print(sensorPins[i]);
  }

  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
  
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
  /*  If you're re-uploading code via USB while leaving the ESC powered on, 
   *  you don't need to re-calibrate each time, and you can comment this part out.
   */
  calibrateESC();
  myPID_Distance.SetMode(AUTOMATIC);
  myPID_Distance.SetOutputLimits(-40,60);
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
    if(xbee.readPacket(500)){
        processResponse();
        Serial.println("here here 1");
        delay(1000);
    }
    else{
      if(autoMode_OR_ControlMode){
        automoving();
        Serial.println("here here 2");
      }
      else{
        esc.write(90);
        delay(500);
        Serial.println("here here 3");
      }
    }
}

void processResponse() {
  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      // got a zb rx packet
      int command;
      // now fill our zb rx class
      xbee.getResponse().getZBRxResponse(rx);
      command = rx.getData(0);

      if (command == 120 ) { //autoMode_OR_ControlMode
        autoMode_OR_ControlMode = !autoMode_OR_ControlMode;
        Serial.print("Switch Mode\n");
      }

      else if (command == 119) {//w
        esc.write(90);
        esc.write(forwardspeed);
        delay(1000);
        Serial.print("forward\n");
      }
      // stop
      else if (command == 115) {//s
        esc.write(90);
        delay(1000);
        Serial.print("Stop\n");
      }
      // forward and left
      else if (command == 97) {//a
        wheels.write(135);
        esc.write(90);
        esc.write(forwardspeed);
        delay(1000);
        Serial.print("Left\n");
      }
      //forward and right
      else if (command == 100) {//d
        wheels.write(45);
        esc.write(90);
        esc.write(forwardspeed);
        delay(1000);
        Serial.print("Right\n");
      }
      //backward and left
//      else if (command == 122) {//z
//        wheels.write(135);
//        esc.write(90);
//        esc.write(backwardspeed);
//        delay(1000);
//        Serial.print("Backward and left\n");
//      }
      //backward and right
      else if (command == 99) {//c
        wheels.write(45);
        esc.write(90);
        esc.write(forwardspeed);
        delay(1000);
        Serial.print("Backward and right\n");
      }
      //backward
//      else if (command == 120) {//x
//        esc.write(90);
//        esc.write(backwardspeed);
//        delay(1000);
//        Serial.print("Backward\n");
//      }   
    }
  }
  else if (xbee.getResponse().isError()) {
    Serial.print("error code:");
    Serial.println(xbee.getResponse().getErrorCode());
  }
} 


void automoving(){
//    enableDisableSensor(sensorPins[0]); // Turn on sensor attached to pin 1 and disable all others
//    distanceAA=enableDisableSensor(sensorPins[1]);;  
//    enableDisableSensor(sensorPins[1]); //Turn on sensor attached to pin 2 and disable all others
    distanceB=enableDisableSensor(sensorPins[1]);

    Serial.println(distanceB);

    //****error checking code ****
//    if(distanceAA>0&&distanceBB>0){
//      distanceA = distanceAA;
//      distanceB = distanceBB;
//      if(distanceAA>400||distanceBB>400){
//        if(distanceAA>400&&formerAA>400){
//          distanceA = distanceAA;
//          distanceB = distanceBB;
//        }
//        else if(distanceBB>400&&formerBB>400){
//          distanceA = distanceAA;
//          distanceB = distanceBB;
//        }
//        else
//          distanceA = formerAA;
//          distanceB = formerBB;                 
//      }
//      else{
//        distanceA = distanceAA;
//        distanceB = distanceBB;
//      }
//      formerAA = distanceAA;
//      formerBB = distanceBB;
//    }
    //**** error checking code end ****

    if(distanceB>0){
      irValue1 = 0;
      irValue1 = getDistance(irPin1);
      Serial.println("IR value");
      Serial.println(irValue1);
      Serial.println("LIDAR dist");
      Serial.println(distanceB);
//      if(distanceB>300){ //slow down first
//         esc.write(80);
//      }
//      if(((irValue1)<80)&&(distanceB>350)&&(irValue1!=0)){
//            distanceB = irValue1;
//      }
      if(getDistance(irPin)>stop_distance){
        Serial.println("IR value hehehehe");
        Serial.println(getDistance(irPin));
        if(distanceB>400){
          if(irValue1>150){
            esc.write(90);
            Serial.print("front distance:");
            Serial.println(get_front_distance());
            if(get_front_distance()<300||get_front_distance()>700){
              Serial.println("Guai wan");
              //esc.write(85);
              //delay(1000);
              wheels.write(NEUTRAL);
              esc.write(backwardspeed);
              delay(5000);
              wheels.write(180);
              esc.write(forwardspeed);
              delay(6000);
            }
            else if(get_front_distance()>=300&&get_front_distance()<700){
              Serial.println("going straight");
              wheels.write(NEUTRAL);
              esc.write(forwardspeed);
              delay(4000);
            }
//          Serial.println("Guai wan");
//          esc.write(85);
//          delay(1000);
//          esc.write(backwardspeed);
//          delay(3000);
//          wheels.write(180);
//          esc.write(forwardspeed);
//          delay(6000);
          }
        }
        else{
          D_CAR(distanceB);       //Call the drive car function
          esc.write(forwardspeed);          // full forwards  
        }
      } 
      else if(abs(distanceB)<60){
          wheels.write(180);
          esc.write(backwardspeed);
          delay(2000);
          wheels.write(0);
          esc.write(forwardspeed);
          delay(3000);         
      }
      else if(abs(distanceB)>60){
          wheels.write(0);
          esc.write(backwardspeed);
          delay(2000);
          wheels.write(180);
          esc.write(forwardspeed);
          delay(3000);
      }
      else{
        esc.write(90);
        Serial.println("Stop when object in front (not corner)");
      }
    }  
}

double get_front_distance(){
  double sum=0.0;
  enableDisableSensor(sensorPins[0]);
  for(int i=0;i<5;i++){
    sum += (double)readDistance(); 
  }
  
  return sum/5;
}

void D_CAR(double b){
//  double c = ((a+b+w)/2);
//  double __d = (l/(a-b));
//  double __f = atan(__d);
//  double g = sin(__f);
//  double e = c*g;            //distance to the wall from center of the car

  double gap2 = Set_point-b;
  double theta;
  Serial.println("");
  Serial.println("SensorB: "+String(b));
//  if(abs(b-Set_point)<halfdist_safearea){
//      wheels.write(NEUTRAL);
//      Serial.print(NEUTRAL);
//      Serial.println("I am going straight");
//  }
   // else
    if(gap2>0){
      theta = distance_PID(abs(b));
      wheels.write(NEUTRAL- theta);
      Serial.print(NEUTRAL-theta);
      Serial.println("I am going right");
    }
    else if(gap2<0){
      theta = distance_PID(abs(b));
      wheels.write(NEUTRAL- theta);
      Serial.print(NEUTRAL-theta);
      Serial.println("I am going left");
    }

}

/*LIDAR 2 functions*/
double enableDisableSensor(int sensorPin){
  for (int i = 0; i < sensorPinsArraySize; i++){
      digitalWrite(sensorPins[i], LOW); // Turn off all sensors
  }
  digitalWrite(sensorPin, HIGH); // Turn on the selected sensor
  delay(1); // The sensor takes 1msec to wake
  return (double)readDistance();
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


double distance_PID(double dist){
 
    Input_Distance = dist;
    Setpoint_Distance = Set_point;
    double gap = abs(Setpoint_Distance - Input_Distance);//distance away from setpoint
//    Serial.print("gap is");
//    Serial.print(gap);
//    Serial.print("\n");
    //We will always use the conservative constants
    myPID_Distance.SetTunings(consKp, consKi, consKd);
    myPID_Distance.Compute();
    Serial.println("output");
    Serial.println(Output_Distance);
    return Output_Distance;
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
