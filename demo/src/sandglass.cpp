#include "../include/sandglass.h"

RTC_TimeTypeDef TimeStruct;

void Sandglass::init(void) {
	this->isActivated = false;
	this->first_interval = true;
	this->interval = 0;
	this->set_time.mins = 0;
	this->set_time.secs = 0;
	this->rest_time.mins = 0;
	this->rest_time.secs = 0;

	this->LedMatrix::init();
}

void Sandglass::start(Countdown_TypeDef* CountdownStruct) {
    this->isActivated = true;
	this->first_interval = true;
	this->Set_Countdown(CountdownStruct);
    
	// Start at 00:00:00
	// At some point, for examplt 00:00:30, it passed 30 seconds
    TimeStruct.Hours   = 0;
    TimeStruct.Minutes = 0;
    TimeStruct.Seconds = 0;
    M5.Rtc.SetTime(&TimeStruct);

	this->interval = 1.0 * (this->set_time.mins * 60 + this->set_time.secs) / 15;	// Get the interval of LED display refreshment
	this->_cur_drop = 1;

    // Then go to led matrix
	this->_start_tick = millis();
	this->LedMatrix::start();
}

// void Sandglass::pause(void) {
//     // Then go to led matrix
// }

// void Sandglass::resume(void) {
//     // Then go to led matrix
// }

void Sandglass::update(void) {
	unsigned long _now_tick;
	M5.Rtc.GetTime(&TimeStruct);

	if (TimeStruct.Seconds > this->set_time.secs) {
		this->rest_time.secs = this->set_time.secs + 60 - TimeStruct.Seconds;
		this->rest_time.mins = this->set_time.mins - TimeStruct.Minutes - 1;
	}
	else {
		this->rest_time.secs = this->set_time.secs - TimeStruct.Seconds;
		this->rest_time.mins = this->set_time.mins - TimeStruct.Minutes;
	}

	this->Show_RestTime();

	// Then go to led matrix
	// Omit the first interval. First refreshment at the second interval
	if (this->first_interval) {
		this->first_interval = false;
	}
	this->_drop_time = this->get_drop_times();

	M5.Lcd.setCursor(10, 90);
	M5.Lcd.printf("Drop time: %d\n", this->_drop_time);

	_now_tick = this->_start_tick;

	this->drop_interval = this->_drop_time > 1 ? (this->interval / (this->_drop_time+1)) : 1;

	_now_tick = millis();
	if (!this->first_interval and (_now_tick - this->_start_tick) >= (unsigned long)(this->drop_interval * 1000)) {
		this->LedMatrix::update(this->_cur_drop);
		this->_cur_drop++;
		this->_start_tick = _now_tick;
	}

	this->LedMatrix::refresh();

	if (this->rest_time.mins == 0 and this->rest_time.secs == 0) {
		M5.Lcd.setCursor(10, 50);
    	M5.Lcd.printf("Time up!\n");
		this->stop();
	}
}

void Sandglass::stop(void) {
    this->isActivated = false;
	this->interval = 0;
	this->set_time.mins = 0;
	this->set_time.secs = 0;
	this->rest_time.mins = 0;
	this->rest_time.secs = 0;

	this->Show_RestTime();

    // Then go to led matrix
	this->LedMatrix::stop();
}

void Sandglass::Set_Countdown(Countdown_TypeDef* CountdownStruct) {
    this->set_time.mins = CountdownStruct->mins;
    this->set_time.secs = CountdownStruct->secs;
}

void Sandglass::Show_RestTime(void) {
	M5.Lcd.setCursor(10, 30);
    M5.Lcd.printf("%02d mins, %02d secs left\n", this->rest_time.mins, this->rest_time.secs);
}

void Sandglass::show_settime(Countdown_TypeDef* CountdownStruct) {
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.printf("%02d mins, %02d secs set\n", CountdownStruct->mins, CountdownStruct->secs);
}

bool Sandglass::is_activated(void) {
    return this->isActivated;
}