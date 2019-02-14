#ifndef POI_TIMER_H
#define POI_TIMER_H

/**
 * Simple timer abstraction for timers that fire interrupts
 **/

#include "ledpoi.h"

#define MINIMAL_INTERVAL 10

#define TIMER0 0
#define TIMER1 1
#define TIMER2 2
#define TIMER3 3

class PoiTimer
{
public:
  PoiTimer(uint8_t id, bool repeat);
  ~PoiTimer();

  void init(void (*timer_intr_func)());
  void setInterval(uint16_t intervalMs);
  void enable();
  void disable();
  void reset();

private:
  void _setInterval(uint16_t intervalMs);

  hw_timer_t *_timer;
  bool _repeat;
};

#endif
