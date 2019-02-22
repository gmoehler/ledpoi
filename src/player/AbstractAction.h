#ifndef ABSTRACT_ACTION_H
#define ABSTRACT_ACTION_H

#include "ledpoi.h"
#include "ledpoi_utils.h"
#include "PoiCommand.h"

struct ActionOptions {
	double dimFactor=1.0;
};

/**
 * Holds the information for an action
 **/

class AbstractAction
{
public:
  // returns a string identifier for messages
  virtual const char* getActionName() = 0;

  // initialized action with command
  virtual void init(PoiCommand cmd, PixelFrame* frame, ActionOptions options) = 0;

  // called for next step
  // may switch handler from active to inactive
  // if it is already inactive it will do nothing
  virtual void next() = 0;
  
  // check whether action is still active (runming)
  // or inactive (finished)
  virtual bool isActive() = 0;
  
  // general info about the action
  virtual void printInfo(const char* prefix = "") = 0;
  // info about current state of the action
  virtual void printState() = 0;
};
#endif
