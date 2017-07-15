var net = require('net');
var fs = require('fs'),
readline = require('readline');

var program = null;
var client = null;
var i = 0;

jump2sync=[
  255,254,3,0,0,0,0,
  255,254,6,2,0,0,0,
  255,254,5,0,0,0,0];

client_run(jump2sync);

/*
async.retry(3, apiMethod, function(err, result) {
    // do something with the result
});*/

function client_run(){
  client = net.connect({ host: "192.168.1.127", port: 1110 }, connected);
}

function client_disconnect(){
  // send client disconnect command and close connection
  var buf = new Buffer([255, 254, 10, 0, 0, 0, 0]);
  client.write(buf);
  client.destroy();
}


function connected() {

  actions=jump2sync;
  console.log ("xPRG:" + actions.toString());
  
  var buf = new Buffer(actions);
  
  client.write(buf);
  //client_disconnect();
}
