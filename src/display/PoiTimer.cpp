#include "PoiTimer.h"

PoiTimer::PoiTimer(uint8_t id, bool repeat) : 
	_repeat(repeat) {
	_timer = timerBegin(id, 80, true);  // timer 3, divider 80 = 1MHz, count up
}

void PoiTimer::init(void (*timer_intr_func)()){
  timerAttachInterrupt(_timer, timer_intr_func, true); // attach timer0_inter, edge type interrupt
}

void PoiTimer::setInterval(uint16_t intervalMs){
  disable();
  // interpret always with minimal delay
  uint16_t interval = intervalMs < MINIMAL_INTERVAL ? MINIMAL_INTERVAL : intervalMs;
	_setInterval(interval);
	enable();
}

void PoiTimer::_setInterval(uint16_t intervalMs){
  LOGI(TIMER, "Setting timer interval to %d ms", intervalMs);
  // Alarm every intervalMs milli secs, auto-reload
  timerAlarmWrite(_timer, 1000 * intervalMs, _repeat);
}

void PoiTimer::enable(){
  LOGD(TIMER, "Enable timer." );
  timerAlarmEnable(_timer);
}

void PoiTimer::disable(){
  LOGD(TIMER, "Disable timer." );
  timerAlarmDisable(_timer);
}

void PoiTimer::reset() {
  LOGI(TIMER, "Timer reset." );
  timerRestart(_timer);
}

PoiTimer::~PoiTimer(){
  //timerDetachInterrupt(_timer);
  timerEnd(_timer);
}
