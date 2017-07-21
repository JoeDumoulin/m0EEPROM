// m0EEPROM.cpp - implementation of eeprom memory library
//

#include "Arduino.h"
#include "Wire.h"
#include "RingBuffer.h" // for SERIAL_BUFFER_SIZE

#include "m0EEPROM.h

#define PAGE_SIZE 64 // page size of the 24LC256 in bytes

// get the uint log based 2 of the number.
uint8_t log2uint(uint32_t v) {
  uint32_t z = 0;
  while (v >>=1) z++;
  return z;
}

// read a byte from the specified address in eeprom memory
uint8_t eeprom::read_byte(uint16_t address) {
  uint8_t rdata = 0xFF;
   
  Wire.beginTransmission(_device_address);
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB
         
  Wire.requestFrom(_device_address,1);
           
  if (Wire.available()) rdata = Wire.read();
             
  return rdata;

}

// write a single byte of data to the given address in eeprom memory
void eeprom::write_byte(uint16_t address, uint8_t value) {
  Wire.beginTransmission(_device_address);
    
  Wire.write((int)(address >> 8));   // MSB
  Wire.write((int)(address & 0xFF)); // LSB
        
  Wire.write(data);

  Wire.endTransmission();
           
  delay(6); // at least 5ms for quiescence
}

// read a structure of arbitrary size from address.
// the size of the structure must be less than 64K-address.
// returns a byte array of memory specified.  The byte array data
// must be preallocated to the expected size.
uint8_t* eeprom::read_bytes(uint16_t start_address, uint8_t* data, uint16_t size) {
  unsigned char i=0;
  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();

  Wire.requestFrom(deviceaddress,num_chars);

  while(Wire.available()) data[i++] = Wire.read();

}

//  write a structure of arbitrary size to memory at address.
//  the size of the structure must be less than 64K-address.
//  The structure is passed as a byte array.
void eeprom::write_bytes(uint16_t address, uint8_t* data, uint16_t size) {
  // Uses Page Write for 24LC256
  // Allows for EEPROM_PAGE_SIZE byte page boundary
  // Splits string into max BUFFER_SIZE byte writes
//  uint8_t counter=0;
  unsigned int  address;
  uint32_t  first_page_len;
  unsigned int  num_writes;
//  unsigned int  data_len=0;
  unsigned char first_write_size;
  unsigned char last_write_size;  
  unsigned char write_size;  
                      
  // get the amount of memory to write in the first page of eeprom
  uint8_t page_bits = log2uint(EEPROM_PAGE_SIZE);
  first_page = ((start_address >> page_bits) +1) * EEPROM_PAGE_SIZE -startaddress;
  
  // get the size of the first write based on the I2C buffer size
  uint8_t buffer_bits = log2uint(SERIAL_BUFFER_SIZE);
  if (first_page > SERIAL_BUFFER_SIZE){
      first_write_size = first_page - (first_page >> buffer_bits) * SERIAL_BUFFER_SIZE;
      if (first_write_size == 0) first_write_size = SERIAL_BUFFER_SIZE;
  }
  else
      first_write_size = first_page;

  // now get the size of the last write in the final page 
  // and the total number of pages to write.
  if (size > first_write_size){
      last_write_size = (size - first_write_size) % SERIAL_BUFFER_SIZE;
      num_writes = ((data_len - first_write_size) >> (buffer_bits)) + 2;
  }
  else
      num_writes = 1;

  //  Write loop
  uint8_t i = 0;
  for (uint32_t page = 0; page < num_writes; ++page){
      if(page==0) 
          write_size = first_write_size;
      else if (page == (num_writes-1)) 
          write_size = last_write_size;
      else write_size = SERIAL_BUFFER_SIZE;

      // I2C handshake and comms happens here 
      Wire.beginTransmission(_device_address);
      Wire.write((int)((address) >> 8));   // MSB
      Wire.write((int)((address) & 0xFF)); // LSB
      for (uint8_t counter=0; counter<write_size; ++counter,++i){
          Wire.write(data[i]);
     }
     Wire.endTransmission();
     address += write_size;

     delay(6);  // needs 5ms for page write
}




