#include <StackArray.h>
#include <XBee.h>
#include <SoftwareSerial.h>
#include <SimpleTimer.h>

SoftwareSerial xbeeSerial(2, 3);

StackArray <int> stack;
//time object
SimpleTimer timer;
SimpleTimer cleartimer;

int green = 5;
int red = 4;
int blue = 6;
int button = 8;
int reading;

//Status flags
int BEACON_ID = 4;
int LEADER_VALUE; // 1 if I am leader, 0 if I am not
int INFECTED;
int HOLD_ELECTION;
int CLEARFLAG; 
int TRANSMITTER_BEACON_ID;
int COUNTER=0;



XBee xbee = XBee();

XBeeResponse response  = XBeeResponse();    //create an xbee object

//create reusable objects for responses we expect to handle

ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

//  uint8_t command1[] = {'D','B'};  //command one
//  AtCommandRequest atRequest = AtCommandRequest(command1);

ZBTxStatusResponse txStatus = ZBTxStatusResponse();
AtCommandResponse atResponse = AtCommandResponse();

uint8_t payload[] = {BEACON_ID, INFECTED, CLEARFLAG};
XBeeAddress64 broadcastAddr = XBeeAddress64(0x00000000, 0x0000FFFF);
ZBTxRequest zbTx = ZBTxRequest(broadcastAddr, payload, sizeof(payload));

void setup() {
  // put your setup code here, to run once:
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(button, INPUT);
  
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
  Serial.println("Initializing beacon...");
  Serial.println(BEACON_ID);
  
  digitalWrite(green, LOW);
  digitalWrite(red, LOW);
  digitalWrite(blue, LOW);
  
  // initialize variables and flags
  LEADER_VALUE=0; // 1 if I am leader, 0 if I am not
 INFECTED=0;
 CLEARFLAG=0;
 //reading=1;
 
 timer.setInterval(30000, elect_leader);
 //cleartimer.setInterval(3000,set_clearflag);
  Serial.println("listening for broadcast");
  sendTx();

}

void set_clearflag(){
  CLEARFLAG=0;
  Serial.println("cleartimer running");
}
void processResponse() {

  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      // got a zb rx packet
//      response_count += 1;
//      Serial.println(response_count);
      Serial.println("xbee.getResponse is available");
      // now fill our zb rx class
      xbee.getResponse().getZBRxResponse(rx);

      Serial.println("Got an rx packet!");

      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
        // the sender got an ACK
        Serial.println("packet acknowledged");
      } else {
        Serial.println("packet not acknowledged");
      }

      Serial.print("checksum is ");
      Serial.println(rx.getChecksum(), HEX);

      Serial.print("packet length is ");
      Serial.println(rx.getPacketLength(), DEC);

      for (int i = 0; i < rx.getDataLength(); i++) {
        Serial.print("payload [");
        Serial.print(i, DEC);
        Serial.print("] is ");
        Serial.println(rx.getData()[i], HEX);
      }

//      for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
//        Serial.print("frame data [");
//        Serial.print(i, DEC);
//        Serial.print("] is ");
//        Serial.println(xbee.getResponse().getFrameData()[i], HEX);
//      }

        TRANSMITTER_BEACON_ID = rx.getData()[0];
       
        if(rx.getData()[1]==1&&LEADER_VALUE==0){
          INFECTED = 1;
        }
//       if (LEADER_ID == 0 ) {
//          LEADER_ID = rx.getData()[3];
//       }
       if(rx.getData()[2]==1){
          CLEARFLAG = 1;
          INFECTED=0;
       }
//       if(LEADER_ID==rx.getData()[0]){
//        //if message from leader, increase the counter
//        COUNTER++;
//       }
       stack.push(rx.getData()[0]);

   
       //pass some params
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("error code:");
    Serial.println(xbee.getResponse().getErrorCode());
  }

  sendTx();
  if(CLEARFLAG==1){
   // cleartimer.run();
  }
}

void sendTx() {
  uint8_t payload[] = {BEACON_ID, INFECTED, CLEARFLAG};
  ZBTxRequest zbTx = ZBTxRequest(broadcastAddr, payload, sizeof(payload));
  Serial.println("sending frame....");
  xbee.send(zbTx);

}

void elect_leader(){
      while(!stack.isEmpty()){
        Serial.print(stack.pop());
        if(stack.pop()>BEACON_ID){
          LEADER_VALUE=0;
          break;
        }
        LEADER_VALUE=1;
      }
     if(LEADER_VALUE==1){
      LeaderLight();
     }
     Serial.println("election");
//   else{
//      if(INFECTED==0){
//         NonLeaderLight();
//      }
//      else{
//        InfectionLight();
//      }
//     }
  }


void setStatus() {
  if (LEADER_VALUE==1)                      //if is leader
  {
    LeaderLight();

  } 
  else                                 //elif not leader        
  {
//    if(CLEARFLAG==1){                   //-if clear message received
//      INFECTED = 0;      
//    }
    if (INFECTED == 0)
    {
      NonLeaderLight();
    }
    else
    {
      InfectionLight();
//        delay(2000);
    } 
   }
}


void LeaderLight(){
    digitalWrite(green, LOW);
    digitalWrite(red, LOW);
    digitalWrite(blue, HIGH);
}
void InfectionLight(){
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
    digitalWrite(blue, LOW);
}

void NonLeaderLight(){
    digitalWrite(green, HIGH);
    digitalWrite(red, LOW);
    digitalWrite(blue, LOW);
}

void loop() {
  
  // put your main code here, to run repeatedly:
  timer.run();
  reading = digitalRead(button);
  if (reading == LOW){
    Serial.print(reading);
     if(LEADER_VALUE==1){
       CLEARFLAG=1;
       
       sendTx();
       //cleartimer.run();
     }
     else{
       INFECTED=1;
       InfectionLight();
       sendTx();
     }    
  }
  else{
    xbee.readPacket(2000);
    processResponse();
    setStatus();
  }
//  if(CLEARFLAG==1){
//    COUNTER++;
//  }
//  if(COUNTER>=50){
//    CLEARFLAG=0;
//    COUNTER=0;
//  }
 // Serial.println("BEACONID: ");
  //Serial.println(BEACON_ID);
//  Serial.println("INFECTED: ");
//  Serial.println(INFECTED);
//  Serial.println("CLEARFLAG: ");
//  Serial.println(CLEARFLAG);
//  Serial.println("Digitalbutton: ");
//  Serial.println(reading);
//  delay(1000);
}

