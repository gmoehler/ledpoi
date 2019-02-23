#include "playerTask.h"

xQueueHandle playerQueue = NULL;
bool skipActions = false;

NoAction noAction;
PlaySpiffsImageAction playSpiffsImageAction; 
ShowRgbAction showRgbAction;
DisplayIpAction displayIpAction;
AnimationAction animationAction;
ActionOptions options;

PixelFrame frame;

AbstractAction* getAction(PoiCommandType type){

  switch (type){

    case PLAY_SPIFFS_IMAGE:
    return dynamic_cast<AbstractAction*>(&playSpiffsImageAction);

    case SHOW_RGB:
    return dynamic_cast<AbstractAction*>(&showRgbAction);

    case ANIMATE:
    return dynamic_cast<AbstractAction*>(&animationAction);

    case DISPLAY_IP:
    return dynamic_cast<AbstractAction*>(&displayIpAction);

    default:
    LOGW(PLAY_T, "Action %d is not implemented.",  type);
    return dynamic_cast<AbstractAction*>(&noAction);
  }
}

void doControlCommand(PoiCommand cmd){
  if (cmd.getType() == DIM) {
      options.dimFactor = static_cast<double>(cmd.getField(1)) / 100;
  }

  else {
     LOGW(PLAY_T, "Error. Control command sent to player task is not implemented: %s", cmd.toString().c_str());
  }
}

void doAction(PoiCommand cmd) {
  AbstractAction *paction = getAction(cmd.getType());
  skipActions = false;

  paction->init(cmd, &frame, options);
  paction->printInfo("Starting action: ");

  while (!skipActions && paction->isActive()){
    paction->printState();
    sendFrameToDisplay(&frame, portMAX_DELAY);
    paction->next();
  }
  if (skipActions) {
    paction->printInfo("Stopped action: ");
  } else {
    paction->printInfo("Finished action: ");
  }
}

void playerTask(void* arg)
{
  RawPoiCommand rawCmd;

  for(;;) {
    // grab next command
    if(xQueueReceive(playerQueue, &( rawCmd ), portMAX_DELAY)) {
      PoiCommand cmd(rawCmd);
      if (skipActions) {
        LOGD(PLAY_T, "Skipping cmd: %s", cmd.toString().c_str());
      }
      else {
        LOGD(PLAY_T, "Receiving cmd: %s", cmd.toString().c_str());

        // play action or realize control command
        if (cmd.isActionCommand()) {
          // TODO: make this interruptable by control commands
          doAction(cmd);
        }
        else if (cmd.isPlayableCommand()) {
          doControlCommand(cmd);
        }
        else {
          LOGE(PLAY_T, "Error. Non-player command sent to player task: %s", cmd.toString().c_str());
        }
      }
    }
  }
}

void player_setup(uint8_t queueSize){
  playerQueue = xQueueCreate(queueSize, sizeof( RawPoiCommand ));
  playSpiffsImageAction.setup();
}

void player_start(uint8_t prio){ 
  skipActions = false;
  xTaskCreate(playerTask, "playerTask", 4096, NULL, prio, NULL);
}

void player_skipActions(){ 
  LOGI(PLAY_T, "Stopping player...");
  skipActions = true;
}

void player_resume(){ 
  LOGI(PLAY_T, "Resuming player...");
  skipActions = false;
}
