"use strict"
const fs = require('fs');
const parse = require('csv-parse');
const util = require('./utils');

function _uploadProgramHeader(client) {

 	// send program tail command
	console.log("Starting program upload....");
	client.sendCmd([195, 0, 0, 0, 0, 0]);   
	return Promise.resolve();
}

function _uploadProgramTailAndSave(client) {

 	// send program tail command
	client.sendCmd([196,  0,  0,  0,  0,  0]);     
	console.log("Saving program....");
	client.sendCmd([197,  0,  0,  0, 0,  0]);     
	return Promise.resolve();
}

function _uploadProgramBody(client, prog) {
	for (let i = 0; i < prog.length; i++) {
 		client.sendCmd(prog[i]);         
 	}
	return Promise.resolve();
}

function _collectProgramBody(programFile, prog) {

  return new Promise((resolve, reject) => {

	if (!fs.existsSync(programFile)) {
		return reject(new Error(`Program file ${programFile} does not exist.`));
	  }
	console.log(`Sending program from ${programFile}....`);
		
  	fs.createReadStream(programFile)
    	.pipe(parse({delimiter: ',', comment: '#'}))
    	.on('data', function(csvrow) {
        	// convert into numbers
        	const cmd=csvrow.map(Number);
        	prog.push(cmd);
    	})
    	.on('end',function() {
			return resolve(prog);
		})
    	.on('error',function(err) {
			console.log(err);
			return reject(err);
    	});
  });
}

async function _uploadProgram2(client, programFile) {

	const prog = await _collectProgramBody(programFile, []);
	await _uploadProgramHeader(client);
	await _uploadProgramBody(client, prog);
	await _uploadProgramTailAndSave(client);
}

async function _uploadPrograms(client, programFiles) {
	let prog = [];
	if (Array.isArray(programFiles)) {
		for (let i = 0; i < programFiles.length; i++) {
			prog = await _collectProgramBody(programFiles[i], prog);
		}
	} else {
		prog = await _collectProgramBody(programFiles, []);
	}
	
	await _uploadProgramHeader(client);
	await _uploadProgramBody(client, prog);
	await _uploadProgramTailAndSave(client);

	return Promise.resolve();
}

function _uploadProgram(client, programFile) {

  return new Promise((resolve, reject) => {

	if (!fs.existsSync(programFile)) {
		return reject(new Error("File does not exist."));
	  }

 	 // send head program command
  	console.log("Starting program upload....");
  	client.sendCmd([195, 0, 0, 0, 0, 0]);   

  	fs.createReadStream(programFile)
    	.pipe(parse({delimiter: ',', comment: '#'}))
    	.on('data', function(csvrow) {
        	// convert into numbers
        	const cmd=csvrow.map(Number);
        	client.sendCmd(cmd);         
    	})
    	.on('end',function() {
			// send program tail and save
			client.sendCmd([196,  0,  0,  0,  0,  0]);     
			console.log("Saving program....");
			client.sendCmd([197,  0,  0,  0,  0,  0]);           
	
			console.log("Program sent");
			return resolve();
		})
    	.on('error',function(err) {
			console.log(err);
			return reject(err);
    	});
  });
}

function _startProgram(client) {
	client.sendCmd([206,  0,  0,  0,  0,  0]);
	console.log("Started.");
	return Promise.resolve();    
}

function _stopProgram(client) {
	client.sendCmd([207,  0,  0,  0,  0,  0]);    
}

function _pauseProgram(client) {
	client.sendCmd([208,  0,  0,  0,  0,  0]);    
}

function _syncProgram(client) {
	client.sendCmd([209,  0,  0,  0,  0,  0]);    
}

module.exports = {
	uploadProgram: _uploadProgram2,
	uploadPrograms: _uploadPrograms,
	startProgram: _startProgram,
	stopProgram: _stopProgram,
	pauseProgram: _pauseProgram,
	syncProgram: _syncProgram
}
	