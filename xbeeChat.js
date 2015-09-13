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
  var array = [];
  var a = 0;
  var b = 0;
  var c = 0;
  var count = 0;
  sp.on('data', function(data) {
    
    console.log(data);


  
    if(/A/.test(data))
    {
        var reg_one = /[0-9]{2}/.exec(data);
        if(reg_one.constructor === Array);
        {
          console.log(reg_one[0]);
          console.log("One says", reg_one[0]);
          var a = parseInt(reg_one[0]) ;
          console.log(a);
          if(count === 0)
           {
           count += 1;
           array.push(a);
           }
         } 
    }   
    
    
      if(/B/.test(data))
    {
       var reg_two = /[0-9]{2}/.exec(data);
       if(reg_two.constructor === Array);
       {
           console.log(reg_two[0]);
           console.log("Two says", reg_two[0]);
           var b = parseInt(reg_two[0]);
           //array.push(b);
           if(count === 1)
           {
           count += 1;
           array.push(b);
           }
      }
    }
      

      if(/C/.test(data))
    {
       var reg_three = /[0-9]{2}/.exec(data);
       if(reg_three.constructor === Array);
       {

           console.log(reg_three[0]);
           console.log("Three says", reg_three[0]);
           var c = parseInt(reg_three[0]) ;
           //array.push(c);
           if(count === 2)
           {
           count += 1;
           array.push(c);
           }
           
       }
    }
    
    console.log("array :" , array);

 
    
    if (array.length === 3){
      var result =0 ;
      for (var i = 0; i< array.length; i++) {
           result += array[i];
           
           }
           var average = result/3 ; 
           count = 0 ;
          console.log("average is :" ,average);
          array = [];
    }
    



    //io.emit("chat message", "An XBee says: " + data);
  });
});

