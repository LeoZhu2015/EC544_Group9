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
app.use(express.static(__dirname + '/public'));

app.get('/rt', function(req, res){
  res.sendfile('realTime_v3.html');
});

app.get('/historical', function(req, res){
  res.sendfile('historical.html');
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
   var str=null;
   console.log(string)

   var findDocuments = function(db, callback) {
   message = string.split(",");
var date = new Date()
console.log(date.toLocaleString());

console.log(message[1]);

  var  starttime = message[1].split("/");
  var  endtime = message[2].split("/");
    console.log(starttime[0])
   //timestring = timearray[1] + ""starttime[2] + time 
var startstring = "20" + starttime[0] + "-" + starttime[1] + "-" + starttime[2] + " " + starttime[3] + ":00:00";
var endstring = "20" + endtime[0] + "-" + endtime[1] + "-" + endtime[2] + " " + endtime[3] + ":00:00";
var timestamp1 = Date.parse(new Date(startstring));
var timestamp2 = Date.parse(new Date(endstring));
console.log(startstring);
console.log(endstring);

var query1 = '{\"sensor_name\":message[0]}';
var query2 = '{\"sensor_name\":message[0],\"time\":{$gt:timestamp1,$lt:timestamp2}}'
 if(message[1]!=""&& message[2]!=""){
  var cursor = db.collection('sensor').find({"sensor_name":message[0],"time":{$gt:timestamp1,$lt:timestamp2}});
 }
 else{
  var cursor =db.collection('sensor').find({"sensor_name":message[0]});
 }
   
   //console.log("temperature'"+doc.temperature);
   cursor.each(function(err, doc) {
      assert.equal(err, null);
      if (doc != null) {
      	 //console.log(doc.temperature);
      	 str=str+doc.time+ "," + doc.temperature+"\n";
      	 // console.log(doc.time);
         // io.emit("A1",doc.time);

      } else {
      	 //console.log(str);
      	 socket.emit('A1',str);
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



