#ifndef POI_TIMER_H
#define POI_TIMER_H

/**
 * Simple timer abstraction for timers that fire interrupts
 **/

#include <Arduino.h>
#include "ledpoi.h"

class PoiTimer
{
public:
  PoiTimer(LogLevel logLevel, bool repeat);
  ~PoiTimer();

  void init(void (*timer_intr_func)());
  void disable();
  void setIntervalAndEnable(uint16_t intervalMs);

private:
  void _setInterval(uint16_t intervalMs);
  void _enable();

  hw_timer_t *_timer;
  bool _repeat;
  LogLevel _logLevel;
};

#endif
