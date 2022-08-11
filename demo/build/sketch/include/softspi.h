#line 1 "/home/kafcoppelia/WORK/BOARDS/M5StickC+/demo/include/softspi.h"
#ifndef _SOFTSPI_H
#define _SOFTSPI_H

#include <Arduino.h>

namespace softspi {

class SoftSpi {
	public:
	SoftSpi(uint8_t latchPin, uint8_t dataPin, uint8_t clockPin, bool auto_write=true): 
		mLatchPin(latchPin), 
		mDataPin(dataPin),
		mClockPin(clockPin),
		auto_write(auto_write)
	{
		this->fill(false);
	};

	void show(void) const {
		int i;
		uint8_t _buf[2];

		for (i = 0; i < 8; i++) {
			_buf[0] = this->_buffer[i];
			_buf[1] = 0x01 << i;     // Choose the row
			this->send16(_buf[1], _buf[0]);
			delay(2);
		}
	}

	void fill(bool color=true) {
		uint8_t fill = (color) ? 0xFF : 0x00;
		int i;

		for (i = 0; i < 8; i++) {
			this->_buffer[i] = fill;
		}

		if (this->auto_write) {
			this->show();
		}
	}

	void write(const uint8_t data[8]) {
		int i;

		for (i = 0; i < 8; i++) {
			this->_buffer[i] = data[i];
		}
	}
	
	void _pixel(int x, int y, bool filled) {
		if (filled) {
			this->_buffer[x] |= (0x01 << y);
		}
		else {
			this->_buffer[x] &= ~(0x01 << y);
		}

		if (this->auto_write) {
			this->show();
		}
	}

	bool _get_pixel(int x, int y) const {
		return (this->_buffer[x] & (0x01 << y));
	}

	/** Initialize the various pins. */
    void begin(void) const {
		pinMode(mLatchPin, OUTPUT);
		pinMode(mDataPin, OUTPUT);
		pinMode(mClockPin, OUTPUT);
    }

    /** Reset the various pins. */
    void end(void) const {
		pinMode(mLatchPin, INPUT);
		pinMode(mDataPin, INPUT);
		pinMode(mClockPin, INPUT);
    }

    /** Begin SPI transaction. Pull latch LOW. */
    void beginTransaction(void) const {
      	digitalWrite(mLatchPin, LOW);
    }

    /** End SPI transaction. Pull latch HIGH. */
    void endTransaction(void) const {
      	digitalWrite(mLatchPin, HIGH);
    }

    /** Transfer 8 bits. */
    void transfer(uint8_t value) const {
      	shiftOut(mDataPin, mClockPin, MSBFIRST, value);
    }

    /** Transfer 16 bits. */
    void transfer16(uint16_t value) const {
		uint8_t msb = (value & 0xff00) >> 8;
		uint8_t lsb = (value & 0xff);
		shiftOut(mDataPin, mClockPin, MSBFIRST, msb);
		shiftOut(mDataPin, mClockPin, MSBFIRST, lsb);
    }

    /** Convenience method to send 8 bits a single transaction. */
    void send8(uint8_t value) const {
		beginTransaction();
		transfer(value);
		endTransaction();
    }

    /** Convenience method to send 16 bits a single transaction. */
    void send16(uint16_t value) const {
		beginTransaction();
		transfer16(value);
		endTransaction();
    }

    /** Convenience method to send 16 bits a single transaction. */
    void send16(uint8_t msb, uint8_t lsb) const {
		beginTransaction();
		shiftOut(mDataPin, mClockPin, MSBFIRST, msb);
		shiftOut(mDataPin, mClockPin, MSBFIRST, lsb);
		endTransaction();
    }

	protected:
	const bool auto_write;
	uint8_t _buffer[8]; 

	private:
	uint8_t const mLatchPin;
	uint8_t const mDataPin;
	uint8_t const mClockPin;
};

}

#endif