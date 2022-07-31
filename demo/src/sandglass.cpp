#include "../include/sandglass.h"

RTC_TimeTypeDef TimeStruct;

Sandglass::Sandglass():isActivated(0) {
	Sandglass::init();
}

void Sandglass::init(void) {
	set_time.mins = 0;
	set_time.secs = 0;
	rest_time.mins = 0;
	rest_time.secs = 0;
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

	if (TimeStruct.Seconds > set_time.secs) {
		rest_time.secs = set_time.secs + 60 - TimeStruct.Seconds;
		rest_time.mins = set_time.mins - TimeStruct.Minutes - 1;
	}
	else {
		rest_time.secs = set_time.secs - TimeStruct.Seconds;
		rest_time.mins = set_time.mins - TimeStruct.Minutes;
	}

	Sandglass::Show_RestTime();
	if (rest_time.mins == 0 and rest_time.secs == 0) {
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
	rest_time.mins = 0;
	rest_time.secs = 0;
    // Then go to ledarray
}

void Sandglass::Set_Countdown(Countdown_TypeDef* CountdownStruct) {
    set_time.mins = CountdownStruct->mins;
    set_time.secs = CountdownStruct->secs;
}

void Sandglass::Show_RestTime(void) {
	M5.Lcd.setCursor(10, 70);
    M5.Lcd.printf("%02d mins, %02d secs left\n", rest_time.mins, rest_time.secs);
}

void Sandglass::show_settime(Countdown_TypeDef* CountdownStruct) {
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.printf("%02d mins, %02d secs left\n", CountdownStruct->mins, CountdownStruct->secs);
}

bool Sandglass::is_Activated(void) {
    return isActivated;
}

uint16_t Sandglass::Get_TotalTime(void) {
    return (uint16_t)set_time.mins * 60 + set_time.secs;
}