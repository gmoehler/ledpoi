"use strict"

const prog = [];

// uint8 constrain
function u8c(val) {
	if (val > 254) {
		return 254;
	}
	if (val < 0) {
		return 0;
	}
	return Integer(val);
}

function u16c(val) {
	if (val > 256*256-1) {
		return 256*256-1;
	}
	if (val < 0) {
		return 0;
	}
	return Integer(val);
}

function splitUint16(val) {
	const u =u8c(Integer(u16c(val)/256));
	return {
		u: u,
		l: u8c(val - 256*u)
	};
}

function loadScene (id) {
		prog.push([255, 199, u8c(id), 0, 0, 0, 0]);
}

function showRgb   (r, g, b, delay) {
	const del = splitUint16(delay);
	prog.push([255, 200, u8c(r), u8c(g), u8c(b), del.u, del.l]);
}

function playFrames (from, to, nTimes, delay) {
	const del = splitUint16(delay);
	prog.push([255, 201, u8c(from), u8c(to), u8c(nTimes), del.u, del.l]);
}   

function animateWorm (color, loops, pixels, delay) {
	const del = splitUint16(delay);
	prog.push([255, 202, u8c(color), u8c(loops), u8c(pixels), del.u, del.l]);
}

function displayIP (ipIncr) {
	prog.push([255, 203, u8c(ipIncr), 0, 0, 0, 0]);
}

function dim (factor) {
	const val = u8c(factor * 254);
	prog.push([255, 204, val, 0, 0, 0, 0]);
}
