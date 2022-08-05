#ifndef _SOFTSPI_H
#define _SOFTSPI_H

#define LEDMATRIX_D         GPIO_NUM_26	// Data pin
#define LEDMATRIX_SRCLK1    GPIO_NUM_25	// Clock pin
#define LEDMATRIX_SRCLK2    GPIO_NUM_33 // Clock pin
#define LEDMATRIX_RCLK		GPIO_NUM_0  // Latch pin
#define MUTEX_PIN           GPIO_NUM_36 // Remember float input

// typedef struct SpiInterface {
// 	uint8_t latchPin;
// 	uint8_t dataPin;
// 	uint8_t clockPin;
// } SpiInterface_TypeDef;

namespace softspi {

class SoftSpi {
	public:
	explicit SoftSpi(uint8_t latchPin, uint8_t dataPin, uint8_t clockPin): 
		mLatchPin(latchPin), 
		mDataPin(dataPin),
		mClockPin(clockPin),
		auto_write(true) 
	{};
	// int i;
	// SpiInterface_TypeDef spi;
	// va_list v;

	// mSpiInterfaces = new SpiInterface_TypeDef[spiInterface_num];
	// va_start(v, spiInterface_num);
	// for(i = 0; i < spiInterface_num; i++)
	// {
	// 	spi = va_arg(v, SpiInterface_TypeDef);
	// 	mSpiInterfaces[i] = spi;
	// }

	void show(void) const {
		int i, j;
		uint8_t _buf[2];

		for (j = 0; j < 8; j++) {
			_buf[0] = this->_buffer[j];
			_buf[1] = 0x01 << i;     // Choose the row
			this->send16(_buf[1], _buf[0]);
			delay(2);
		}
	}

	void fill(void) {
		uint8_t fill = 0xFF;
		int i, j;

		for (j = 0; j < 8; j++) {
			this->_buffer[j] = fill;
		}

		if (this->auto_write) {
			this->show();
		}
	}
	
	void _pixel(int x, int y, bool filled=true) {
		if (filled) {
			this->_buffer[x] |= 0x01 << y;
		}

		if (this->auto_write) {
			this->show();
		}
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
	uint8_t _buffer[8]; 
	const bool auto_write;

	private:
	uint8_t const mLatchPin;
	uint8_t const mDataPin;
	uint8_t const mClockPin;
};

}

//MultiSoftSpi multi_softspi(SimpleSpiInterface(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK1), SimpleSpiInterface(LEDMATRIX_RCLK, LEDMATRIX_D, LEDMATRIX_SRCLK2));

#endif