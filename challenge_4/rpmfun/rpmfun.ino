int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor
volatile byte revolutions;
unsigned int rpmilli;
float speed;
unsigned long timeold;

void setup()
{
   Serial.begin(9600);
  revolutions = 0;
  rpmilli = 0;
  timeold = 0;
}

void loop()
{
   delay(200);
   if(disk_output() == 0)
   {
    revolutions++;
    Serial.print("revolutions");
     Serial.println(revolutions);
        if(revolutions >= 10)
          {
          rpm_fun(); 
          }
   }
  
 
}

void rpm_fun(){

    //Update RPM every 20 counts, increase this for better RPM resolution,
    //decrease for faster update
    double duration = (millis() - timeold)/1000 ;
    
    rpmilli = (60 /duration) * revolutions ;
      Serial.print("duration");
     Serial.println(duration);
    timeold = millis();
    revolutions = 0;

    
    Serial.print(" speed:");
    Serial.print(rpmilli * 0.026);
    Serial.println(" kmph");
  
}

int disk_output(){
 // read the value from the sensor:
  sensorValue = analogRead(sensorPin);    
  
  
  if(sensorValue > 600 ){
    return 1;
  }
  else{
    return 0;
  }
//Serial.print ("Opto returning: "); 
//Serial.println(sensorValue);
delay(100);

}


