#include "PoiTimer.h"

PoiTimer::PoiTimer(LogLevel logLevel) : _logLevel (logLevel) {
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
	if (_logLevel != MUTE) {
		printf("Setting timer interval to %d ms\n", intervalMs);
	}
  // Alarm every intervalMs milli secs, auto-reload
  timerAlarmWrite(_timer, 1000 * intervalMs, true);
}

void PoiTimer::_enable(){
	if (_logLevel == CHATTY) printf("Enable timer.\n" );
  timerAlarmEnable(_timer);
}

void PoiTimer::disable(){
	if (_logLevel == CHATTY) printf("Disable timer.\n" );
  timerAlarmDisable(_timer);
}

PoiTimer::~PoiTimer(){
  //timerDetachInterrupt(_timer);
  timerEnd(_timer);
}
