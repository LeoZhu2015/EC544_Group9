int green = 5;
//#include<SimpleTimer.h>

int red = 4;
int blue = 6;
int button = 8;
int beacon_id = 1;
int leader_value; // 1 if I am leader, 0 if I am not


int reading;
int infected = 0;
//SimpleTimer simpleTimer;

void setup()
{
  //pinMode setup
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(button, INPUT);
  Serial.begin(9600);
  
  digitalWrite(green, LOW);
  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
//  timer1 = simpleTimer.setInterval(2000, sentInfectedMessage);
}

bool isLeader()
{
  xbee.write(beacon_id); // Send my beacon id to all other xbee's
  y = xbee.read(); // read beacon id's of other xbee's

  if (y = 2 || y= 3 || y = 4 || y = 5)
  { 
    return flase; // Some other beacon has higher id so i am not leader
  }
  else
  {  
    return ture; // I have higher beacon id than other beacon so i am leader
  }
  
}



bool isCleared(){
 return false;
}



void clearInfection(){

}

void getMessage(){

}
void sentInfectedMessage(){

}

void loop()
{
  //read the *button:
  //if this arduino is leader, *LOW to clear infect.
  //if not leader, set to be green first, *LOW to send "I am infected",
  //*High to see if it is infected by others, yes turn red, no keep green.
//  simpleTimer.run();
//  simpleTimer.disable(timer1);
  reading = digitalRead(button);

  if (isLeader())                      //if is leader
  {
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    digitalWrite(blue, HIGH);
    if (reading == LOW)                //-if push botton
    {      
      clearInfection();
      //digitalWrite(green, HIGH);
    }
  } 
else                                 //elif not leader        
  {
     if(reading == LOW)               //--if push botton
    { 
         infected = 1;
    }
    
    if (infected == 0)
    {
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
      digitalWrite(blue, LOW);
     // getMessasge();
    }
    if (infected == 1)
    {
        sentInfectedMessage();
        digitalWrite(green, LOW);
        digitalWrite(red, HIGH);
        digitalWrite(blue, LOW);
        delay(2000);
    }
     
    if(isCleared()){                   //-if clear message received
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
      digitalWrite(blue, LOW);
      clearInfection();
      infected = 0;
      delay(3000);//delay for immune 3secs
    }

      }
    }
  