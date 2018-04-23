'use strict';

const fs = require('fs');
const path = require("path");
const prog = require("./program");
const img = require("./image");

async function _uploadShow(client, showFile) {
    const showFileWithPath = path.join(process.cwd(), showFile);
	if (!fs.existsSync(showFileWithPath)) {
		return Promise.reject(new Error(`Show file ${showFileWithPath} does not exist.`));
	}
	const show = require (showFileWithPath);

	const files = show && show.program && show.program.files;
	if (!files || !Array.isArray(files)) {
		return Promise.reject(new Error(`No program files in show file ${showFile}.`));
	}
	await prog.uploadPrograms(client, show.program.files);

    const images = show.images;
	if (images) {
        const ids = Object.keys(images);
		for (let i = 0; i < ids.length; i++) {
            const id = ids[i];
			await img.uploadImage(client, id, images[id]);
		}
	}
    
    return Promise.resolve();
}

module.exports = {
	uploadShow: _uploadShow
}
	