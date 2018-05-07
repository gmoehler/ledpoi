// Serial port client that opens a connection with 115200 baud
"use strict";

const SerialPort = require ("serialport");
const Readline = SerialPort.parsers.Readline;
const utils = require('./utils');
 
module.exports = class SerialClient {
	constructor(pathOfPort) {
		if (this.connected) {
			this.disconnect();
		}
		this.port = new SerialPort(pathOfPort, {
			baudRate: 921600, 
			// baudRate: 115200, // default 
			autoOpen: false 
		});
		this.init();
	};
 
	init() {	 
		const parser = new Readline();
		this.port.pipe(parser);
		parser.on('data', console.log);

		this.port.on("open", () => {
			console.log("Open.");
			this.connected = true;
		});
		this.port.on("connect", () => {
			console.log("Connected.");
			this.connected = true;
		});
		this.port.on("error", (error) => {
			console.log("ERROR! " + error);
			client.port.close();
		});
		this.port.on("close", () => {
			console.log("Connection closed.");
			this.connected = false;
		});
	}
	
	type() {
		return "uart";
	}

	connect() {
		var that = this;
		return new Promise ((resolve, reject) => {
			that.port.open((err) => {
				if (err) {
					console.log("ERROR connecting:" + err);
					return reject(err);
				}
				console.log("Connected.")
				that.connected = true;
				return resolve();
			});
		});
	}

	isConnected() {
		return this.connected;
		/*
		if (this.connected) {
			if (this.port.isConnected){
				return true;
			}
			this.connected = false;
		}
		return false; */
	}

	disconnect() {
		var that = this;
		return new Promise ((resolve, reject) => {
			that.port.close((err) => {
				if (err) {
					console.log("ERROR closing:" + err);
					return reject(err);
				}
				console.log("Disconnected.")
				that.connected = false;
				return resolve();
			});
		});
	}

	write(data) {
		if (!this.connected) {
			console.log("Not connected. Cannot write data.");
			return;
		}
		this.port.write(data);
	}
	
	sendCmd(cmd, doLog){
		const doLogFlag = (doLog === undefined) || Boolean(doLog) === true; // default: true
		
		const verifiedCmd = [255]; // cmd separator
		// since 255 is cmd sep we do not allow it as value
		for (let i=0; i< 6; i++) {
			verifiedCmd.push(
				utils.constrain(cmd[i], 0, 254));
		}
		
		if (doLogFlag) {
			console.log("cmd: " + verifiedCmd);
		}
		const buf = new Buffer(verifiedCmd);
		this.write(buf);   
	}
};
