#ifndef SHOW_RGB_ACTION_H
#define SHOW_RGB_ACTION_H

#include "ledpoi.h"
#include "AbstractAction.h"

/**
 * action showing a steady rgb value
 **/

class ShowRgbAction : public AbstractAction
{
public:
  ShowRgbAction();
  const char* getActionName();
  
  void init(PoiCommand cmd, PixelFrame* frame, ActionOptions options);

  void next();
  bool isActive();
  
  void printInfo(const char* prefix);
  void printState();
  
private:  
  PixelFrame* _pframe;
  bool _active;
  rgbVal _rgb;
};
#endif
