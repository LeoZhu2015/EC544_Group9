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
  
  var count = 0; // Initialize count for order of acceptance
  sp.on('data', function(data) {
    
    console.log(data); //Raw Data


  
    if(/A/.test(data)) //If message includes A
    {
        var reg_one = /[0-9]{2}/.exec(data);// Extracting temperature from the massage
        if(reg_one.constructor === Array); // If massage is proper
        {
          console.log(reg_one[0]); // Display Number
          console.log("One says", reg_one[0]); //Print message on the console
          var a = parseInt(reg_one[0]) ; // Convert string to int
          console.log(a); // Print temperature in int
          if(count === 0) // If 1st message
           {
           count += 1; 
           array.push(a); // Append temperature to array
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
           if(count === 1) // If 2nd message 
           {
           count += 1; 
           array.push(b); // Append temperature to array
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
           if(count === 2) // If 3rd message
           {
           count += 1;
           array.push(c); // Append temperature to array
           }
           
       }
    }
    
    console.log("array :" , array); //Display array before averaging

 
    
    if (array.length === 3){ // If array has 3 values
      var result =0 ; //Initialize result
      for (var i = 0; i< array.length; i++) {
           result += array[i];
           
           }
           var average = result/3 ;  //Calculate average
           count = 0 ;
          console.log("average is :" ,average); //Displat Average
          array = []; //Initialize array back to empty
    }
    



    //io.emit("chat message", "An XBee says: " + data);
  });
});

