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
//
var portName = process.argv[2];
var portNameNew = process.argv[3];

var sp;
sp = new SerialPort.SerialPort(portName, portConfig);

var spNew;
spNew = new SerialPort.SerialPort(portNameNew, portConfigNew);

var sampleDelay = 3000;

var portConfig = {
	baudRate: 9600,
  parser: XBeeAPI.rawParser()
};

var portConfigNew = {
	baudRate: 9600,
  parser: SerialPort.parsers.readline("\n")
};


//---------------------------->

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


var writeArduino = function(word){
  spNew.open(function(err) {
         console.log("Writing serial data: " + word);
         spNew.write(word, function(err, res) {
                 if (err) { console.log(err); }
                //  spNew.close();
         });
     });

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


//  requestRSSI('z');
  writeArduino('z');
});

socket.on('auto', function(auto){  //Auto Message
  console.log(auto);


//  requestRSSI('x');
  writeArduino('x');

});


socket.on('forward-right', function(forward_right){  //forward-right message
    console.log(forward_right);


  //  requestRSSI('e');
    writeArduino('e');


});

socket.on('forward-left', function(forward_left){  //forward-left Message
  console.log(forward_left);

//  requestRSSI('q');
  writeArduino('q');


});

socket.on('forward', function(forward){ // forward message
  console.log(forward);

//  requestRSSI('w');
  writeArduino('w');


});

socket.on('stop', function(stop){ // stop message
  console.log(stop);

//  requestRSSI('s');
  writeArduino('s');

});

socket.on('backward-left', function(backward_left){ // backward-left message
  console.log(backward_left);

//  requestRSSI('a');
  writeArduino('a');

});

socket.on('backward-right', function(backward_right){ // backward_right message
  console.log(backward_right);

//  requestRSSI('s');
  writeArduino('d');

});

});
