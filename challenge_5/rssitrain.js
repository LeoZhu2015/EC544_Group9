var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');
var ml = require('ml-knn');
var fs = require('fs');
// var train_data = require('jsonfile');
// var util = require('util');

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});


var portName = process.argv[2];

var sampleDelay = 3000;

portConfig = {
	baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);


//Create a packet to be sent to all other XBEE units on the PAN.
// The value of 'data' is meaningless, for now.
var RSSIRequestPacket = {
  type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
  destination64: "000000000000ffff",
  broadcastRadius: 0x01,
  options: 0x00,
  data: "test"
}

var requestRSSI = function(){
  sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

sp.on("open", function () {
  console.log('open');
  requestRSSI();
  setInterval(requestRSSI, sampleDelay);
});


global.sensor_one = 0;
global.sensor_two = 0;
global.sensor_three = 0;
global.sensor_four = 0;

XBeeAPI.on("frame_object", function(frame) {
  if (frame.type == 144){
    console.log("Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]));
    var string = "Beacon ID: " + frame.data[1] + ", RSSI: " + (frame.data[0]); 
    var sensordata = (frame.data[0]);
    console.log(sensordata);
   
    if (frame.data[1] == '1')
      global.sensor_one = sensordata;
    if (frame.data[1] == '2')
      global.sensor_two = sensordata;
    if (frame.data[1] == '3')
      global.sensor_three = sensordata;
    if (frame.data[1] == '4')
      global.sensor_four = sensordata;

    if((global.sensor_one != 0) && (global.sensor_two != 0) && (global.sensor_three != 0) && (global.sensor_four != 0))
    {
      // code fo perform machine learning
      console.log(global.sensor_one,global.sensor_two,global.sensor_three,global.sensor_four);
      var objnew = {sensor1: global.sensor_one, sensor2: global.sensor_two, sensor3: global.sensor_three, sensor4: global.sensor_four, partition: '11'};
      var jsonString =JSON.stringify(objnew)+'\n';
      // train_data.writeFile(file, objnew, function(err){
      //   console.error(err);
      // })

      fs.appendFile("traindata.json", jsonString, function(err) {
          if(err) {
              console.log(err);
          } else {
              console.log("The file was saved!");
          }

      }); 
      global.sensor_one = 0;
      global.sensor_two = 0;
      global.sensor_three = 0;
      global.sensor_four = 0;
    }
  }
});