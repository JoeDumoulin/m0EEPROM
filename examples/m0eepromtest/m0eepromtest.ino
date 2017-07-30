// m0eepromtest.ino - some simple tests of the m0eeprom library

#include "m0EEPROM.h"

uint8_t rdata[32];

// prototypes
//void read_after_reset();
void test_simple_read_write_one_byte();
void test_simple_read_write_two_bytes();
void test_write_one_byte_doesnt_erase_second_byte_in_memory();
void test_write_and_read_a_string();
void test_multiple_string_writes_over_page_boundaries();
void test_multiple_string_writes_over_page_boundaries(uint32_t offset);
void write_an_arbitrary_structure_to_eeprom();
void clear_eeprom_bytes(uint16_t offset);

void setup(void)
{
  Serial.begin(9600);
    while ( ! Serial ) {
      delay( 1 );
  }
  Serial.flush();
  Eeprom.begin();

//  read_after_reset();
  clear_eeprom_bytes(0x40);
  test_simple_read_write_one_byte();
  test_simple_read_write_two_bytes();
  test_write_one_byte_doesnt_erase_second_byte_in_memory();
  test_write_and_read_a_string();
  
  // offset for write start = 0
  test_multiple_string_writes_over_page_boundaries();
  // offset for write start = 27
  test_multiple_string_writes_over_page_boundaries(27);
  write_an_arbitrary_structure_to_eeprom();

}
 
void loop(){
}

/* start tests */
// two one-byte reads to be used after a reset to ensure that eeprom is stable.  
//  enable after you run tests once.
/*
void read_after_reset() {
  unsigned int address = 0;
  Serial.println("read_after_reset begin");
  Serial.println(Eeprom.read_byte(address), DEC);
  Serial.println(Eeprom.read_byte(address+1), DEC);

  Serial.println("read_after_reset end");
}
*/
void test_simple_read_write_one_byte() {
  unsigned int address = 0x40;
  Serial.println("test_simple_read_write_one_byte begin");
//  Eeprom.write_byte(address, 0);
  uint8_t w = 123;
  Eeprom.write_byte(address, w);
  
  Serial.println(Eeprom.read_byte(address), DEC);
  Serial.println("test_simple_read_write_one_byte end");
}

void test_simple_read_write_two_bytes() {
  unsigned int address = 0;
  Serial.println("test_simple_read_write_two_bytes begin");
  Eeprom.write_byte(address, (uint8_t) 123);
  Eeprom.write_byte(address+1, 66);
  
  Serial.println(Eeprom.read_byte(address), DEC);
  Serial.println(Eeprom.read_byte(address+1), DEC);
  
  Serial.println("test_simple_read_write_two_bytes end");
}

// The second byte in memory is preserved here
void test_write_one_byte_doesnt_erase_second_byte_in_memory() {
  uint16_t address = 0;
  Serial.println("test_write_one_byte_doesnt_erase_second_byte_in_memory begin");
  Eeprom.write_byte(address, 123);
  Eeprom.write_byte(address+1, 66);
  
  Eeprom.write_byte(address, 1);

  Serial.println(Eeprom.read_byte(address), DEC);
  Serial.println(Eeprom.read_byte(address+1), DEC);

  Serial.println("test_write_one_byte_doesnt_erase_second_byte_in_memory end");

}

void test_write_and_read_a_string() {
  char p[] = "Once again.";
  uint16_t psize = strlen(p)+1;
  uint8_t recv[psize]; // where to put the result
  uint16_t address = 0;
  Serial.println("test_write_and_read_a_string begin");

  Serial.println(p);
  uint8_t* rp = (uint8_t*)p;

  for (uint8_t i=0; i<psize; i++) {
    Serial.print(rp[i], HEX);
  }
  Serial.println();
  
  Eeprom.write_bytes(address, rp, psize);
  uint8_t* r = Eeprom.read_bytes(address, recv, psize);

  if (r != 0) {
 //   Serial.println("error");
    Serial.print((char*)r);Serial.println();
  }
    

  for (uint8_t i=0; i<psize; i++) {
    Serial.print(recv[i], HEX);
  }
  Serial.println();

  Serial.println((char*)recv);
  
  Serial.println("test_write_and_read_a_string end");
}

void test_multiple_string_writes_over_page_boundaries(){
  test_multiple_string_writes_over_page_boundaries(0);
}

void test_multiple_string_writes_over_page_boundaries(uint32_t offset){
  size_t i = 0;

  // define large string of data to be written
  char str_data[]={"Hello-1234567890-and-abcdefghijklmnopqrstuvwxyz-Goodbye\n"};
  size_t str_len = strlen(str_data);
  Serial.print("string length = "); Serial.println(str_len);

  
  // Write out data several times consecutively starting at address 0
  for (i=0;i<10;i++) Eeprom.write_bytes(i*str_len+offset,(uint8_t*)str_data, str_len);

  // read back the data 28 bytes at a time
  // reading data doesn't suffer from the page boundary rules
  Serial.println("DATA READ");
  for(i=0;i<20;i++) {
    Eeprom.read_bytes((i*28)+offset, rdata, 28);
    Serial.write(rdata,28);
  }
}

void write_an_arbitrary_structure_to_eeprom(){
  // write a structure
  typedef
  struct Foo {
    int x;
    float f;
    char c;
    char s[10] = {0};
  } Foo;
  Foo foo;

  foo.x = 61200;
  foo.f = 9e7;
  foo.c = 'a';
  strcpy(foo.s, "Hello");

  size_t len = sizeof(foo);
  Serial.print("sizeof(foo) = ");Serial.print(len);Serial.println();

  char hexchr[2]; // for printing
  uint8_t rdbuf[len];
  uint8_t* p = (uint8_t*)&foo;

  Eeprom.write_bytes(0,p, len);
  Eeprom.read_bytes(0, rdbuf, len);

  Foo* bar = (Foo*) &rdbuf;
  Serial.print("bar->x = ");Serial.print(bar->x);Serial.println();
  Serial.print("bar->f = ");Serial.print(bar->f);Serial.println();
  Serial.print("bar->c = ");Serial.print(bar->c);Serial.println();
  Serial.print("bar->s = ");Serial.print(bar->s);Serial.println();

  // print rdbuf size
  for (size_t i = 0; i < len; ++i){
    sprintf(hexchr, "%02X", rdbuf[i]);
    Serial.print(hexchr);
  }
  Serial.println();
}

void clear_eeprom_bytes(uint16_t offset){
  // write zeros for 10 bytes starting at offset.
  Serial.println("clear_eeprom_bytes begin");
  uint8_t buff[10] = {0};
  Eeprom.write_bytes(offset, buff, 10);

  uint8_t read_buff[10];
  Eeprom.read_bytes(offset, read_buff, 10);
  Serial.println((byte) read_buff[1], DEC);
  Serial.println("clear_eeprom_bytes end");

  
}

