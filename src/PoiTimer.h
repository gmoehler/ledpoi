#ifndef POI_TIMER_H
#define POI_TIMER_H

/**
 * Simple timer abstraction for timers that fire interrupts
 **/

#include <Arduino.h>

class PoiTimer
{
public:
  PoiTimer();
  ~PoiTimer();

  void init(void (*timer_intr_func)());
  void setInterval(uint32_t intervalMs);
  void enable();
  void disable();

private:
  hw_timer_t *_timer;
};

#endif
