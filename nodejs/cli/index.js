'use strict';
const inquirer = require('inquirer');
const prg = require('./lib/program');
const img = require('./lib/image');
const show = require('./lib/show');
const ctrl = require('./lib/control');
const utils = require('./lib/utils');

let client = null;
const config = {};

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
				name: 'Upload show', 
			 	value:'up_show'
		 	} ,
			{
	   			name: 'Upload image with id 0', 
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
	   			name: 'Status', 
		    	value:'status'
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
    default: getDefaultFilename,
    when: function(answers) {
      return (["up_image", "up_prog", "up_show"].includes(answers.selection));
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
  },
  {
    type: 'input',
    name: 'sync_point',
    message: getSyncs,
    default: '0',
    when: function(answers) {
      return (answers.selection == "sync");
	}
  }
]

function getDefaultFilename(answers) {
	switch (answers.selection) {
		case "up_image":
			return config.image;
		case "up_prog":
			return config.prog;
 		case "up_show":
			return config.show;
		default:
			return "";
	}
}


function getSyncs(){
	var str = "Choose sync point: ";
	const syncs = prg.getSyncMap();
	for (let i in syncs) {
		str = str.concat(i + ":" + syncs[i] + " ");
	}
	
	return str;
}

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
			.then((client) => {
				return client.disconnect();
			})
			.then(main)
			.catch(handleError);

		}
		else if (answer.selection === "up_show") {
			utils.checkConnected(client) 
			.then(() => {
				return show.uploadShow(client, answer.filename);
			})
			.then(() => {
				config.show = answer.filename;
			})
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "up_prog") {
			utils.checkConnected(client) 
			.then(() => {
				return prg.uploadPrograms(client, [answer.filename]);
			})
			.then(() => {
				config.prog = answer.filename;
			})
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "up_image") {
			utils.checkConnected(client) 
			.then(() => {
				return img.uploadImage(client, 0, answer.filename);
			})
			.then(() => {
				config.image = answer.filename;
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
			.then((client) => prg.syncProgram(client, parseInt(answer.sync_point)))
			.then(main)
			.catch(handleError);
		}
		else if (answer.selection === "status") {
			utils.checkConnected(client) 
			.then((client) => ctrl.logStatus(client))
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
