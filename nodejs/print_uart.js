const SerialPort = require ("serialport");
const port = new SerialPort("COM4", {
    baudRate: 115200
});

const parser = new SerialPort.parsers.Readline({delimiter: '\n'})

port.on("error", (err) => {
    console.log("Error: ", err.message);
})

port.on("data", (buf) => console.log(buf.toString('utf8')));

port.write("A text\n", (err) => {
    if (err) {
        return console.log("error on write: " + err.message);
    }
    console.log("text written");
})


