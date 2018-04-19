# Overview over Commands:
(*) are optional commands

## Memory related commands (Entry of `Memory Task`)
- Start of active scene frames
- End of active scene frames
- Pixel of active scene frame (setPixel)
- Save active scene to flash
- Load active scene from flash
- Start of program statements
- End of program statements
- Program statement (play command or control statement)
- Save program to flash
- Initialize flash memory

## Play Commands (Entry to `Player Task`)
- Play Frames 
- Play Animation
- Show static RGB
- Fade to RGB (*)
- Fade to frame (*)
- Dim all upcoming frames by a factor

## Program Control (Entry of `Program Task`)
- Start program (from beginning)
- Pause program
- Continue program
- Jump to sync point

## Wifi Commands (Entry to `Wifi Task`)
- Wifi connect
- Wifi disconnect
- Set incremental IP (base IP + `id`)

## Program Control Statements (in addition to play commands)
- Label
- Loop
- Goto label
- Sync point

# Tasks

## Dispatch Task 
Takes commands from `Wifi Task` and `Button Task` and dispatches them to the suitable queue.

## Button Task
User interface task reading button events and sending out commands to the `Dispatch Task`

## Wifi Task
Gets commands from client and sends them on to `Dispatch Task`. Also deals with with Keep-Alive signal. 
Input is a queue that takes `Wifi Commands`.

## Program Task
Runs program and sends next command to `Player Task`. Program statements are either `Play Commands` for a direct play or `Program Control Statements`.
Input is a queue with `Start/Pause/Continue program` commands.

## Player Task
Steps thru frames, plays a static rgb or plays an animation. Sends frame to the input queue of the `Display Task`
Input is a queue with `Player Commands`. If flagged as _direct play_ then all running programs / plays are interrupted

## Display Task
Displays a frame on the leds. 
Input is a queue with frames to display.

# Actual commands

Transmit word by word:		0xFF+Byte0...Byte6. Therefore, **Byte1...Byte5 may not contain a 0xFF!**

Keep-alive signal is a single or double 0xFF.

Direct commands are carried out instantly cleaning the current program and play action. Program will not continue afterwards. 


|Impl|Function|Byte0|Byte1|Byte2|Byte3|Byte4|Byte5|Description|Prog / DirectPlay / Admin|Implemented|
|---|---|---|---|---|---|---|---|---|---|---|
|**Memory**|
|*|setPixel  |idx(0...127)|frame|(scene)|r|g|b|pixel of a frame in a scene -- scene is not supported for now|A|+|
| |          |_128...191_||||||_reserved address space_||+|
|*|headScene |192|scene     |||||first pixels of active scene `id` in next cmd (overwriting current active scene)|A|+|
|*|tailScene |193|          |||||last pixels of active scene `id` in prev cmd|A|+|
|*|saveScene |194|          |||||save active scene to flash |A|
|*|headProg  |195|          |||||first prog statement in next cmd (overwriting program)|A|
|*|tailProg  |196|          |||||prev. cmd was final statement of prog|A|
|*|saveProg  |197|          |||||save prog to flash |A|
|*|initFlash |198|          |||||init flash memory and image map in memory|A|
|**Play**  |
|*|loadScene |199|scene     |||||load scene `id` from flash to active scene|D/P|
|*|showRgb   |200|r         |g|b|timeout MSB|timeout LSB|show a static rgb with timeout|D/P|
|*|playFrames|201|frameStart|frameEnd|loops|delay/to MSB|delay/to LSB|Play frames of active scene with delay<br/> or single frame with timeout (when frameStart == frameEnd)|D/P|+|
|*|animate   |202|color id  |nLoops|nPixels|delay/to MSB|delay/to LSB|play worm animation in color|D/P|
|*|displayIP |203|ip        |with_static_bg|||| display ips from 0 to 10 - with static background if _with_static_bg_ is > 0 |D(/P)|+|
|*|dim       |204|dimFactor         |||||dim current image by a factor of `dimFactor/254`. Will be reset at the beginning of the program|D/P|
| |fade2Frame|205|(scene)   |frame||fadeTime MSB|fadeTime LSB|fade last Frame to a new frame -- scene is not supported for now|D/P|
|**Control**|
|*|startProg |206|          |||||start program immediately (interrupting all running programs/play actions)|A|
|*|stopProc  |207|          |||||stop processing immediately and clear queues|A|
| |pauseProc |208|          |||||pauses/resume processing immediately|A|
|*|jump2Sync |209|          |||||jump to snyc point immediately(interrupting all running programs/play actions)|A|
| |reset     |217|          |||||reset internal states (e.g. program_head, scene_head)|A|
|**Wifi**  |
|*|connect   |210|id        |||||connect to ip with `initial IP` + `id` (0...8)|A|+|
|*|disconnect|211|          |||||disconnect from wifi|A|+|
|*|clientDiscon|212|          |||||disconnect poi from client (PC)|A|
| |useSSID   |213|id        |||||connect to SSID with `id` as defined in the code|A|
|**Prog control statements**  |
|*|loopStart |214|loopId   |||loopCt MSB|loopCt LSB|define start of loop with id and number of loops |P|
|*|syncPoint |215|sequenceId|stopAtSync||||define sync point for jumpTo action, if stopAtSync is > 0 then the show stops at the sync point until sync is received|P|
|*|loopEnd   |216|loopId   |||||end of loop|P|
|**Other** |
| |          |_218...254_|   |||||_reserved address space_||
|*|          |255|          |||||keep-alive signal||

# Legacy commands

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
||jumpToSync||6|syncId||||done|jump to label with syncId immediatelly - if syncId==0 jump to next syncId|
||setIP||7|IP1|IP2|IP3|IP4||0.0.0.0: use DHCP|
||setGW||8|GW1|GW2|GW3|GW4|||
||initializeFlash||9|||||done|initialize(clear) flash completely|
||clientDisconnect||10|||||done|tell poi (server) to disconnect from PC (client)|
|**setProg**||**253**|*progCMD*|*param0*|*param1*|*param2*|*param3*|||
||ProgramEnd||0|||||(done)|End of program|
||label||1|labelId|||||define label with id |
||syncPoint||2|sequenceId|stopAtSync||||define sync point for jumpTo action, if stopAtSync is > 0 then the show stop at the sync point until sync is received|
||play||3|frameStart|frameEnd|delayMSB|delay LSB|done|Play frames of active scene, or single frame with timeout|
||loop||4|loopCt MSB|loopCt LSB|labelId||(done)|loop starting at label with labelId|
||setActiveScene||5|scene||||(done)||
||presetRGB||6|r|g|b|||pre-set RGB color for following command|
||FadetoRGB||7|scene|frame|delayMSB|delayLSB||fade from scene/frame (or current colors?) to RGB (as defined by presetRGB - or black if not defined)|
||FadetoFrame||8|scene|frame|delayMSB|delayLSB||fade from current color to scene/frame|
||jumpto||9|labelId|||||jump to label with labelId|
||dim||10|level||||done|dim output by factor level/254|
||||*default*||||||do nothing|
|**playDirect**||**252**|scene|start|end|delay |loops|||
|**endOfMessage**||**251**|||||||end of message  - client can disconnect|
