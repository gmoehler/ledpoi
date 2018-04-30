"use strict"

const prg = require("../program");

const client = {
	sendCmd: jest.fn()
};

test("uploadProgramHeaderAndFooter", async () => {
	await prg.doUpload(client, null);
	expect(client.sendCmd).toHaveBeenCalledTimes(3);
	expect(client.sendCmd).toHaveBeenCalledWith([195, 0, 0, 0, 0, 0]);
	expect(client.sendCmd).toHaveBeenCalledWith([196, 0, 0, 0, 0, 0]);
	expect(client.sendCmd).toHaveBeenCalledWith([197, 0, 0, 0, 0, 0]);
});

test("doUploadProgram", async () => {
	const prog = [
		[215, 11, 0, 0, 0, 0], 
    	[203, 5, 0, 0, 0, 0]
    ]; 
	
	await prg.doUpload(client, prog);
	expect(client.sendCmd).toHaveBeenCalledTimes(3+prog.length);
	expect(client.sendCmd).toHaveBeenCalledWith([215, 11, 0, 0, 0, 0]);
	expect(client.sendCmd).toHaveBeenCalledWith([203, 5, 0, 0, 0, 0]);
});

test("uploadProgram with numeric cmds", async () => {
	const filename = "./lib/__tests__/testprog1.cpoi";
	const prog = await prg.doCollectProgram(filename, 0);
	expect(prog.length).toEqual(4); 
	expect(prg.getSyncMap()).toEqual({0: filename});
});

test("uploadProgram with javascript cmds", async () => {
	const filename = "./lib/__tests__/testprog2.jpoi";
	const prog = await prg.doCollectProgram(filename, 0);

	expect(prog.length).toEqual(5);
	expect(prg.getSyncMap()).toEqual({0: filename});
});

test("uploadPrograms with mixed cmds", async () => {
	const filename1 = "./lib/__tests__/testprog1.cpoi";
	const filename2 = "./lib/__tests__/testprog2.jpoi";
	const prog = await prg.uploadPrograms(client, [filename1, filename2]);
	console.log(prog);
	expect(prog.length).toEqual(9);
	expect(client.sendCmd).toHaveBeenCalledTimes(3+prog.length);
	expect(prg.getSyncMap()).toEqual({0: filename1, 1:filename2});
});
