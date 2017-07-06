#ifndef STATIC_HANDLER_H
#define STATIC_HANDLER_H


/**
 * Holds the information for a static action
 **/

class StaticHandler : public AbstractHandler
{
public:
  virtual const char* getActionName() = 0;

  void next() {
    return;
  }

  bool isActive() {
    return false;
  }
  
  uint16_t getDelayMs() {
    return 0;
  }

  virtual rgbVal* getDisplayFrame() = 0;

  virtual void printInfo() = 0;
  
  void printState() {
    printInfo();
  }
};
#endif
