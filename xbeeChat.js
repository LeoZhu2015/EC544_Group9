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

app.get('/', function(req, res){
  res.sendfile('index.html');
});

io.on('connection', function(socket){
  console.log('a user connected');
  socket.on('disconnect', function(){
  });
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
    sp.write(msg + "\n");
  });
});

http.listen(3000, function(){
  console.log('listening on *:3000');
});

sp.on("open", function () {
  console.log('open');
  sp.on('data', function(data) {
    
    console.log(data);

    if(/A/.test(data))
    {
    var reg_one = /[0-9]{2}/.exec(data);
    console.log(reg_one[0]);
    console.log("One says", reg_one[0]);
    var a = parseInt(reg_one[0]) ;
    console.log(a);
    
    }
      if(/B/.test(data))
    {
    var reg_two = /[0-9]{2}/.exec(data);
    console.log(reg_two[0]);
    console.log("Two says", reg_two[0]);
    var b = parseInt(reg_two[0]);


    }
      if(/C/.test(data))
    {
    var reg_three = /[0-9]{2}/.exec(data);
    console.log(reg_three[0]);
    console.log("Three says", reg_three[0]);
    var c = parseInt(reg_three[0]) ;
    }
    
    var avg = a+2 ;
      console.log("The aVerage is = ", avg );



    //io.emit("chat message", "An XBee says: " + data);
  });
});

