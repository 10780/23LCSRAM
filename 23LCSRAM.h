#ifndef 23LCSRAM_h
#define 23LCSRAM_h

#include "Arduino.h"

class IC {
	public:
		chipSelect(int pin);
		bool isChipSelected();
		int getChipSelectPin();
		void writeIC(uint32_t address, byte data);
		void writeIC(uint32_t address, byte* data, uint16_t length);
		byte readByte(uint32_t address);
		void readIC(uint32_t address, byte* buffer, uint16_t length);
		bool testIC();
		
		void initialize();
		void terminate();
	private:
		int _pin;
		bool _initialized;
};

#endif