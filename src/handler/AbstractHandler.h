#ifndef ABSTRACT_HANDLER_H
#define ABSTRACT_HANDLER_H

#include "ledpoi.h"

/**
 * Holds the information for an action
 **/

class AbstractHandler
{
public:
  // returns a string identifier for messages
  virtual const char* getActionName() = 0;

   // called for next step
   // may switch handler from active to inactive
   // if it is already inactive it will do nothing
  virtual void next() = 0;
  
  // check whether action is still active (runming)
  // or inactive (finished)
  virtual bool isActive() = 0;
  
  // delay configured beteeen 2 steps
  // delay is enforced outside of handler
  virtual uint16_t getDelayMs() = 0;
  
  // array of size N_PIXELS with the current colors
  virtual rgbVal* getDisplayFrame() = 0;

  // set action to inactive
  virtual void finish() = 0;

  // general info about the action
  virtual void printInfo() = 0;
  // info about current state of the action
  virtual void printState() = 0;
};
#endif
