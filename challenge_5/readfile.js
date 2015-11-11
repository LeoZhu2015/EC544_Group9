var fs = require('fs');
var obj;
fs.readFile('traindata1.json', function(err, f){
    var array = f.toString().split('\n');
    // use the array
    console.log(array);
	var jsonstring; 
	var tempArray = [];
	var tempResult = [];
	var data = [];
	for(var i=0; i<array.length; i++){
		jsonstring = JSON.parse(array[i]);
		tempArray.push(jsonstring['sensor1']);
		tempArray.push(jsonstring['sensor2']);
		tempArray.push(jsonstring['sensor3']);
		tempArray.push(jsonstring['sensor4']);
		data.push(tempArray);
		tempArray=[];
		tempResult.push(jsonstring['partition']);
	}
	console.log(tempResult);
	console.log(data);   
});
