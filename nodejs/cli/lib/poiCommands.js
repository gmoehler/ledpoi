"use strict"

const prog = [];
let _loopIncr =0;
let _loopHi = 0;

function _getProg() {
	return prog;
}

function _init(hi) {
	prog.length  = 0;
	_loopIncr = hi.loop;
	_loopHi = _loopIncr;
}

function _getHiCounts() {
	return {
		loop:_loopHi
	}
}

// uint8 constrain
function u8c(val) {
	if (val > 254) {
		return 254;
	}
	if (val < 0) {
		return 0;
	}
	return parseInt(val);
}

function u16c(val) {
	if (val > 256*256-1) {
		return 256*256-1;
	}
	if (val < 0) {
		return 0;
	}
	return parseInt(val);
}

function splitUint16(val) {
	const u =u8c(parseInt(u16c(val)/256));
	return {
		u: u,
		l: u8c(val - 256*u)
	};
}

global.loadScene = function(id) {
		prog.push([199, u8c(id), 0, 0, 0, 0]);
}

global.showRgb = function(r, g, b, delay) {
	const del = splitUint16(delay);
	prog.push([200, u8c(r), u8c(g), u8c(b), del.u, del.l]);
}

global.playFrames = function(from, to, nTimes, delay) {
	const del = splitUint16(delay);
	prog.push([201, u8c(from), u8c(to), u8c(nTimes), del.u, del.l]);
}   

global.animateWorm = function(color, loops, pixels, delay) {
	const del = splitUint16(delay);
	prog.push([202, u8c(color), u8c(loops), u8c(pixels), del.u, del.l]);
}

function _displayIP(ipIncr) {
	prog.push([203, u8c(ipIncr), 0, 0, 0, 0]);
}

global.dim = function(factor) {
	const val = u8c(factor * 254);
	prog.push([204, val, 0, 0, 0, 0]);
}

global.loopStart = function(loopId, delay) {
	const del = splitUint16(delay);
	_loopHi = loopId+_loopIncr;
	prog.push([214, u8c(_loopHi), 0, 0, del.u, del.l]);
}

function _syncPoint(syncId) {
	prog.push([215, u8c(syncId), 0, 0, 0, 0]);
}

global.loopEnd = function(loopId) {
	prog.push([216, u8c(loopId+_loopIncr), 0, 0, 0, 0]);
}

module.exports = {
	getProg: _getProg,
	init: _init,
	getHiCounts: _getHiCounts,
	syncPoint: _syncPoint,
	displayIP: _displayIP
}
