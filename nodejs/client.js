var net = require('net');
var fs = require('fs'),
readline = require('readline');

var rd = readline.createInterface({
    input: fs.createReadStream('./program.poi'),
//    output: process.stdout,
    console: false
});

var program = null;
var client = null;
var i = 0;

rd.on('line', function(line) {
    var cmdString = line;
    if (line.indexOf('#') !== -1){
      cmdString= line.substr(0, line.indexOf('#'));
    }
    var cmd = cmdString.split(" ").map(Number);
    console.log(cmd.length);
    cmd.unshift(255);
    console.log(cmd)
    program = cmd
/*    if (cmd.length>=6) {
      program.push(cmd);
    } */
});
rd.on('close', function(line) {
  console.log ("PRG:" + program);

  client = net.connect({ host: "192.168.1.127", port: 1110 }, connected);
});

function client_disconnect(){
  // send client disconnect command and close connection
  var buf = new Buffer([255, 254, 9, 0, 0, 0, 0]);
  client.write(buf);
  client.destroy();
}

function connected() {
  program=[255,253,6, 0, 0, 0, 0, 255, 253, 3, 0, 30, 0, 100, 255, 253, 0, 0, 0, 0, 0];
  var buf = new Buffer(program);
  console.log ("xPRG:" + program.toString());
  client.write(buf);
  client_disconnect();
}
