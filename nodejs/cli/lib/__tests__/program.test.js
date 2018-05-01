"use strict"

const prg = require("../program");

const client = {
	sendCmd: jest.fn()
};

beforeEach(() => {
	jest.resetModules();
});

const cpoi1 = "./lib/__tests__/testprog1.cpoi";
const cpoi1Lines = 3;
const jpoi2 = "./lib/__tests__/testprog2.jpoi";
const jpoi2Lines = 4;
const cpoi_invalid = "./lib/__tests__/testprog_invalid.cpoi";


test("start, stop, sync program", () => {
	prg.startProgram(client);
	prg.pauseProgram(client);
	prg.stopProgram(client);
	prg.syncProgram(client, 3);
	
	expect(client.sendCmd).toHaveBeenCalledTimes(4);
	expect(client.sendCmd).toHaveBeenCalledWith([206, 0, 0, 0, 0, 0]);
	expect(client.sendCmd).toHaveBeenCalledWith([207, 0, 0, 0, 0, 0]);
	expect(client.sendCmd).toHaveBeenCalledWith([208, 0, 0, 0, 0, 0]);
	expect(client.sendCmd).toHaveBeenCalledWith([209, 3, 0, 0, 0, 0]);
});

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
	const prog = await prg.doCollectProgram(cpoi1, 0);
	expect(prog.length).toEqual(1+cpoi1Lines); 
	expect(prog[0][0]).toEqual(215); // sync point
	expect(prog[0][1]).toEqual(0);
	expect(prg.getSyncMap()).toEqual({0: cpoi1});
});

test("uploadProgram with invalid commands", async () => {
	expect.assertions(1);
	try {
		await prg.doCollectProgram(cpoi_invalid, 0);
	} catch (e) {
    	expect(e.message).toMatch("Number of columns is inconsistent on line 2");
	}
});

test("uploadProgram with unknown file name", async () => {
	expect.assertions(1);
	try {
		await prg.doCollectProgram("unknownprog", 0);
	} catch (e) {
    	expect(e.message).toMatch(/Program file .* does not exist./);
	}
});

test("uploadProgram with javascript cmds", async () => {
	const prog = await prg.doCollectProgram(jpoi2, 0);

	expect(prog.length).toEqual(1+jpoi2Lines);
	expect(prog[0][0]).toEqual(215); // sync point
	expect(prog[0][1]).toEqual(0);
	expect(prg.getSyncMap()).toEqual({0: jpoi2});
});

test("uploadPrograms with mixed cmds", async () => {
	const prog = await prg.uploadPrograms(client, [cpoi1, jpoi2]);
	//console.log(prog);
	expect(prog.length).toEqual(2+cpoi1Lines+jpoi2Lines);
	expect(client.sendCmd).toHaveBeenCalledTimes(3+prog.length);
	expect(prog[0][0]).toEqual(215); // sync point 0
	expect(prog[0][1]).toEqual(0);
	expect(prog[1+cpoi1Lines][0]).toEqual(215); // sync point 1
	expect(prog[1+cpoi1Lines][1]).toEqual(1);
	expect(prg.getSyncMap()).toEqual({0: cpoi1, 1:jpoi2});
});

test("uploadProgram with 1 prog", async () => {
	const prog = await prg.uploadPrograms(client, jpoi2);
	//console.log(prog);
	expect(prog.length).toEqual(1+jpoi2Lines);
	expect(client.sendCmd).toHaveBeenCalledTimes(3+prog.length);
	expect(prog[0][0]).toEqual(215); // sync point 0
	expect(prog[0][1]).toEqual(0);
	expect(prg.getSyncMap()).toEqual({0:jpoi2});
});

test("uploadProgram with null", async () => {
	const prog = await prg.uploadPrograms(client, null);
	expect(prog.length).toEqual(0);
	expect(client.sendCmd).toHaveBeenCalledTimes(3);
	expect(prg.getSyncMap()).toEqual({});
});
