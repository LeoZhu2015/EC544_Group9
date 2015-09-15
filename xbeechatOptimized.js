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
   var TempArray = [0,0,0,0];
   var NameArray = [];
   var CountArray = [0,0,0,0];
   var TimeArray = [];
   var PrintCount = 0;
   var Name
   //var a = 0;
   //var b = 0;
   //var c = 0;
   //var count = 0;
   sp.on('data', function(data) {

   var time = new Date();
   TimeArray.push(time.getTime());
   //console.log(timeArray[0]);  

   console.log(data);
     //split the data and 
   message = data.split(',');
   //message.push(time.getTime());
    console.log(message);
   if(message[0]=='A'){
      NameArray.push(message[0]);
      TempArray[0]= parseInt(message[1]);
      CountArray[0]++;
     //console.log(TempArray[0]);
   }
   if(message[0]=='B'){
      NameArray.push(message[0]);
      TempArray[1] = parseInt(message[1]);
      CountArray[1]++;
   }
   if(message[0]=='C'){
      NameArray.push(message[0]);
      TempArray[2] = parseInt(message[1]);
      CountArray[2]++;
   }
   if(message[0]=='D'){
      NameArray.push(message[0]);
      TempArray[3] = parseInt(message[1]);
      CountArray[3]++;

   }

  console.log(time.getTime()-TimeArray[0]);
   if((time.getTime()-TimeArray[0]) >= 9000){
     var num = 0;
     var result = 0;
     for(var i = 0; i < TempArray.length; i++){
       if(TempArray[i]!=0){
         result += TempArray[i];
         num++;
         
      }

    }
 
     var average = (result/num).toFixed(2);
     console.log("Data reveived from " + NameArray + ", and the average temperature is "+ average +"*C");
     var myString =  "Data reveived from " + NameArray + ", and the average temperature is "+ average +"*C";
     var timeString = "The time now is " + time.toLocaleTimeString();
     io.emit("chat message", myString );
     io.emit("chat message", timeString);
     TempArray=[0,0,0,0];
     TimeArray=[];
     NameArray=[];
     PrintCount++;

   }


   if (PrintCount==5){
    for (var i=0; i<CountArray.length; i++){
      if (CountArray[i]==0){
          console.log("The sensor "+ String.fromCharCode(65+i) +" has not sent data for 10 mins, please check!");
          var myString = "The sensor "+ String.fromCharCode(65+i) +" has not sent data for 10 mins, please check!";
          var timeString = "The time now is " + time.toLocaleTimeString();
          io.emit("chat message", myString );
          io.emit("chat message", timeString);
      }
    }
      PrintCount=0;
      CountArray=[0,0,0,0];
   }
  });

});

   