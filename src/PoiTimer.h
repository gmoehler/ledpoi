#ifndef POI_TIMER_H
#define POI_TIMER_H

#include <Arduino.h>

class PoiTimer
{
public:
PoiTimer(void (*timer_intr_func)(int));
~PoiTimer();

void setInterval(uint32_t intervalMs);
void enable();
void disable();

private:
hw_timer_t *_timer;
}
#endif