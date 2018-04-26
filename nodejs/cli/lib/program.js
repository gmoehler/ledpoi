"use strict"
const fs = require('fs');
const parse = require('csv-parse');
const util = require('./utils');
const path = require("path");
const cmd = require("./poiCommands");

const syncMap = {};

function _getSyncMap() {
	return syncMap;
}

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

function _collectProgramBody(progFileWithPath) {

  return new Promise((resolve, reject) => {
	const prog = [];
		
  	fs.createReadStream(progFileWithPath)
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

async function _collectProgramBodyJs(progFileWithPath) {
	
	// allow reading a file twice
	delete require.cache[progFileWithPath] ;
	const prog = require (progFileWithPath);
	return Promise.resolve(cmd.getProg());
}

async function _uploadPrograms(client, programFiles) {
	let prog = [];
	
	if (Array.isArray(programFiles)) {
		for (let i = 0; i < programFiles.length; i++) {
			syncMap[i] = programFiles[i];
			const progFileWithPath = path.join(process.cwd(), programFiles[i]);
			console.log(`Sending program from ${progFileWithPath}....`);
			
			if (!fs.existsSync(progFileWithPath)) {
				return Promise.reject(new Error(`Program file ${progFileWithPath} does not exist.`));
			}
			try {
				const hi = cmd.getHiCounts();
				cmd.init(hi);
				// add initial sync point
				cmd.syncPoint(i);
				const subprog = _isJpoiFile(progFileWithPath)
					? await _collectProgramBodyJs(progFileWithPath)
					: await _collectProgramBody(progFileWithPath);
				console.log(subprog);
				prog.push(...subprog);
			}
			catch(err) {
				return Promise.reject(err);
			}
		}
	} 
	else {
		prog = _isJpoiFile(programFiles)
				? await _collectProgramBodyJs(programFiles)
				: await _collectProgramBody(programFiles);
	}
	
	await _uploadProgramHeader(client);
	await _uploadProgramBody(client, prog);
	await _uploadProgramTailAndSave(client);

	return Promise.resolve(prog);
}


function _isJpoiFile(filename) {
	const ext = filename.split('.').pop();
	return ext === "jpoi";
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
	uploadPrograms: _uploadPrograms,
	startProgram: _startProgram,
	stopProgram: _stopProgram,
	pauseProgram: _pauseProgram,
	syncProgram: _syncProgram,
	getSyncMap: _getSyncMap
}
	