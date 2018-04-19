
function _connectWifi(client, ipIncr) {
	client.sendCmd([210,  ipIncr,  0,  0,  0,  0]);
	return Promise.resolve();    
}

function _disconnectWifi(client) {
	client.sendCmd([211,  0,  0,  0,  0,  0]);
	return Promise.resolve();    
}

module.exports = {
	connectWifi: _connectWifi,
	disconnectWifi: _disconnectWifi
}