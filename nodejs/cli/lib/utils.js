"use strict"

function _constrain(val, minval, maxval){
  const val1 = val>maxval ? maxval : val;
  return val1 < minval ? minval : val1;
}

function _isConnected(client) {
	return (client && client.isConnected());
}

function _checkConnected(client) {
	return _isConnected(client) ?
		Promise.resolve(client) :
		Promise.reject(new Error("Not connected."));
}

function _checkNotConnected(client) {
	return _isConnected(client) ?
		Promise.reject(new Error("Already connected.")) : 
		Promise.resolve(client);
}

function _delay(msecs) {
	setTimeout(() => { 
		return Promise.resolve();
	}, msecs);
}

module.exports = {
	delay: _delay,
	constrain: _constrain,
    checkConnected: _checkConnected,
    checkNotConnected: _checkNotConnected
}