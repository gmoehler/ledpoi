#ifndef ABSTRACT_HANDLER_H
#define ABSTRACT_HANDLER_H


/**
 * Holds the information for an action
 **/

class AbstractHandler
{
public:
  virtual void next() = 0;
  virtual bool isActive() = 0;
  
  virtual uint16_t getDelayMs() = 0;
  virtual rgbVal* getDisplayFrame() = 0;

  virtual void printInfo() = 0;
  virtual void printState() = 0;
};
#endif
