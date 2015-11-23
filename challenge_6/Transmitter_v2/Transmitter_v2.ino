#include <XBee.h>
#include <SoftwareSerial.h>

SoftwareSerial xbeeSerial(2, 3);

int green = 5;
int red = 4;
int blue = 6;
int button = 8;
int reading;

//Status flags
int BEACON_ID = 1;
int leader_value; // 1 if I am leader, 0 if I am not
int INFECTED;
int LEADER_ID=0;
int HOLD_ELECTION;
int CLEARFLAG; 
int TRANSMITTER_BEACON_ID;
 
//SimpleTimer simpleTimer;


//const uint8_t MY_BEACON_ID = 4;
//boolean LEADER_ELECTED_FLAG;
//boolean I_AM_LEADER;
//boolean INFECTED_FLAG;
//uint8_t LEADER_BEACON_ID;
//uint8_t XMITTER_BEACON_ID;
//uint8_t MIN_BEACON_ID;
//int response_count;

XBee xbee = XBee();

XBeeResponse response  = XBeeResponse();    //create an xbee object

//create reusable objects for responses we expect to handle

ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

//  uint8_t command1[] = {'D','B'};  //command one
//  AtCommandRequest atRequest = AtCommandRequest(command1);

ZBTxStatusResponse txStatus = ZBTxStatusResponse();
AtCommandResponse atResponse = AtCommandResponse();

uint8_t payload[] = {BEACON_ID, HOLD_ELECTION, INFECTED, LEADER_ID,CLEARFLAG};
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
 leader_value=0; // 1 if I am leader, 0 if I am not
 INFECTED=0;
 HOLD_ELECTION=0;
 LEADER_ID=4;
 TRANSMITTER_BEACON_ID;
 CLEARFLAG=0;
 
//  LEADER_ELECTED_FLAG = 0;
//  I_AM_LEADER = 0;
//  INFECTED_FLAG = 0;
//  LEADER_BEACON_ID = 0;
//  MIN_BEACON_ID = 100;

  Serial.println("listening for broadcast");
  sendTx();
//  if (xbee.readPacket(10000)) {
//    processResponse();
//    Serial.println("initial response from network...");
//  } else {
//    elect_leader();
//    sendTx();
//    delay(1000);
//  }
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
//      //////////the change////////////////////////
//      XMITTER_BEACON_ID = rx.getData()[0];
//      LEADER_ELECTED_FLAG = rx.getData()[1];
//      INFECTED_FLAG = rx.getData()[2];
//      if (LEADER_BEACON_ID == 0) {
//        LEADER_BEACON_ID = rx.getData()[3];
//      }
//      if (XMITTER_BEACON_ID < MIN_BEACON_ID) {
//        // since we choose to elect the beacon with smallest beacon id as the leader, find the smallest beacon id
//        MIN_BEACON_ID = XMITTER_BEACON_ID;
//      }
        TRANSMITTER_BEACON_ID = rx.getData()[0];
        HOLD_ELECTION = rx.getData()[1];
        if(rx.getData()[2]==1){
          INFECTED = 1;
        }
       if (LEADER_ID == 0 ) {
        LEADER_ID = rx.getData()[3];
       }
        CLEARFLAG = rx.getData()[4];

      setStatus(BEACON_ID,LEADER_ID,INFECTED,CLEARFLAG); //pass some params
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("error code:");
    Serial.println(xbee.getResponse().getErrorCode());
  }
  // leader election algortihm
//  if (response_count == 0 || response_count > 10) {   //wait for 10 responses or if no response
//    if (!LEADER_ELECTED_FLAG) {                       // if no leader has been elected yet
//      if (MY_BEACON_ID < MIN_BEACON_ID) {             //if the beacon has smallest beacon ID
//        I_AM_LEADER = 1;                              //elect itself as the leader
//        LEADER_ELECTED_FLAG = 1;
//        LEADER_BEACON_ID = MY_BEACON_ID;
//        response_count = 0;
//        Serial.println("Leader Elected");
//      }
//    }
//  }



  sendTx();
}

void sendTx() {
  uint8_t payload[] = {BEACON_ID, HOLD_ELECTION, INFECTED, LEADER_ID,CLEARFLAG};
  ZBTxRequest zbTx = ZBTxRequest(broadcastAddr, payload, sizeof(payload));
  Serial.println("sending frame....");
  Serial.print("Leader ID: ");
  Serial.println(LEADER_ID);

  xbee.send(zbTx);

}

void elect_leader(){}


void setStatus(int my_id, int leader_id,int infected, int clearflag) {
    reading = digitalRead(button);

  if (leader_id==my_id)                      //if is leader
  {
    LeaderLight();
    if (reading == LOW)                //-if push botton
    {      
      //clearInfection();
      //digitalWrite(green, HIGH);
      CLEARFLAG=1;
    }
  } 
else                                 //elif not leader        
  {
    if(clearflag==1){                   //-if clear message received
      NonLeaderLight();
      //clearInfection(); 
      INFECTED = 0;
      delay(3000);//delay for immune 3secs
    }else{
//        if(reading == LOW)               //--if push botton
//      { 
//           INFECTED = 1;
//           infected=1;
//      }
      
      if (infected == 0)
      {
        NonLeaderLight();

       // getMessasge();
      }
      if (infected == 1)
      {
          //sentInfectedMessage();
          InfectionLight();
          delay(2000);
      }
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
    reading = digitalRead(button);
  if(reading == LOW){
    if(LEADER_ID==BEACON_ID){
      CLEARFLAG=1;
      sendTx();
    }
    else{
      INFECTED=1;
      InfectionLight();
      sendTx();
    }
  }
  xbee.readPacket(500);
  processResponse();

  delay(1000);
}
