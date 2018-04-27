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
	for (let i = 0; prog && i < prog.length; i++) {
		client.sendCmd(prog[i]);         
 	}
	return Promise.resolve();
}

function _collectProgramCmd(progFileWithPath, i) {

  return new Promise((resolve, reject) => {
	
	//initial sync point
	const prog = [[215, i, 0, 0, 0, 0]];
	
	// todo: do loop incr
		
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

async function _collectProgramJs(progFileWithPath, i) {
	
    // reset prog cache
	const hi = cmd.getHiCounts();
	cmd.init(hi);

    // add initial sync point
	cmd.syncPoint(i);
	
	// allow reading a file twice
	delete require.cache[progFileWithPath] ;
	const prog = require (progFileWithPath);
	return Promise.resolve(cmd.getProg());
}

async function _doCollectProgram(filename, i) {
	syncMap[i] = filename;
	
	const progFileWithPath = path.join(process.cwd(), filename);
	if (!fs.existsSync(progFileWithPath)) {
		return Promise.reject(new Error(`Program file ${progFileWithPath} does not exist.`));
	}
	console.log(`Sending program from ${progFileWithPath}....`);
	
	// collect cmds from program
	const prog = _isJpoiFile(progFileWithPath)
			? await _collectProgramJs(progFileWithPath, i)
			: await _collectProgramCmd(progFileWithPath, i);
			
	return Promise.resolve(prog);
}

async function _doUpload(client, prog) {
	await _uploadProgramHeader(client);
	await _uploadProgramBody(client, prog);
	await _uploadProgramTailAndSave(client);
}

function _isJpoiFile(filename) {
	const ext = filename.split('.').pop();
	return ext === "jpoi";
}

async function _uploadPrograms(client, programFiles) {
	let prog = [];
	
	if (Array.isArray(programFiles)) {
		for (let i = 0; i < programFiles.length; i++) {
			const subprog= await _doCollectProgram(programFiles[i], i);
			console.log(subprog);
			prog.push(...subprog);
		}
	} 
	else if (programFiles) {
		prog= await _doCollectProgram(programFiles, 0);
	}
	
	await _doUpload(client, prog);

	return Promise.resolve(prog);
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
	getSyncMap: _getSyncMap,
	// export for testing only
	doUpload: _doUpload,
	doCollectProgram: _doCollectProgram,
}
	