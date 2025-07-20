/**
 * 23LCSRAM.cpp
 * Implementation of the 23LCSRAM class for interfacing with the 23LCXXX SRAM chips.
 * This file contains methods for chip selection, writing, testing, and initializing the IC.
 * 
 * This code can probably be probably be refactored to work with other ICs (like EEPROMs)
 * but for now it is designed to work with a single type of IC (23LC1024 SRAM specifically).
 * 
 * Public Domain, use at your own risk.
 * 
 * Opus inceptum: 02025-07
 **/

#include "Arduino.h"
#include "23LCSRAM.h"

/**** SRAM opcodes ****/
#define WRITE_OPCODE 0x02  // Write instruction opcode
#define READ_OPCODE  0x03  // Read instruction opcode

#define CLK 13 // Clock line
#define DATAIN 12 // Data in line, for reading
#define DATAOUT 11 // Data out line, for writing

IC::chipSelect(int pin) {
    /* Chip select
    * Chip select pin can be selected by user. This allows memory arrays using multiple ICs to be designed and implemented
    * The good thing about SPI is you can reuse the clock and data in/out pins for multiple ICs, you only need chip select to be unique
    */
    
    // Validate pin number (typical Arduino digital pins are 0-13, plus A0-A5 which are 14-19)
    if (pin < 0 || pin > 19) {
        Serial.print("Warning: Invalid chip select pin ");
        Serial.print(pin);
        Serial.println(". Valid range is 0-19.");
        _pin = -1;  // Mark as invalid
        _initialized = false;
        return;
    }
    
    // Check if pin conflicts with SPI pins
    if (pin == CLK || pin == DATAIN || pin == DATAOUT) {
        Serial.print("Error: Chip select pin ");
        Serial.print(pin);
        Serial.println(" conflicts with SPI pins. Choose a different pin.");
        _pin = -1;  // Mark as invalid
        _initialized = false;
        return;
    }
    
    _pin = pin;
    _initialized = false;  // Will be set to true when initialize() is called
    
    Serial.print("Chip select pin set to: ");
    Serial.println(_pin);
}

bool IC::isChipSelected() {
    return (_pin >= 0 && _initialized);
}

int IC::getChipSelectPin() {
    return _pin;
}

void IC::writeByte(uint32_t address, byte data) {
    // Check if chip is properly initialized
    if (!isChipSelected()) {
        Serial.println("Error: Cannot write - chip not properly initialized");
        return;
    }
    
    // Write a single byte to the specified address
    digitalWrite(_pin, LOW);  // Select the chip
    
    // Send write command
    shiftOut(DATAOUT, CLK, MSBFIRST, WRITE_OPCODE);
    
    // Send 24-bit address (23LC1024 uses 17-bit addressing, but we'll send 24 for compatibility)
    shiftOut(DATAOUT, CLK, MSBFIRST, (address >> 16) & 0xFF);  // High byte
    shiftOut(DATAOUT, CLK, MSBFIRST, (address >> 8) & 0xFF);   // Middle byte
    shiftOut(DATAOUT, CLK, MSBFIRST, address & 0xFF);          // Low byte
    
    // Send data byte
    shiftOut(DATAOUT, CLK, MSBFIRST, data);
    
    digitalWrite(_pin, HIGH); // Deselect the chip
}

void IC::writeIC(uint32_t address, byte* data, uint16_t length) {
    // Check if chip is properly initialized
    if (!isChipSelected()) {
        Serial.println("Error: Cannot write - chip not properly initialized");
        return;
    }
    
    // Write multiple bytes starting at the specified address
    digitalWrite(_pin, LOW);  // Select the chip
    
    // Send write command
    shiftOut(DATAOUT, CLK, MSBFIRST, WRITE_OPCODE);
    
    // Send 24-bit address
    shiftOut(DATAOUT, CLK, MSBFIRST, (address >> 16) & 0xFF);
    shiftOut(DATAOUT, CLK, MSBFIRST, (address >> 8) & 0xFF);
    shiftOut(DATAOUT, CLK, MSBFIRST, address & 0xFF);
    
    // Send data bytes
    for (uint16_t i = 0; i < length; i++) {
        shiftOut(DATAOUT, CLK, MSBFIRST, data[i]);
    }
    
    digitalWrite(_pin, HIGH); // Deselect the chip
}

