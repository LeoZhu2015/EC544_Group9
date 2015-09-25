var SerialPort = require("serialport");
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};

var sp;

sp = new SerialPort.SerialPort(portName, portConfig);

app.get('/rt', function(req, res){
  res.sendfile('realTime.html');
});

app.get('/', function(req,res){
	res.sendfile('index.html');
});



http.listen(3000, function(){
  console.log('listening on *:3000');
});


	sp.on('data', function(data) {
		console.log(data);
			message = data.split(',');
			// console.log(message[0]);
			if (message[0] === 'A'){
				data = message[1];
				console.log(data);
				io.emit("A",data);
			}
			if (message[0] === 'B'){
				data = message[1];
				io.emit("B", data);
			}
			if (message[0] === 'C'){
				data = message[1];
				io.emit("C", data);
			}
			if (message[0] === 'D'){
				data = message[1];
				io.emit("D", data);
			}
});
