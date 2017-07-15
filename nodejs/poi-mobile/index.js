var net = require('net');
var fs = require('fs'),
readline = require('readline');
//const async = require ("async");

var program = null;
var client = null;
var i = 0;

jump2sync=[
  255,254,3,0,0,0,0,
  255,254,6,2,0,0,0,
  255,254,5,0,0,0,0];

//client_run(jump2sync);

// Pretend this is some complicated async factory
function ping1(id, callback) {
    console.log("ping")
    callback(null, {
        id: 'user' + id
    });
};

// generate 5 users
/*async.times(5, function(n, next) {
    ping(n, function(err, user) {
        next(err, user);
    });
}, function(err, users) {
    console.log (users);
});*/

repeat(5,ping);

function repeat(n, func){
  func(n, (n)=>{
    console.log(n);
    if (n>0){
      setTimeout(() => {
        repeat(n-1, func)},
        500);
    }
  })
}

function ping(n, callback){
  pingAction=[255];
  console.log ("ping \n");
 /* var buf = new Buffer(pingAction);
  client.write(buf);*/
  callback(n);
}

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
/*
  actions=jump2sync;
  console.log ("xPRG:" + actions.toString());
  
  var buf = new Buffer(actions);
  
  client.write(buf);
  //client_disconnect();*/
}