byte IC::readByte(uint32_t address) {
    // Check if chip is properly initialized
    if (!isChipSelected()) {
        Serial.println("Error: Cannot read - chip not properly initialized");
        return 0xFF;  // Return error value
    }
    
    // Read a single byte from the specified address
    digitalWrite(_pin, LOW);  // Select the chip
    
    // Send read command
    shiftOut(DATAOUT, CLK, MSBFIRST, READ_OPCODE);
    
    // Send 24-bit address
    shiftOut(DATAOUT, CLK, MSBFIRST, (address >> 16) & 0xFF);
    shiftOut(DATAOUT, CLK, MSBFIRST, (address >> 8) & 0xFF);
    shiftOut(DATAOUT, CLK, MSBFIRST, address & 0xFF);
    
    // Read data byte
    byte data = shiftIn(DATAIN, CLK, MSBFIRST);
    
    digitalWrite(_pin, HIGH); // Deselect the chip
    return data;
}

void IC::readIC(uint32_t address, byte* buffer, uint16_t length) {
    // Check if chip is properly initialized
    if (!isChipSelected()) {
        Serial.println("Error: Cannot read - chip not properly initialized");
        return;
    }
    
    // Read multiple bytes starting at the specified address
    digitalWrite(_pin, LOW);  // Select the chip
    
    // Send read command
    shiftOut(DATAOUT, CLK, MSBFIRST, READ_OPCODE);
    
    // Send 24-bit address
    shiftOut(DATAOUT, CLK, MSBFIRST, (address >> 16) & 0xFF);
    shiftOut(DATAOUT, CLK, MSBFIRST, (address >> 8) & 0xFF);
    shiftOut(DATAOUT, CLK, MSBFIRST, address & 0xFF);
    
    // Read data bytes
    for (uint16_t i = 0; i < length; i++) {
        buffer[i] = shiftIn(DATAIN, CLK, MSBFIRST);
    }
    
    digitalWrite(_pin, HIGH); // Deselect the chip
}

bool IC::testIC() {
    byte testData = 0xAA;
    uint32_t testAddress = 0x000000; // Test at address 0
    
    Serial.print("Testing IC at pin ");
    Serial.println(_pin);
    
    // Write test data
    writeByte(testAddress, testData);
    delayMicroseconds(10);
    
    // Read back the data
    byte readData = readByte(testAddress);
    
    bool success = (readData == testData);
    Serial.print("Wrote: 0x");
    Serial.print(testData, HEX);
    Serial.print(", Read: 0x");
    Serial.print(readData, HEX);
    Serial.print(" - Test ");
    Serial.println(success ? "PASSED" : "FAILED");
    
    return success;
}

IC::initialize() {
    // Check if chip select pin is valid
    if (_pin < 0) {
        Serial.println("Error: Cannot initialize - invalid chip select pin");
        return;
    }
    
    // Set pin modes for the IC
    pinMode(CLK, OUTPUT);
    pinMode(DATAOUT, OUTPUT);
    pinMode(DATAIN, INPUT);
    pinMode(_pin, OUTPUT);
    
    digitalWrite(CLK, LOW);
    digitalWrite(DATAOUT, LOW);
    digitalWrite(_pin, HIGH); // Deselect chip initially
    
    _initialized = true;
    
    Serial.print("IC initialized with chip select pin ");
    Serial.println(_pin);
}

IC::terminate() {
    // Check if chip was initialized
    if (!_initialized) {
        Serial.println("Warning: Attempting to terminate uninitialized chip");
        return;
    }
    
    // Ensure chip is deselected
    if (_pin >= 0) {
        digitalWrite(_pin, HIGH);
    }
    
    // Set SPI pins to safe states
    digitalWrite(CLK, LOW);
    digitalWrite(DATAOUT, LOW);
    
    // Reset pin modes to INPUT to avoid conflicts with other devices
    pinMode(CLK, INPUT);
    pinMode(DATAOUT, INPUT);
    // Note: DATAIN is already INPUT, and chip select pin should remain OUTPUT
    // for other potential uses, but set it HIGH (deselected)
    
    // Mark as no longer initialized
    _initialized = false;
    
    Serial.print("IC on pin ");
    Serial.print(_pin);
    Serial.println(" terminated successfully");
}