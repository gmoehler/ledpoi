void PoiTimer(){
	 _timer = timerBegin(3, 80, true);  // divider 80 = 1MHz
}

void PoiTimer::init(void (*timer_intr_func)(int)){
  timerAttachInterrupt(_timer, timer_intr_func, true); // attach timer0_inter, edge type interrupt
}
	
void PoiTimer::setInterval(uint32_t intervalMs){
  timerAlarmWrite(_timer, 1000 * intervalMs, true); // Alarm every intervalMs milli secs, auto-reload
}

void PoiTimer::enable(){
  timerAlarmEnable(_timer);
}

void PoiTimer::disable(){
  //timerDetachInterrupt(_timer);
  timerAlarmDisable(_timer);
}

void ~PoiTimer(){
  timerEnd(_timer);
}
