'use strict';
const inquirer = require('inquirer');
const prg = require('./lib/program');
const img = require('./lib/image');
const ctrl = require('./lib/control');
const utils = require('./lib/utils');

let client = null;

var mainMenu = [
  {
    type: 'rawlist',
    name: 'selection',
		message: 'Poi Commander',
		pageSize: 12,
    	choices: [
		 	{ 
	      		name: 'Connect',
				value: 'connect'
			},
			{ 
	      		name: 'Disconnect',
				value: 'disconnect'
			},
			{
	   			name: 'Upload image', 
		    	value:'up_image'
			} ,
			{
	   			name: 'Upload program', 
		    	value:'up_prog'
			} ,
			{
				 name: 'Connect poi to wifi', 
				 value:'wifi_connect'
			},
			{
				name: 'Disconnect poi from wifi', 
				value:'wifi_disconnect'
		    } ,
			{
				 name: 'Start program', 
				 value:'start_prog'
			} ,
			{
	   			name: 'Stop program', 
				value:'stop_prog'
			} ,
			{
	   			name: 'Sync', 
		    	value:'sync'
			},
			{
				name: 'Exit', 
			 	value:'exit'
		 	} 
		]
  },
  {
    type: 'input',
    name: 'filename',
    message: 'Filename:',
    when: function(answers) {
      return (["up_image", "up_prog"].includes(answers.selection));
    }
  },
  {
    type: 'input',
    name: 'ip_incr',
    message: 'IP increment:',
    default: '0',
    when: function(answers) {
      return (answers.selection == "wifi_connect");
	}
  }
]

function handleError(err) {
	console.log("Error: " + err.message);
	main();
}

function main(){
	inquirer.prompt(mainMenu).then(answer => {
		// console.log(answer);
		if (answer.selection === "connect") {
			utils.checkNotConnected(client) 
			.then(() => {
				console.log("Connecting...");
				const SerialClient = require("./lib/serialclient");
				client = new SerialClient("COM4");
				return client.connect();
			})
			.then(main)
			.catch(handleError);
		}

		else if (answer.selection === "disconnect") {
			utils.checkConnected(client) 
			.then(client.disconnect)
			.then(main)
			.catch(handleError);

		}
		else if (answer.selection === "up_prog") {
			utils.checkConnected(client) 
			.then(() => {
				return prg.uploadProgram(client, answer.filename);
			})
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "up_image") {
			utils.checkConnected(client) 
			.then(() => {
				return img.uploadImage(client, answer.filename);
			})
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "wifi_connect") {
			utils.checkConnected(client) 
			.then((client) => {
		  		return ctrl.connectWifi(client, parseInt(answer.ip_incr));
			})
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "wifi_disconnect") {
			utils.checkConnected(client) 
			.then(ctrl.disconnectWifi)
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "start_prog") {
			utils.checkConnected(client) 
			.then(prg.startProgram)
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "stop_prog") {
			utils.checkConnected(client) 
			.then(prg.stopProgram)
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "pause_prog") {
			utils.checkConnected(client) 
			.then(prg.pauseProgram)
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "sync") {
			utils.checkConnected(client) 
			.then(prg.syncProgram)
			.then(main)
			.catch(handleError);
		}
		else {
			client && client.isConnected() && client.disconnect();
			return;
		}

	});
}

main();
