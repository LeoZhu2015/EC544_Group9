var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);
var SerialPort = require("serialport");

var portName = process.argv[2],
  portConfig = {
    baudRate: 9600,
    parser: SerialPort.parsers.readline("\n")
  };
// var sp;
// sp = new SerialPort.SerialPort(portName, portConfig);

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

socket.on('drive', function(drive){
  console.log(drive);
  // sp.write('drive');
});

socket.on('right', function(right){
    console.log(right);
    // sp.write('d');
});

socket.on('left', function(left){
  console.log(left);
  // sp.write('a');
});

socket.on('up', function(up){
  console.log(up);
  // sp.write('w');
});

socket.on('down', function(down){
  console.log(down);
  // sp.write('s');
})




});
