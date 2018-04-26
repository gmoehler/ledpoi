"use strict"
const cmd = require('../poiCommands');

beforeEach(() => {
	cmd.init({loop: 0});
});

test('all user poi commands', () => {
	
	cmd.syncPoint(11);
	cmd.displayIP(5);
	
	loadScene(0);
	showRgb(1,2,3, 600);
	dim(0.5);
	loopStart(1, 10);
	animateWorm(3, 2, 15, 500);
	playFrames(10, 20, 3, 1000);
	loopEnd(1);

	const prog = cmd.getProg();
    expect(prog).toEqual([
    	[255, 215, 11, 0, 0, 0, 0], 
    	[255, 203, 5, 0, 0, 0, 0],
    
    	[255, 199, 0, 0, 0, 0, 0], 
		[255, 200, 1, 2, 3, 2, 88], 
		[255, 204, 127, 0, 0, 0, 0],
		[255, 214, 1, 0, 0, 0, 10], 
		[255, 202, 3, 2, 15, 1, 244], 
		[255, 201, 10, 20, 3, 3, 232], 
		[255, 216, 1, 0, 0, 0, 0]
	]);
});

test('delay edge cases', () => {
	showRgb(1,2,3, -100);
	showRgb(1,2,3, 70000);

	const prog = cmd.getProg();
    expect(prog).toEqual([
		[255, 200, 1, 2, 3, 0, 0],
		[255, 200, 1, 2, 3, 254, 254]
	]);
});

test('uint8 edge cases', () => {
	showRgb(255, 2, 3, 600);
	showRgb(256, 2, 3, 600);
	showRgb(-10, 2 ,3, 600);

	const prog = cmd.getProg();
    expect(prog).toEqual([
		[255, 200, 254, 2, 3, 2, 88],
		[255, 200, 254, 2, 3, 2, 88],
		[255, 200, 0, 2, 3, 2, 88]
	]);
});

test('loop count update', () => {
	const hi0 = cmd.getHiCounts();
	expect(hi0.loop).toEqual(0);
	
	// first sub program
	loopStart(1, 10);
	showRgb(1, 2, 3, 600);
	loopEnd(1);

	const prog1 = cmd.getProg();
    expect(prog1).toEqual([
		[255, 214, 1, 0, 0, 0, 10], 
		[255, 200, 1, 2, 3, 2, 88],
		[255, 216, 1, 0, 0, 0, 0]
	]);
	
	// re-init loop count
	const hi1 = cmd.getHiCounts();
	expect(hi1.loop).toEqual(1);
	cmd.init(hi1);
	
	// second sub program
	loopStart(1, 10);
	showRgb(1, 2, 3, 600);
	loopEnd(1);

	const prog2 = cmd.getProg();
    expect(prog2).toEqual([
		[255, 214, 2, 0, 0, 0, 10], 
		[255, 200, 1, 2, 3, 2, 88],
		[255, 216, 2, 0, 0, 0, 0]
	]);
	const hi2= cmd.getHiCounts();
	expect(hi2.loop).toEqual(2);
});


 