int green = 5;
//#include<SimpleTimer.h>

int red = 4;
int blue = 6;
int button = 8;


int reading;

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
    }
  } 
  else                                 //elif not leader        
  {
  	digitalWrite(green, HIGH);
	  digitalWrite(red, LOW);
	  digitalWrite(blue, LOW);
	  getMessasge();
    if(isCleared()){                   //-if clear message received
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
      digitalWrite(blue, LOW);
      clearInfection();
      delay(3000);//delay for immune 3secs
    }
    else{                              //-elif no clear message received
      if(reading == LOW)               //--if push botton
      {            
        sentInfectedMessage();
        digitalWrite(green, LOW);
        digitalWrite(red, HIGH);
        digitalWrite(blue, LOW);
      }
      else                             //--elif not push botton
      {
        if (isInfected())              //---if infect message received
        {
          sentInfectedMessage();//???????
          digitalWrite(green, LOW);
          digitalWrite(red, HIGH);
          digitalWrite(blue, LOW);
        }
        else                           //---elif no infect message received
        {
          digitalWrite(green, HIGH);
          digitalWrite(red, LOW);
          digitalWrite(blue, LOW);
        }
      }
    }
  }
}


bool isLeader(){

}

bool isCleared(){
  
}
bool isInfected(){

}


void clearInfection(){

}

void getMessage(){

}
void sentInfectedMessage(){

}
