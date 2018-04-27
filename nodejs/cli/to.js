"use strict"

function timeout (msec) {
    setTimeout(() => { 
        console.log("to");
        return Promise.resolve();
    }, msec);
}


async function test() {
    console.log("start");
    await timeout(3000)
    .then(() => {
        console.log("stop");
    });
};

test();

