var SerialPort = require("serialport");
var express = require('express');
var app = express();
var http = require('http').Server(app);
var io = require('socket.io')(http);

var portName = process.argv[2],
portConfig = {
	baudRate: 9600,
	parser: SerialPort.parsers.readline("\n")
};
var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');
var ObjectId = require('mongodb').ObjectID;
var url = 'mongodb://localhost:27017/test';

var sp;

sp = new SerialPort.SerialPort(portName, portConfig);

app.use(express.static(__dirname + '/public/css'));

app.get('/rt', function(req, res){
  res.sendfile('realTime.html');
});

app.get('/index', function(req, res){
  res.sendfile('newindex.html');
});

app.get('/', function(req,res){
	res.sendfile('index.html');
});



http.listen(3000, function(){
  console.log('listening on *:3000');
});


sp.on('data', function(data) {
	//console.log("THia");
		message = data.split(',');
		// console.log(message[0]);
		if (message[0] === 'A'){
			data = message[1];
			//console.log(data);
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
var date = new Date();

var sensor_data = {
     "sensor_name" : message[0],
      "temperature" : message[1],
      "unit":"C",
      "time": date.getTime()
   }
 //console.log(sensor_data);

 
var insertDocument = function(db, callback) {
   db.collection('sensor').insertOne(sensor_data, function(err, result) {
    assert.equal(err, null);
    console.log("Inserted a document.");
    callback(result);
  });
};

MongoClient.connect(url, function(err, db) {
  assert.equal(null, err);
  console.log("Connected correctly to server.");
  insertDocument(db, function() {
    db.close();
  });
});

});


//On Button Press THis function happens


io.on("connection",function(socket){
			console.log("gfdkhfgakg");
socket.on("buttonPress", function(string){ 
   var array=[];
   var findDocuments = function(db, callback) {
   
   var cursor =db.collection('sensor').find({"time":{$gt:1443560762944, $lt:1443560774972}});
   //console.log("temperature'"+doc.temperature);
   cursor.each(function(err, doc) {
      assert.equal(err, null);
      if (doc != null) {
      	 //console.log(doc.temperature);
      	 array.push( doc.temperature+"\n");
      	 // console.log(doc.time);
         // io.emit("A1",doc.time);

      } else {
      	 console.log(array);
      	 socket.emit('A1',array);
         callback();
      }
   });
  // io.emit("A1",array);

};

MongoClient.connect(url, function(err, db) {
  assert.equal(null, err);
  findDocuments(db, function() {
      db.close();
  });
});


/*	MongoClient.connect(url, function(err, db) {
				console.log("reading from database");
                var date = new Date();
			    var t=date.getTime();
   
			   db.collection("sensor").find();
			    console.log("found");
			    db.close();


				//io.emit(A_history, data);



		});*/
	});
});



