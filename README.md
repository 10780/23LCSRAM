Has this ever happened to you?

You work on a physical prototyping project only to find out your microcontroller board doesn't have the resources (meaning memory) to actually make your code, and consequently your device, work as designed. 

:(

Luckily there is a solution...

Interfacing outboard memory with a microcontroller board is not an impossible task, it's actually rather straightforward. If you're using SPI ICs (like the ones this library was designed for), you just need to 
connect the lines for clock, data in, data out, and chip select. EZPZ.

HOWEVER, that doesn't mean you have a whole new piece of working memory for your device. Most of what actually ensures functionality for your new ICs is handled by software run on the microcontroller.

Which is where this comes into play...

## 23LCSRAM Arduino Library

This library provides a simple interface for controlling 23LCXXX series SRAM chips (specifically designed for the 23LC1024) via SPI communication. It handles all the low-level SPI protocol details, allowing you to easily read and write data to external SRAM memory.

### Features

- ✅ Single byte and multi-byte read/write operations
- ✅ Support for multiple SRAM chips in arrays
- ✅ Built-in validation and error checking
- ✅ Easy-to-use API with clear function names
- ✅ Hardware SPI communication
- ✅ Automatic chip selection management

### Hardware Requirements

- Arduino-compatible microcontroller
- 23LC1024 SRAM chip (or compatible 23LCXXX series)
- SPI connections:
  - **Pin 13** - Clock (CLK)
  - **Pin 12** - Data In (MISO)
  - **Pin 11** - Data Out (MOSI)
  - **Digital Pin** - Chip Select (user selectable)

### Installation

1. Download the library files (`23LCSRAM.h` and `23LCSRAM.cpp`)
2. Place them in your Arduino libraries folder
3. Include the library in your sketch: `#include "23LCSRAM.h"`

## Library Functions

### Initialization Functions

#### `chipSelect(int pin)`
Sets the chip select pin for the SRAM IC.
```cpp
IC sram;
sram.chipSelect(10);  // Use pin 10 as chip select
```
- **Parameters**: `pin` - Digital pin number (0-19)
- **Notes**: Validates pin conflicts with SPI pins and provides error messages

#### `initialize()`
Initializes the SRAM IC and sets up pin modes.
```cpp
sram.initialize();  // Must be called after chipSelect()
```
- **Function**: Sets up SPI pins and prepares the chip for communication
- **Required**: Must be called before any read/write operations

#### `terminate()`
Safely shuts down the SRAM IC and resets pin states.
```cpp
sram.terminate();  // Clean shutdown
```
- **Function**: Deselects chip, resets pins, and marks as uninitialized

### Write Functions

#### `writeByte(uint32_t address, byte data)`
Writes a single byte to the specified memory address.
```cpp
sram.writeByte(0x000100, 0x42);  // Write 0x42 to address 0x100
```
- **Parameters**: 
  - `address` - Memory address (0 to 0x1FFFF for 23LC1024)
  - `data` - Byte value to write

#### `writeIC(uint32_t address, byte* data, uint16_t length)`
Writes multiple bytes starting at the specified address.
```cpp
byte myData[] = {0x01, 0x02, 0x03, 0x04, 0x05};
sram.writeIC(0x000200, myData, 5);  // Write array starting at 0x200
```
- **Parameters**:
  - `address` - Starting memory address
  - `data` - Pointer to byte array
  - `length` - Number of bytes to write

### Read Functions

#### `readByte(uint32_t address)`
Reads a single byte from the specified memory address.
```cpp
byte value = sram.readByte(0x000100);  // Read from address 0x100
```
- **Parameters**: `address` - Memory address to read from
- **Returns**: Byte value at the specified address

#### `readIC(uint32_t address, byte* buffer, uint16_t length)`
Reads multiple bytes into a buffer starting at the specified address.
```cpp
byte buffer[5];
sram.readIC(0x000200, buffer, 5);  // Read 5 bytes starting at 0x200
```
- **Parameters**:
  - `address` - Starting memory address
  - `buffer` - Pointer to buffer array
  - `length` - Number of bytes to read

### Utility Functions

#### `testIC()`
Tests the SRAM IC by writing and reading back test data.
```cpp
if (sram.testIC()) {
    Serial.println("SRAM is working!");
} else {
    Serial.println("SRAM test failed!");
}
```
- **Returns**: `true` if test passes, `false` if it fails
- **Function**: Writes test pattern and verifies read-back

#### `isChipSelected()`
Checks if the chip is properly initialized and ready for use.
```cpp
if (sram.isChipSelected()) {
    // Safe to perform read/write operations
}
```
- **Returns**: `true` if chip is ready, `false` otherwise

#### `getChipSelectPin()`
Returns the current chip select pin number.
```cpp
int pin = sram.getChipSelectPin();
Serial.print("Using pin: ");
Serial.println(pin);
```
- **Returns**: Current chip select pin number

## ICArray Class (Multi-Chip Management)

The `ICArray` class provides automatic management of multiple SRAM chips, eliminating the need for manual setup of individual chips.

### Array Initialization Functions

#### `initializeArray(int numChips, int* chipSelectPins)`
Initializes an array of SRAM chips with specified chip select pins.
```cpp
ICArray sramArray;
int pins[] = {4, 5, 6, 7};
sramArray.initializeArray(4, pins);  // Initialize 4 chips on pins 4,5,6,7
```
- **Parameters**:
  - `numChips` - Number of chips (1-10)
  - `chipSelectPins` - Array of pin numbers for chip select
- **Returns**: `true` if successful, `false` if failed

#### `initializeArray(int numChips, int startPin)`
Initializes an array of SRAM chips with consecutive chip select pins.
```cpp
ICArray sramArray;
sramArray.initializeArray(4, 4);  // Initialize 4 chips on pins 4,5,6,7
```
- **Parameters**:
  - `numChips` - Number of chips (1-10)
  - `startPin` - Starting pin number (pins will be consecutive)
- **Returns**: `true` if successful, `false` if failed

#### `terminateArray()`
Safely terminates all chips in the array.
```cpp
sramArray.terminateArray();
```

### Array Access Functions

#### `writeByte(int chipIndex, uint32_t address, byte data)`
Writes a single byte to a specific chip in the array.
```cpp
sramArray.writeByte(0, 0x000100, 0x42);  // Write to chip 0
```

#### `readByte(int chipIndex, uint32_t address)`
Reads a single byte from a specific chip in the array.
```cpp
byte value = sramArray.readByte(2, 0x000100);  // Read from chip 2
```

#### `writeIC(int chipIndex, uint32_t address, byte* data, uint16_t length)`
Writes multiple bytes to a specific chip in the array.
```cpp
byte data[] = {0x01, 0x02, 0x03};
sramArray.writeIC(1, 0x000200, data, 3);  // Write to chip 1
```

#### `readIC(int chipIndex, uint32_t address, byte* buffer, uint16_t length)`
Reads multiple bytes from a specific chip in the array.
```cpp
byte buffer[3];
sramArray.readIC(3, 0x000200, buffer, 3);  // Read from chip 3
```

### Array-Wide Operations

#### `writeByteToAll(uint32_t address, byte data)`
Writes the same byte to all chips in the array at the specified address.
```cpp
sramArray.writeByteToAll(0x000000, 0xFF);  // Write 0xFF to address 0 on all chips
```

#### `writeICToAll(uint32_t address, byte* data, uint16_t length)`
Writes the same data array to all chips in the array.
```cpp
byte data[] = {0x01, 0x02, 0x03};
sramArray.writeICToAll(0x000100, data, 3);  // Write same data to all chips
```

### Array Utility Functions

#### `testAllChips()`
Tests all chips in the array and reports results.
```cpp
if (sramArray.testAllChips()) {
    Serial.println("All chips passed!");
} else {
    Serial.println("Some chips failed!");
}
```
- **Returns**: `true` if all chips pass, `false` if any fail

#### `getChipCount()`
Returns the number of chips in the array.
```cpp
int count = sramArray.getChipCount();
```

#### `getChip(int chipIndex)`
Returns a pointer to a specific chip for direct access.
```cpp
IC* chip2 = sramArray.getChip(2);  // Get direct access to chip 2
if (chip2 != nullptr) {
    chip2->writeByte(0x000000, 0xAA);
}
```

#### `isArrayInitialized()`
Checks if the array is properly initialized.
```cpp
if (sramArray.isArrayInitialized()) {
    // Safe to use array functions
}
```

## Usage Examples

### Basic Single Chip Usage
```cpp
#include "23LCSRAM.h"

IC sram;

void setup() {
    Serial.begin(9600);
    
    // Initialize SRAM
    sram.chipSelect(10);
    sram.initialize();
    
    // Test the chip
    if (sram.testIC()) {
        Serial.println("SRAM ready!");
        
        // Write some data
        sram.writeByte(0x000000, 0xAB);
        
        // Read it back
        byte data = sram.readByte(0x000000);
        Serial.print("Read: 0x");
        Serial.println(data, HEX);
    }
}

void loop() {
    // Your main code here
}
```

### Multiple Chip Array Usage
```cpp
#include "23LCSRAM.h"

IC sram1, sram2, sram3;

void setup() {
    Serial.begin(9600);
    
    // Initialize multiple chips
    sram1.chipSelect(4);
    sram1.initialize();
    
    sram2.chipSelect(5);
    sram2.initialize();
    
    sram3.chipSelect(6);
    sram3.initialize();
    
    // Test all chips
    Serial.println("Testing SRAM array...");
    if (sram1.testIC() && sram2.testIC() && sram3.testIC()) {
        Serial.println("All chips ready!");
        
        // Write to different chips
        sram1.writeByte(0x000000, 0x11);
        sram2.writeByte(0x000000, 0x22);
        sram3.writeByte(0x000000, 0x33);
        
        // Read from all chips
        Serial.print("Chip 1: 0x");
        Serial.println(sram1.readByte(0x000000), HEX);
        Serial.print("Chip 2: 0x");
        Serial.println(sram2.readByte(0x000000), HEX);
        Serial.print("Chip 3: 0x");
        Serial.println(sram3.readByte(0x000000), HEX);
    }
}

void loop() {
    // Your main code here
}
```

### Automated Array Management (Recommended)
```cpp
#include "23LCSRAM.h"

ICArray sramArray;

void setup() {
    Serial.begin(9600);
    
    // Option 1: Initialize with consecutive pins (4, 5, 6, 7)
    if (sramArray.initializeArray(4, 4)) {
        Serial.println("Array initialized successfully!");
        
        // Test all chips at once
        if (sramArray.testAllChips()) {
            Serial.println("All chips are working!");
            
            // Write different data to each chip
            sramArray.writeByte(0, 0x000000, 0x11);  // Chip 0
            sramArray.writeByte(1, 0x000000, 0x22);  // Chip 1
            sramArray.writeByte(2, 0x000000, 0x33);  // Chip 2
            sramArray.writeByte(3, 0x000000, 0x44);  // Chip 3
            
            // Read back from all chips
            for (int i = 0; i < sramArray.getChipCount(); i++) {
                byte value = sramArray.readByte(i, 0x000000);
                Serial.print("Chip ");
                Serial.print(i);
                Serial.print(": 0x");
                Serial.println(value, HEX);
            }
            
            // Write same data to all chips
            sramArray.writeByteToAll(0x000100, 0xAA);
            
            // Write array data to a specific chip
            byte data[] = {0x10, 0x20, 0x30, 0x40, 0x50};
            sramArray.writeIC(0, 0x000200, data, 5);
        }
    } else {
        Serial.println("Failed to initialize array!");
    }
}

void loop() {
    // Your main code here
}
```

### Custom Pin Assignment Array
```cpp
#include "23LCSRAM.h"

ICArray sramArray;

void setup() {
    Serial.begin(9600);
    
    // Option 2: Initialize with custom pin assignment
    int customPins[] = {2, 7, 9, 10};  // Non-consecutive pins
    if (sramArray.initializeArray(4, customPins)) {
        Serial.println("Custom array initialized!");
        
        // Test and use the array
        sramArray.testAllChips();
        
        // Access individual chips
        sramArray.writeByte(0, 0x000000, 0xAA);  // Pin 2
        sramArray.writeByte(1, 0x000000, 0xBB);  // Pin 7
        sramArray.writeByte(2, 0x000000, 0xCC);  // Pin 9
        sramArray.writeByte(3, 0x000000, 0xDD);  // Pin 10
        
        // Broadcast same data to all chips
        byte broadcastData[] = {0x01, 0x02, 0x03, 0x04};
        sramArray.writeICToAll(0x001000, broadcastData, 4);
    }
}

void loop() {
    // Your main code here
}
```

### Array Data Handling
```cpp
// Writing and reading arrays
byte writeData[] = {0x10, 0x20, 0x30, 0x40, 0x50};
sram.writeIC(0x001000, writeData, 5);

byte readData[5];
sram.readIC(0x001000, readData, 5);

// Verify data
for (int i = 0; i < 5; i++) {
    Serial.print("Position ");
    Serial.print(i);
    Serial.print(": 0x");
    Serial.println(readData[i], HEX);
}
```

## Error Handling

The library includes comprehensive error checking:

- **Invalid pin numbers**: Warns about pins outside valid range (0-19)
- **SPI pin conflicts**: Prevents using SPI pins (11, 12, 13) as chip select
- **Uninitialized operations**: Blocks read/write on uninitialized chips
- **Serial feedback**: Provides clear error messages via Serial output

## Memory Addressing

- **23LC1024**: 128KB (0x00000 to 0x1FFFF)
- **Address format**: 24-bit addressing (supports larger chips)
- **Page boundaries**: No special page handling required
- **Sequential access**: Automatic address increment for multi-byte operations

## Array Construction Using ICArray

- **Limits**: Minimum of 2, maximum of 10

## Technical Notes

- Uses hardware SPI with `shiftOut()` and `shiftIn()` functions
- Clock polarity: CPOL = 0, CPHA = 0 (SPI Mode 0)
- Maximum SPI clock speed depends on your Arduino model
- Chip select is active LOW (standard SPI)
- All operations are blocking (synchronous)

## Troubleshooting

**Chip not responding?**
- Check wiring connections
- Verify power supply (3.3V or 5V depending on chip)
- Ensure chip select pin is not conflicting
- Try the `testIC()` function first

**Getting 0xFF on reads?**
- Usually indicates communication failure
- Check SPI connections
- Verify chip is properly initialized

**Multiple chips interfering?**
- Ensure each chip has unique chip select pin
- Check that unused chips are properly deselected
- Verify power supply can handle multiple chips

## License

Public Domain - Use at your own risk.

(Credit is appreciated though if you use this for any derivative work)
