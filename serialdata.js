var SerialPort = require("serialport").SerialPort;
var serialport = new SerialPort("/dev/cu.usbserial-DA01LOZ8");
serialport.on('open', function(){
  console.log('Serial Port Opend');
  serialport.on('data', function(data){
      console.log(data[0]);
      // if (data[0] === "one"){
      // 	console.log("we win");
      // }
   //    var b = data[1];
	  // var c = data[2];
	  // var d = data[4];
	  // var e = data[5];
	  // console.log("This is data" + a);
	  // console.log("This is data" + b);
	  // console.log("This is data" + c);
	  // console.log("This is data" + d);
	  // console.log("This is data" + e);
	  // var f = ((a + b + c + d + e)/5) ;
	  // console.log("The output is" + f);
  });
});

