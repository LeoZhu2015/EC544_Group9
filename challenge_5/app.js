var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var socket = io();
var SerialPort = require("serialport");

var portName = process.argv[2],
  portConfig = {
    baudRate: 9600,
    parser: SerialPort.parsers.readline("\n")
  };

var sp;

sp = new SerialPort.SerialPort(portName, portConfig);

var r1; //RSSI distance value of xbee 1
var r2; // RSSI distance value of xbee 2
var r3; //RSSI distance value of xbee 3


http.listen(3000, function() {
  console.log('Listening on port 3000');
});
// Look in the Public directory for all static files.
app.use('/', express.static(__dirname + '/Public'));
app.get('/', function(req, res) {
  res.sendfile('index.html');
});

sp.on('data', function(data) {
  console.log(data);
  message = data.split(',');
  // console.log(message[0]);
  if (message[0] === 'r1') {
    data = message[1];
    console.log(data);
    r1 = data;
    socket.emit("r1", data);
  }
  if (message[0] === 'r2') {
    data = message[1];
    r2 = data;
    socket.emit("r2", data);
  }
  if (message[0] === 'r3') {
    data = message[1];
    r3 = data;
    socket.emit("r3", data);
  }
  var locate = trilateration(r1, r2, r3);
  socket.emit('locate', locate);

});




socket.on('connection', function(socket){

  console.log("Client Connected");

});


function trilateration(r1, r2, r3)
{
var a = 0; //x coordinate of xbee 1
var b = 0; //y coordinate of xbee 1 at origin
var c = ; // x coordinate of xbee 2
var d = 0; //y coordinate of xbee 2
var e = sensor_three_x; // x coordinate of xbee 3
var f = sensor_three_y; // y coordinate of xbee 3

var x = ((r1*r1)-(r2*r2)+(c*c))/(2*c) ; // X coordinate of crawler

var y =  (((r1*r2)-(r2*r2)+(e*e)+(f*f))/(2*f))-((e/f)*x)

return {X:x,Y:y}
}
