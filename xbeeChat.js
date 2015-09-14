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
  var array = [0,0,0,0];
  
  var timeArray = [];
  //var a = 0;
  //var b = 0;
  //var c = 0;
  //var count = 0;
  sp.on('data', function(data) {
  var time = new Date();
  timeArray.push(time.getTime());
  //console.log(timeArray[0]);  
  console.log(data);
    //split the data and 
  message = data.split(',');
  //message.push(time.getTime());
   console.log(message);
  if(message[0]=='A'){
    array[0]= parseInt(message[1]);
    console.log(array[0]);
  }
  if(message[0]=='B'){
    array[1] = parseInt(message[1]);
  }
  if(message[0]=='C'){
    array[2] = parseInt(message[1]);
  }
  if(message[0]=='D'){
    array[3] = parseInt(message[1]);
  }
  console.log(time.getTime()-timeArray[0]);

  if((time.getTime()-timeArray[0]) >= 10000){
    var num = 0;
    var result = 0;
    for(var i = 0; i < array.length; i++){
      if(array[i]!=0){
        result += array[i];
        num++;
      }
    }
    var average = (result/num).toFixed(2);
    array=[0,0,0,0];
    timeArray = [];
    console.log("I got data from " + num + " sensors, and the average temperature is "+ average +"*C");
    var myString =  "I got data from " + num + " sensors, and the average temperature is "+ average +"*C"
    var timeString = "The time now is " + time.toLocaleTimeString()
    io.emit("chat message", myString );
    io.emit("chat message", timeString);
  }

/*  
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
*/    



   
  });

});

