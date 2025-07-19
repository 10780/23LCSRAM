#ifndef 23LCSRAM_h
#define 23LCSRAM_h

#include "Arduino.h"

class IC {
	public:
		chipSelect(int pin);
		void writeSRAM();
		
		void initialize();
		void terminate();
	private:
		int _pin;
};

#endif