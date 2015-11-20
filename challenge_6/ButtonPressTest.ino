// One Button On/Off Toggle
const int buttonPin = 8;     // the number of the pushbutton pin
const int ledPin =  4;      // the number of the LED pin

// Variables will change:
int ledState = LOW;         // the current state of the output pin
int buttonState = LOW;             // the current reading from the input pin
int lastButtonState = HIGH;   // the previous reading from the input pin
int reading;

long lastDebounceTime = 0;  // the last time the output pin was toggled
long debounceDelay = 50;    // the debounce time; increase if the output flickers
void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
}
void loop() {
  reading = digitalRead(buttonPin);
   if (reading != lastButtonState) {
      lastDebounceTime = millis();
      lastButtonState = reading;
   } 

   if ((millis() - lastDebounceTime) > debounceDelay) {
       if (buttonState != lastButtonState) {
           buttonState = lastButtonState;
           if (buttonState == HIGH) {
                 ledState = !ledState;
                 digitalWrite(ledPin, ledState);
           }
       }
   }
}
