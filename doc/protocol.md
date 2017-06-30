# Protocol

Transmit word by word:		0xFF+Byte[0]..Byte[5]

Byte[n] may not contain any 0xFF!


|Function||Byte0|Byte1|Byte2|Byte3|Byte4|Byte5|Status|Description|
|---|---|---|---|---|---|---|---|---|---|
|**setPixel**||**0..200**|scene|frame|r|g|b|done|Adds a pixel to a frame in a scene|
|**setAction**||**254**|*actionCMD*|*action0*|*action0*|*action2*|*action3*|||
||save scene||0|scene|||||set scene id to scene currently in memory and save it to flash|
||showStatic||1|scene|frame|timeOutMSB|timeOutLSB|done|timeOut=0: no time out|
||black||2|fade[ms]MSB|fade[ms]LSB|||(done)|fade=0: sofort aus|
||startProg||3|||||done||
||pauseProg||4|||||done||
||continueProg||5|frame||||(done)|frame does not make sense(?)|
||jumpTo||6|syncId||||done|jump to label with syncId immediatelly - if syncId==0 jump to next syncId|
||setIP||7|IP1|IP2|IP3|IP4||0.0.0.0: use DHCP|
||setGW||8|GW1|GW2|GW3|GW4|||
||initializeFlash||9|||||done|initialize(clear) flash completely|
||clientDisconnect||10|||||done|tell poi (server) to disconnect from PC (client)|
|**setProg**||**253**|*progCMD*|*param0*|*param1*|*param2*|*param3*|||
||ProgramEnd||0|||||(done)|End of program|
||label||1|labelId|||||define label with id |
||syncPoint||2|sequenceId|||||define sync point for jumpTo action|
||play||3|frameStart|frameEnd|delayMSB|delay LSB|done|Play frames of active scene, or single frame with timeout|
||loop||4|loopCt MSB|loopCt LSB|loopStart labelCode||(done)|loop starting at label with labelCode - or prev command if labelCode=0 is not allowed|
||setActiveScene||5|scene||||(done)||
||presetRGB||6|r|g|b|||pre-set RGB color for following commands|
||FadetoRGB||7|scene|frame|delayMSB|delay LSB||goto next [scene/frame] - nofade|
||FadetoFrame||8|scene|frame|delayMSB|delay LSB||goto next [scene/frame] - nofade|
||spiralIn||9|delayMSB|delayLSB||||spiral turning in with delay spec before|
||spiralOut||10|delayMSB|delayMSB||||spiral turning out with delay spec before|
||allOneColor||11|timeOutMSB|timeOutMSB||||spiral turning out with delay spec before|
||||*default*||||||do nothing|
|**playDirect**||**252**|scene|start|end|delay |loops|||
|**endOfMessage**||**251**|||||||end of message  - client can disconnect|
