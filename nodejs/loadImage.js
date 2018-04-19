let net = require('net');
let fs = require('fs');
let png = require('pngjs').PNG;

if (process.argv.length < 3){
  throw new Error("Usage: node loadImage.js filename");
}
let imageFile = process.argv[2]; // after "node" and "loadImage.js"
console.log("imagefile:", imageFile);

// connect
let client = net.connect({ host: "192.168.1.127", port: 1110 }, readImageAndSend);

function constrain(val, minval, maxval){
  const val1 = val>maxval ? maxval : val;
  return val1 < minval ? minval : val1;
}

function readImageAndSend() {
  fs.createReadStream(imageFile)
  .pipe(new png({
      filterType: 4
  }))
  .on('parsed', function() {

    console.log(`Read image with ${this.width} frames and ${this.height} px` );

    // start transmission scene 0
    var buf = new Buffer([255, 192, 0, 0, 0, 0, 0]);
    client.write(buf);

    for (let w = 0; w < this.width; w++) {
      console.log(`frame: ${w}`);

      for (let h = 0; h < this.height; h++) {
        let idx = (this.width * h + w) << 2;

        console.log(`${h}: ${this.data[idx]} ${this.data[idx+1]} ${this.data[idx+2]}`);
        let buf = new Buffer(
          [255, h, w, 0, constrain(this.data[idx],0,254), 
            constrain(this.data[idx+1],0,254), constrain(this.data[idx+2],0,254)]);
        client.write(buf);
        
        // mirror values on the back
        console.log(`${2*this.height-h-1}: ${this.data[idx]} ${this.data[idx+1]} ${this.data[idx+2]}`);
        let buf2 = new Buffer(
          [255, 2*this.height-h-1, w, 0, constrain(this.data[idx],0,254), 
            constrain(this.data[idx+1],0,254), constrain(this.data[idx+2],0,254)]);
        client.write(buf2);
      }
    }
    // end transmission scene 0
    var buf = new Buffer([255, 193, 0, 0, 0, 0, 0]);
    client.write(buf);

    // disconnect
    client.destroy();
    });
}

