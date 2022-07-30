#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/src/sandglass.cpp"
#include "../include/sandglass.h"

RTC_TimeTypeDef TimeStruct;

Sandglass::Sandglass():isActivated(0) {

}

void Sandglass::start(Countdown_TypeDef* CountdownStruct) {
    isActivated = true;
	Sandglass::Set_Countdown(CountdownStruct);
    
	// Start at 00:00:00
	// At some point, for examplt 00:00:30, it passed 30 seconds
    TimeStruct.Hours   = 0;
    TimeStruct.Minutes = 0;
    TimeStruct.Seconds = 0;
    M5.Rtc.SetTime(&TimeStruct);

	M5.Lcd.setCursor(10, 30);
	M5.Lcd.printf("Start\n");
    // Then go to ledarray
}

// void Sandglass::pause(void) {
//     // Then go to ledarray
// }

// void Sandglass::resume(void) {
//     // Then go to ledarray
// }

void Sandglass::update(void) {
	M5.Rtc.GetTime(&TimeStruct);
	now_time.mins = TimeStruct.Minutes;
	now_time.secs = TimeStruct.Seconds;

	Sandglass::Show_Countdown(&now_time);
	if ((now_time.mins > set_time.mins) or ((now_time.mins == set_time.mins) and (now_time.secs >= set_time.secs))) {
		M5.Lcd.setCursor(10, 50);
    	M5.Lcd.printf("Time up!\n");
		Sandglass::stop();
	}
    // Then go to ledarray
}

void Sandglass::stop(void) {
    isActivated = false;
	set_time.mins = 0;
	set_time.secs = 0;
	now_time.mins = 0;
	now_time.secs = 0;
    // Then go to ledarray
}

bool Sandglass::is_Activated(void) {
    return isActivated;
}

void Sandglass::Set_Countdown(Countdown_TypeDef* CountdownStruct) {
    CountdownStruct->mins = set_time.mins;
    CountdownStruct->secs = set_time.secs;

	M5.Lcd.setCursor(10, 70);
	M5.Lcd.printf("Set %02d %02d!\n", set_time.mins, set_time.secs);
}

void Sandglass::Get_Countdown(Countdown_TypeDef* CountdownStruct) {
    now_time.mins = CountdownStruct->mins;
    now_time.secs = CountdownStruct->secs;
}

void Sandglass::Show_Countdown(Countdown_TypeDef* CountdownStruct) {
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.printf("%02d mins, %02d secs left\n", CountdownStruct->mins, CountdownStruct->secs);
}

uint16_t Sandglass::Get_TotalTime(void) {
    return (uint16_t)set_time.mins * 60 + set_time.secs;
}