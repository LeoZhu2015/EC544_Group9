var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var SerialPort = require("serialport");
var xbee_api = require('xbee-api');

var C = xbee_api.constants;
var XBeeAPI = new xbee_api.XBeeAPI({
  api_mode: 2
});

var portName = process.argv[2];

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);

var sampleDelay = 3000;

var portConfig = {
	baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

//---------------------------->

//
// var RSSIRequestPacket = {
//   type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
//   destination64: "000000000000ffff",
//   broadcastRadius: 0x01,
//   options: 0x00,
//   data: "test"
// }

var requestRSSI = function(letter){

  var RSSIRequestPacket = {
    type: C.FRAME_TYPE.ZIGBEE_TRANSMIT_REQUEST,
    destination64: "000000000000ffff",
    broadcastRadius: 0x01,
    options: 0x00,
    data: "test"
  }
  RSSIRequestPacket.data = letter ;

  sp.write(XBeeAPI.buildFrame(RSSIRequestPacket));
}

//----------------------------->




http.listen(3000, function() {
  console.log('Listening on port 3000');
});



app.use('/', express.static(__dirname + '/Public'));
app.use('/',express.static(__dirname + '/Public/js'));
app.use('/',express.static(__dirname + '/Public/styles'));


app.get('/', function(req, res) {
  res.sendfile('index.html');
});


io.on('connection', function(socket) {


console.log('We are connected');

socket.on('message', function(message) {
  socket.broadcast.emit('message', message);
});

socket.on('drive', function(drive){   //Drive Message
  console.log(drive);


  requestRSSI('z');

});

socket.on('auto', function(auto){  //Auto Message
  console.log(auto);


  requestRSSI('x');

});


socket.on('right', function(right){  //Right message
    console.log(right);


    requestRSSI('d');


});

socket.on('left', function(left){  //Left Message
  console.log(left);

  requestRSSI('a');

});

socket.on('up', function(up){ // Up message
  console.log(up);

  requestRSSI('w');

});

socket.on('down', function(down){ // Down message
  console.log(down);

  requestRSSI('s');
})


});
