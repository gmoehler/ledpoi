#ifndef POI_TIMER_H
#define POI_TIMER_H

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
