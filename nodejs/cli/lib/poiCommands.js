"use strict"

const prog = [];

function _getProg() {
	return prog;
}

function _clearProg() {
	prog.length  = 0;
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
		prog.push([255, 199, u8c(id), 0, 0, 0, 0]);
}

global.showRgb = function(r, g, b, delay) {
	const del = splitUint16(delay);
	prog.push([255, 200, u8c(r), u8c(g), u8c(b), del.u, del.l]);
}

global.playFrames = function(from, to, nTimes, delay) {
	const del = splitUint16(delay);
	prog.push([255, 201, u8c(from), u8c(to), u8c(nTimes), del.u, del.l]);
}   

global.animateWorm = function(color, loops, pixels, delay) {
	const del = splitUint16(delay);
	prog.push([255, 202, u8c(color), u8c(loops), u8c(pixels), del.u, del.l]);
}

global.displayIP = function(ipIncr) {
	prog.push([255, 203, u8c(ipIncr), 0, 0, 0, 0]);
}

global.dim = function(factor) {
	const val = u8c(factor * 254);
	prog.push([255, 204, val, 0, 0, 0, 0]);
}

module.exports = {
	getProg: _getProg,
	clearProg: _clearProg
}
