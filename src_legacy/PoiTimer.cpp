#include "PoiTimer.h"

PoiTimer::PoiTimer(LogLevel logLevel, bool repeat) :
	_logLevel (logLevel), _repeat(repeat) {
	 _timer = timerBegin(3, 80, true);  // divider 80 = 1MHz
}

void PoiTimer::init(void (*timer_intr_func)()){
  timerAttachInterrupt(_timer, timer_intr_func, true); // attach timer0_inter, edge type interrupt
}

void PoiTimer::setIntervalAndEnable(uint16_t intervalMs){
	if (intervalMs > 0) {
		_setInterval(intervalMs);
		_enable();
	}
}

void PoiTimer::_setInterval(uint16_t intervalMs){
	LOGD(TIMER, "Setting timer interval to %d ms", intervalMs);
  // Alarm every intervalMs milli secs, auto-reload
  timerAlarmWrite(_timer, 1000 * intervalMs, _repeat);
}

void PoiTimer::_enable(){
	LOGD(TIMER, "Enable timer.");
  timerAlarmEnable(_timer);
}

void PoiTimer::disable(){
	LOGD(TIMER, printf("Disable timer.");
  timerAlarmDisable(_timer);
}

PoiTimer::~PoiTimer(){
  //timerDetachInterrupt(_timer);
  timerEnd(_timer);
}
