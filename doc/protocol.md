# Protocol

Transmit word by word:		0xFF+Byte[0]..Byte[5]

Byte[n] may not contain any 0xFF!


|Function||Byte0|Byte1|Byte2|Byte3|Byte4|Byte5|Status|Description|
|---|---|---|---|---|---|---|---|---|---|
|**setPixel**||**0..200**|scene|frame|r|g|b|done|Adds a pixel to a frame in a scene|
|**setAction**||**254**|actionCMD|action[0]|action[1]|action[2]|action[3]|||
||showCurrent||0||||||Show last displayed frame|
||showStatic||1|scene|frame|timeOutMSB|timeOutLSB|done|timeOut=0: no time out|
||black||2|fade[ms]MSB|fade[ms]LSB|||(done)|fade=0: sofort aus|
||startProg||3|||||(done)||
||pauseProg||4|||||(done)||
||continueProg||5|frame||||(done)|frame does not make sense(?)|
||saveProg||6|||||?||
||savePix||7|scene|frame|||?||
||setIP||8|IP1|IP2|IP3|IP4||0.0.0.0: use DHCP|
||setGW||9|GW1|GW2|GW3|GW4|||
||clientDisconnect||10|||||done|ask poi (server) to disconnect from PC (client)|
|**setProg**||**253**|function|||||||
||ProgramEnd||0|||||(done)|End of program|
||setActiveScene||1|scene|frame (opt)|||(done)||
||play||2|frameStart|frameEnd|delayMSB|delay LSB|(done)|Play frames of active scene|
||gotoNoFade||3|scene|frame|delayMSB|delay LSB||goto next [scene/frame] - nofade|
||gotoFade||4|scene|frame|delayMSB|delay LSB||goto next [scene/frame] - fade|
||loop||5|loopCt MSB|loopCt LSB|loopStart labelCode||(done)|loop starting at label with labelCode - or prev command if labelCode=0|
||label||6|labelCode|syncId||||labelCode is *name* of label (labelCode > 0) - syncId for syncImpulse (syncId > 0), syncIds start with 1 |
||||*default*||||||do nothing|
|**playDirect**||**252**|scene|start|end|delay |loops|||
|**endOfMessage**||**251**|||||||end of message  - client can disconnect|
