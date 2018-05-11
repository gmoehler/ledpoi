#ifndef POI_TIMER_H
#define POI_TIMER_H

/**
 * Simple timer abstraction for timers that fire interrupts
 **/

#include "ledpoi.h"

#define MINIMAL_INTERVAL 10

class PoiTimer
{
public:
  PoiTimer();
  ~PoiTimer();

  void init(void (*timer_intr_func)());
  void setInterval(uint16_t intervalMs);
  void enable();
  void disable();

private:
  void _setInterval(uint16_t intervalMs, bool repeat);

  hw_timer_t *_timer;
  bool _repeat;
};

#endif
