int green = 5;
int red = 4;
int blue = 6;
int button = 8;


int reading;


void setup()
{
	pinMode(green, OUTPUT);
	pinMode(red, OUTPUT);
	pinMode(blue, OUTPUT);
	pinMode(button, INPUT);
	
	digitalWrite(green, LOW);
	digitalWrite(red, LOW);
	digitalWrite(blue, LOW);
}



  void loop()
{
  reading = digitalRead(button);
  if (isLeader)
  {
  	digitalWrite(green, LOW);
	digitalWrite(red, LOW);
	digitalWrite(blue, HIGH);
    if (reading == LOW)
    {
    	clearInfection();
    }

  }
  else
  {
  	digitalWrite(green, HIGH);
	digitalWrite(red, LOW);
	digitalWrite(blue, LOW);
	getMessasge();

	if(reading == LOW)
	{
		sentMessage();
		digitalWrite(green, LOW);
		digitalWrite(red, HIGH);
		digitalWrite(blue, LOW);
	}
	else
	{
		if (isInfected)
		{
	    	digitalWrite(green, LOW);
			digitalWrite(red, HIGH);
			digitalWrite(blue, LOW);
		}
		else
		{
			digitalWrite(green, HIGH);
			digitalWrite(red, LOW);
			digitalWrite(blue, LOW);
		}
	}

  }
}


bool isLeader(){

}

bool isInfected(){

}


void clearInfection(){

}

void getMessage(){

}
void sentMessage(){

}