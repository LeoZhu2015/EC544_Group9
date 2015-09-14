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
*/    



   
  });

});

