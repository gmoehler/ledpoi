#ifndef NO_ACTION_H
#define NO_ACTION_H

#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "AbstractAction.h"

/**
 * A action that does nothing
 **/


class NoAction : public AbstractAction
{
public:
  NoAction();
  const char* getActionName();
  
  void init(PoiCommand cmd, PixelFrame* frame, ActionOptions options);

  void next();
  bool isActive();
  
  void printInfo(const char* prefix);
  void printState();
  
private:  
  bool _active;
};
#endif
