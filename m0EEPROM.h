// m0EEPROM.h - definitions for 24LC256 EEPROM read and write for m0 SAMD21G18 and adafruit feather
#ifndef M0EEPROM_H
#define M0EEPROM_H

class eeprom {
    int _device_address; // chip address.  currently only supporting one
public:
    begin(device_address=0x50) : _device_address(device_address){}

    // read a byte from the specified address in eeprom memory
    uint8_t read_byte(uint16_t address);

    // write a single byte of data to the given address in eeprom memory
    void write_byte(uint16_t address, uint8_t value);

    // read a structure of arbitrary size from address.
    // the size of the structure must be less than 64K-address.
    // returns a byte array of memory specified.  The byte array data
    // must be preallocated to the expected size.
    uint8_t* read_bytes(uint16_t address, uint8_t* data, uint16_t size);

    //  write a structure of arbitrary size to memory at address.
    //  the size of the structure must be less than 64K-address.
    //  The structure is passed as a byte array.
    void write_bytes(uint16_t address, uint8_t* data, uint16_t size);`
};

#endif // M0EEPROM_H
