const net = require('net');
const fs = require('fs');
const parse = require('csv-parse');


if (process.argv.length < 3){
  throw new Error("Usage: node program.js [start/stop/load] [program.poi]");
}

let action = process.argv[2]; // after "node" and "program.js"
let program = "";

if (action == "start"){
  console.log("starting program...");
} else if (action == "stop") {
  console.log("stopping program...");
} else if (action == "load" && process.argv.length >= 4) {
  program = process.argv[3];
  console.log(`loading program ${program}...`);
} else {
  throw new Error("Usage: node program.js [start/stop/load] [program.poi]");
}

// connect
let client = net.connect({ host: "192.168.1.127", port: 1110 }, onConnect);

function constrain(val, minval, maxval){
  const val1 = val>maxval ? maxval : val;
  return val1 < minval ? minval : val1;
}

function onConnect() {

  if (action =="start" || action == "stop"){
    startOrStop();
  }
  else if (action == "load") {
    loadProgramFromFileAndSend();
  }

}

function loadProgramFromFileAndSend() {

  // send head program command
  console.log("Starting program upload...");
  let cmd = [255, 195,  0,  0,  0,  0,   0];
  sendCmd(cmd);   

  fs.createReadStream(program)
    .pipe(parse({delimiter: ',', comment: '#'}))
    .on('data', function(csvrow) {
        // convert into numbers
        const cmd=csvrow.map(Number);
        sendCmd(cmd);         
    })
    .on('end',function() {
      // send program tail and save
      console.log("End of program upload.");
      cmd = [196,  0,  0,  0,  0,   0];
      sendCmd(cmd);     
      console.log("Saving program....");
      cmd = [197,  0,  0,  0,  0,   0];
      sendCmd(cmd);           
   
      console.log("Program sent");
      // disconnect
      client.destroy();
    })
    .on('error',function(err) {
      console.log(err);
      // disconnect
      client.destroy();
    });
};

function startOrStop() {

    const cmd = action == "start" ? 
      [255, 206, 0, 0, 0, 0, 0] :
      [255, 207, 0, 0, 0, 0, 0];

    sendCmd(cmd)

    // disconnect
    client.destroy();
}

function sendCmd(cmd){
  // add cmd separator
  cmd.unshift(255);
  console.log(cmd);
  buf = new Buffer(cmd);
  client.write(buf);   
}