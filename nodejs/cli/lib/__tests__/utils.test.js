"use strict"

const utils = require("../utils");

const client = {
	isConnected: jest.fn()
		.mockImplementation(() => true)
};

test('checkConnected', () => {
	
	expect(utils.checkConnected(client))
		.resolves.toBeNull();
	
});