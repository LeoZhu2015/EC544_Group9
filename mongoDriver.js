var MongoClient = require('mongodb').MongoClient;
var assert = require('assert');
var ObjectId = require('mongodb').ObjectID;
var url = 'mongodb://localhost:27017/test';

MongoClient.connect(url, function(err, db) {
  assert.equal(null, err);
  console.log("Connected correctly to server.");
  insertDocument(db, function() {
    db.close();
  });
});

var sensor_data = {
     "sensor_name" : 'A',
      "location":{
        "x":"1",
        "y":"3",
      },
      "temperature" : "23",
      "unit":"C"
   }

var insertDocument = function(db, callback) {
   db.collection('sensor').insertOne(sensor_data, function(err, result) {
    assert.equal(err, null);
    console.log("Inserted a document into the restaurants collection.");
    callback(result);
  });
};

