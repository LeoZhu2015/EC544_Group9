var SerialPort = require("serialport");
var app = require('express')();
var xbee_api = require('xbee-api');
var ml = require('ml-knn');
var fs = require('fs');  //Qingqing Add this in order to read file

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

var sp;var fs = require('fs');
sp = new SerialPort.SerialPort(portName, portConfig);

//My code of reading the file starts here
var obj;
fs.readFile('traindata1.json', function(err, f){
    var array = f.toString().split('\n');
    // use the array
    console.log(array);
  var jsonstring; 
  var tempArray = [];
  var result = [];
  var data = [];
  for(var i=0; i<array.length; i++){
    jsonstring = JSON.parse(array[i]);
    tempArray.push(jsonstring['sensor1']);
    tempArray.push(jsonstring['sensor2']);
    tempArray.push(jsonstring['sensor3']);
    tempArray.push(jsonstring['sensor4']);
    data.push(tempArray);
    tempArray=[];
    result.push(jsonstring['partition']);
  }
  console.log(result);
  console.log(data);   
});

//Code of reading file ends here.

// var data = [[55.70,58.18,50.72,51.70],
//             [65.30,60.70,45.20,43.50],
//             [57.82,54.45,46.54,48.40],
//             [53.41,63.18,50.09,53.92],
//            ];

// var result = [1,2,3,4];

var knn = new ml();
knn. train(data, result);

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
      var dataset = [[global.sensor_one,global.sensor_two,global.sensor_three,global.sensor_four]];
      var ans = knn.predict(dataset);
      console.log("Here I am: "+ans);

    //   var y = knn.predict({
    //   x : [sensor_one,sensor_two,sensor_three,sensor_four],
    //   k : 4,
    //   // weightf : {type : 'gaussian', sigma : 10.0},
    //   weightf : function(distance) {return 1./distance},
    //   distance : {type : 'euclidean'}
    //   });

    //   console.log(y);
    //   global.sensor_one = 0;
    //   global.sensor_two = 0;
    //   global.sensor_three = 0;
    //   global.sensor_four = 0;

    }
  }
});