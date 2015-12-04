#include <XBee.h>
#include <SoftwareSerial.h>

/*
This example is for Series 2 XBee
*/

// Should be a unique ID between 0 and 255.
uint8_t BEACON_ID = 1;


XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

uint8_t dbCommand[] = {'w', 's', 'a', 'd', 'z', 'c', 'x'};
AtCommandRequest atRequest = AtCommandRequest(dbCommand);

ZBTxStatusResponse txStatus = ZBTxStatusResponse();
AtCommandResponse atResponse = AtCommandResponse();

SoftwareSerial xbeeSerial(2, 3);

void setup() {
  // start serial
  Serial.begin(9600);
  xbeeSerial.begin(9600);
  xbee.setSerial(xbeeSerial);
  Serial.println("Initializing beacon...");
}

void sendTx(ZBTxRequest zbTx) {
  xbee.send(zbTx);
}

void processResponse() {
  if (xbee.getResponse().isAvailable()) {
    // got something

    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
      // got a zb rx packet
      int command;
      // now fill our zb rx class
      xbee.getResponse().getZBRxResponse(rx);
       command=rx.getData(0);

      if (command == 119) {//w
        Serial.print("forward\n");
      }
      // stop
      else if (command == 115) {//s
        Serial.print("Back\n");
      }
      // forward and left
      else if (command == 97) {//a
        Serial.print("Left\n");
      }
      //forward and right
      else if (command == 100) {//d
        Serial.print("Right\n");
      }
      //backward and left
      else if (command == 122) {//z 
        Serial.print("Drive mode\n");
      }
      //backward and right
      else if (command == 99) {//c
        Serial.print("Stop\n");
      }
      //backward
      else if (command == 120) {//x
        Serial.print("Auto\n");
      }else{
        Serial.print("error\n");
        }
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("error code:");
    Serial.println(xbee.getResponse().getErrorCode());
  }
}

// continuously reads packets, looking for ZB Receive or Modem Status
void loop() {
  xbee.readPacket();
  processResponse();
}
