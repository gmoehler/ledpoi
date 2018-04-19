const prg = require('./lib/program');
const img = require('./lib/image');
const SerialClient = require("./lib/serialclient");

client = new SerialClient("COM4");
client.connect()
    .then(() => {
        console.log("connected");
        client.write("echo");
    })
    .then(() => {
        console.log("uploading...");
        // return img.uploadImage(client, "tmp.png");
        return prg.uploadProgram(client, "program1.poi")
    })
    .then(() => {
        console.log("disconnecting...");
        return client.disconnect()
    })
    .then(() => {
        console.log("Done.")
    })
    .catch((err) => {
        console.log("ERROR" + err);
    })


/*
prg.uploadProgram(client, "xnodejs/poi/program1.poi")
    .then(() => {
        console.log("Done.")
    })


img.uploadImage(client, "nodejs/poi/tmp2.png")
.then(() => {
    console.log("Done.")
})
*/

/*
const fs = require('fs');
const png = require('pngjs').PNG;
const utils = require('./lib/utils');

const client = {
    sendCmd: function(cmd) {
        console.log(">" + cmd);
    }
}

function getPict(imageFile) {
    return new Promise((resolve, reject) => {
        if (!fs.existsSync(imageFile)) {
            return reject(new Error("File does not exist."));
          }      
        
        fs.createReadStream(imageFile)
        .pipe(new png({ filterType: 4 }))
        .on('parsed', function() {
            console.log(`Read image with ${this.width} frames and ${this.height} px` );

            // start transmission scene 0
            client.sendCmd([255, 192, 0, 0, 0, 0, 0]);
      
            for (let w = 0; w < this.width; w++) {
                console.log(`frame: ${w}`);
        
                for (let h = 0; h < this.height; h++) {
                  let idx = (this.width * h + w) << 2;
        
                  console.log(`${h}: ${this.data[idx]} ${this.data[idx+1]} ${this.data[idx+2]}`);
                  client.sendCmd(
                    [255, h, w, 0, utils.constrain(this.data[idx],0,254), 
                      utils.constrain(this.data[idx+1],0,254), utils.constrain(this.data[idx+2],0,254)]);
                  
                  // mirror values on the back
                  console.log(`${2*this.height-h-1}: ${this.data[idx]} ${this.data[idx+1]} ${this.data[idx+2]}`);
                  client.sendCmd(
                    [255, 2*this.height-h-1, w, 0, utils.constrain(this.data[idx],0,254), 
                      utils.constrain(this.data[idx+1],0,254), utils.constrain(this.data[idx+2],0,254)]);
                }
              }
            // end transmission scene 0
            client.sendCmd([255, 193, 0, 0, 0, 0, 0]);
 
            return resolve();
         });
    });
}

getPict("nodejs/poi/tmp.png")
.then(() => {
    console.log("Done.");
})
*/

