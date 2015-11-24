#include <StackArray.h>
#include <XBee.h>
#include <SoftwareSerial.h>
#include <SimpleTimer.h>

SoftwareSerial xbeeSerial(2, 3);

StackArray <int> stack;
//time object
SimpleTimer timer;
//SimpleTimer cleartimer;
//SimpleTimer broadcasttimer;

int green = 5;
int red = 4;
int blue = 6;
int button = 8;
int reading;
int process_packet = 0;

//Status flags
int BEACON_ID = 1;
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

void set_clearflag(){
  CLEARFLAG=0;
  Serial.println("cleartimer running");
}

void broadcaststatus(){
  sendTx(broadcastAddr);
  Serial.println();
  Serial.println("I am running, hey!");
  Serial.println("CLEARFLAG: ");
  Serial.println(CLEARFLAG);
  if(CLEARFLAG==1){
     COUNTER++;
  }
  if(COUNTER>3){
     CLEARFLAG=0;
     COUNTER=0;
  }  
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

      for (int i = 0; i < xbee.getResponse().getFrameDataLength(); i++) {
        Serial.print("frame data [");
        Serial.print(i, DEC);
        Serial.print("] is ");
        Serial.println(xbee.getResponse().getFrameData()[i], HEX);
      }
        
        TRANSMITTER_BEACON_ID = rx.getData()[0];
        if(TRANSMITTER_BEACON_ID != BEACON_ID){  
          process_packet = 1;        
        }
        else{
          process_packet = 0;
        }
       
        if(rx.getData()[1]==1&&LEADER_VALUE==0){
          INFECTED = 1;
        }

       if(rx.getData()[2]==1&&LEADER_VALUE==0&&INFECTED==1){
          CLEARFLAG = 1;
          INFECTED=0;
       }
       stack.push(rx.getData()[0]);
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("error code:");
    Serial.println(xbee.getResponse().getErrorCode());
  }

//  if(process_packet==1){
//    XBeeAddress64 replyAddress = rx.getRemoteAddress64();
//    sendTx(replyAddress);
//  }
}

void sendTx(XBeeAddress64 targetAddress) {
  uint8_t payload[] = {BEACON_ID, INFECTED, CLEARFLAG};
  ZBTxRequest zbTx = ZBTxRequest(targetAddress, payload, sizeof(payload));
  Serial.println("sending frame....");
  Serial.println("Info: ");
  Serial.print(BEACON_ID);
  Serial.print(INFECTED);
  Serial.print(CLEARFLAG);
  xbee.send(zbTx);
//  if (xbee.readPacket(500)) {
//    Serial.println("Got a response!");
//    // got a response!
//
//    // should be a znet tx status              
//    if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
//      xbee.getResponse().getZBTxStatusResponse(txStatus);
//
//      // get the delivery status, the fifth byte
//      if (txStatus.getDeliveryStatus() == SUCCESS) {
//        // success.  time to celebrate
//        Serial.println("SUCCESS!");
//      } else {
//        Serial.println("FAILURE!");
//        // the remote XBee did not receive our packet. is it powered on?
//      }
//    }
//  } else if (xbee.getResponse().isError()) {
//    Serial.print("Error reading packet.  Error code: ");  
//    Serial.println(xbee.getResponse().getErrorCode());
//  } else {
//    // local XBee did not provide a timely TX Status Response -- should not happen
//    Serial.println("This should never happen...");
//  }
}

void elect_leader(){
    Serial.println("Election performing");
    if(stack.isEmpty()){
      LEADER_VALUE=1;
      INFECTED=0;
    }
    else{
       while(!stack.isEmpty()){
         if(stack.pop()>BEACON_ID){
           LEADER_VALUE=0;
           break;
       }
         LEADER_VALUE=1;
         INFECTED=0;
       }
    }   
    if(LEADER_VALUE==1){
      LeaderLight();
    }
    while(!stack.isEmpty()){
      stack.pop();
    }
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
 timer.setInterval(3000, broadcaststatus);
 setStatus();
  Serial.println("listening for broadcast");
}

void loop() {
  
  // put your main code here, to run repeatedly:
  timer.run();
  reading = digitalRead(button);
  if (reading == LOW){
     if(LEADER_VALUE==1){
       CLEARFLAG=1;
     }
     else{
       INFECTED=1;
       InfectionLight();
     }         
  }
  else{
    if(xbee.readPacket(3000)){
       processResponse();
       setStatus();
    }
  }
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

