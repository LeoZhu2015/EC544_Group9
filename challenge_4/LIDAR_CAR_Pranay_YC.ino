#include <Wire.h>
#include <math.h>
#include <Servo.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.

int reading = 0;
int switch_pin1=4;
int switch_pin2=6;
double d_LIDAR1;
double d_LIDAR2;
 
Servo wheels; // servo for turning the wheels
Servo esc; // not actually a servo, but controlled like one!
bool startup = true; // used to ensure startup only happens once
int startupDelay = 1000; // time to pause at each calibration step
double maxSpeedOffset = 45; // maximum speed magnitude, in servo 'degrees'
double maxWheelOffset = 85; // maximum wheel turn magnitude, in servo 'degrees'

void setup(){
  Wire.begin(); // join i2c bus
  Serial.begin(9600); // start serial communication at 9600bps
  pinMode(switch_pin1, OUTPUT);
  pinMode(switch_pin2, OUTPUT);
  digitalWrite(switch_pin1,LOW);
  digitalWrite(switch_pin2,LOW);
  wheels.attach(8); // initialize wheel servo to Digital IO Pin #8
  esc.attach(9); // initialize ESC to Digital IO Pin #9
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
delay(500);
}


double D_CAR(){
//double a = getDistance_1();
//double b = getDistance_2();
for(int i=0;i<2;i++){
     if(i==0){ 
       digitalWrite(switch_pin1, HIGH); 
       digitalWrite(switch_pin2, LOW);
      // while(d_LIDAR1==0.0){
       d_LIDAR1 = (double)get_LIDAR_data();
      // }
       Serial.print("First LIDAR says:");  
       Serial.print(d_LIDAR1); 
       Serial.println();
     }
     else if(i==1){ 
       digitalWrite(switch_pin1, LOW); 
       digitalWrite(switch_pin2, HIGH);
      // while(d_LIDAR2==0.0){
       d_LIDAR2 = (double)get_LIDAR_data(); 
      // } 
       Serial.print("Second LIDAR says:");  
       Serial.print(d_LIDAR2); 
       Serial.println(); 
     }
}
double a = d_LIDAR1;
double b = d_LIDAR2;
double w = 13.0;
double l = 26.0;
double c = ((a+b+w)/2);
double __d = (l/(a-b));
double __f = atan(__d);

if (abs(a-b) < 5){
  wheels.write(90);
}
else{
if (__f < 0){  // The car is going right
double orientation =  (180 + radToDeg(__f));
Serial.print("Orientation : ");
Serial.println(orientation);
double wheelwrite =  (orientation) ;
Serial.print("wheelwrite : ");
Serial.print(wheelwrite);
wheels.write(wheelwrite) ;

}

if( __f > 0){
double orientation = radToDeg(__f);
Serial.print("Orientation : ");
Serial.println(orientation); 
double wheelwrite = (orientation);
Serial.print("wheelwrite : ");
Serial.print(wheelwrite);
wheels.write(wheelwrite) ; 
}
  
}

//Serial.print("IR_1 : ");
//Serial.println(b);
//Serial.print("IR_2 : ");
//Serial.println(a);


}

int get_LIDAR_data()
{
    Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterMeasure); // sets register pointer to  (0x00)  
  Wire.write((int)MeasureValue); // sets register pointer to  (0x00)  
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit

  Wire.beginTransmission((int)LIDARLite_ADDRESS); // transmit to LIDAR-Lite
  Wire.write((int)RegisterHighLowB); // sets register pointer to (0x8f)
  Wire.endTransmission(); // stop transmitting

  delay(20); // Wait 20ms for transmit

  Wire.requestFrom((int)LIDARLite_ADDRESS, 2); // request 2 bytes from LIDAR-Lite

  if(2 <= Wire.available()) // if two bytes were received
  {
    reading = Wire.read(); // receive high byte (overwrites previous reading)
    reading = reading << 8; // shift high byte to be high 8 bits
    reading |= Wire.read(); // receive low byte as lower 8 bits
    //Serial.println(reading); // print the reading
  }
}
