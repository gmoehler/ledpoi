"use strict"

const utils = require("../utils");

const connectedClient = {
	isConnected: jest.fn(() => true)
};
const disconnectedClient = {
	isConnected: jest.fn(() => false)
};

test("constrain", () => {
	expect (utils.constrain(10, 5, 20)).toEqual(10);
	expect (utils.constrain(1, 5, 20)).toEqual(5);
	expect (utils.constrain(30, 5, 20)).toEqual(20);
})

test('checkConnected', () => {	
	expect (utils.checkConnected(connectedClient))
		.resolves.toBe(connectedClient);
	expect (utils.checkConnected(disconnectedClient))
		.rejects.toThrow("Not connected.");
});

test('checkNotConnected', () => {	
	expect (utils.checkNotConnected(disconnectedClient))
		.resolves.toBe(disconnectedClient);
	expect (utils.checkNotConnected(connectedClient))
		.rejects.toThrow("Already connected.");
});

test('delay', async () => {	
	const startTime = new Date();
	await utils.delay(1000);
	const diff = new Date() - startTime;
	expect(Math.abs(diff-1000))
		.toBeLessThan(20);
});
