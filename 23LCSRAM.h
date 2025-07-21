#ifndef 23LCSRAM_h
#define 23LCSRAM_h

#include "Arduino.h"

/**** IC class definition ****/

class IC {
	public:
		chipSelect(int pin);
		bool isChipSelected();
		int getChipSelectPin();
		void writeByte(uint32_t address, byte data);
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

/**** ICArray class definition ****/

class ICArray {
	public:
		ICArray();
		~ICArray();
		bool initializeArray(int numChips, int* chipSelectPins);
		bool initializeArray(int numChips, int startPin);
		void terminateArray();
		bool testAllChips();
		
		// Single chip access by index
		void writeByte(int chipIndex, uint32_t address, byte data);
		byte readByte(int chipIndex, uint32_t address);
		void writeIC(int chipIndex, uint32_t address, byte* data, uint16_t length);
		void readIC(int chipIndex, uint32_t address, byte* buffer, uint16_t length);
		
		// Array-wide operations
		void writeByteToAll(uint32_t address, byte data);
		void writeICToAll(uint32_t address, byte* data, uint16_t length);
		
		// Utility functions
		int getChipCount();
		IC* getChip(int chipIndex);
		bool isArrayInitialized();
		
	private:
		IC* _chips;
		int _numChips;
		bool _arrayInitialized;
		void cleanupArray();
};

#endif