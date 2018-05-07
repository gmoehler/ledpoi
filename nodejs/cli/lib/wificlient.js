// Serial port client that opens a connection with 115200 baud
"use strict";

const net = require ("net");
const utils = require('./utils');
 
module.exports = class WifiClient {
	constructor(host, port) {
		this.connected = false;

		this.connectionParams = { 
			host: host, port: port 
		};

		this.client = new net.Socket();
		this.init();
	};
 
	init() {	 
		this.client.on('data', (data) => {
			console.log(data.toString());
		});

		this.client.on('connect', () => {
			console.log("Connected.");
			this.connected = true;
		});

		this.client.on('error', (error) => {
			console.log("ERROR! " + error);
			// close is called after this event
		});

		this.client.on('end', () => {
			console.log("Connection ended.");
			this.connected = false;
		});

		this.client.on('close', () => {
			console.log("Connection closed.");
			this.connected = false;
		});

		this.client.on('timeout', () => {
			console.log("Socket timeout.");
			this.client.close();
		});
	}
	
	type() {
		return "wifi";
	}
	
	connect() {
		const that = this;
		return new Promise ((resolve, reject) => {
			try{
				that.client.connect(that.connectionParams, () => {
					console.log("Connected.");
					//that.client.setKeepAlive(true);
					that.connected = true;
					return resolve();
				});
			}
			catch (err) {
				console.log("ERROR connecting:" + err);
				return reject(err);
			}
		});
	}

	isConnected() {
		return this.connected;
	}

	disconnect() {
		var that = this;
		return new Promise ((resolve, reject) => {
			try{
				that.client.end();
				console.log("Disconnected.")
				that.connected = false;
				return resolve();
			}
			catch (err) {
				console.log("ERROR closing:" + err);
				return reject(err);
			}
		});
	}

	write(data) {
		if (!this.connected) {
			console.log("Not connected. Cannot write data.");
			return;
		}
		const allWritten = this.client.write(data, () => {
		// console.log("Data written.");
		});
		if (!allWritten) {
			console.log("Some data buffered...");
		}
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
