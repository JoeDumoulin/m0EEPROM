// m0eepromtest.ino - some simple tests of the m0eeprom library

#include "m0eeprom.h"

uint8_t rdata[32];

void setup(void)
{
  Serial.begin(9600);
    while ( ! Serial ) {
      delay( 1 );
  }
  Serial.flush();

  Wire.begin();  
 
  size_t i;
  // define large string of data to be written
  cleareeprom();
  
  char str_data[]={"Hello-1234567890-and-abcdefghijklmnopqrstuvwxyz-Goodbye\n"};
  size_t str_len = strlen(str_data);
  // Work out length of data
//  char str_len=0;  
//  do{ str_len++; } while(str_data[str_len]);  
  
  // Write out data several times consecutively starting at address 0
  for(i=0;i<WRITE_CNT;i++) writeEEPROM(eeprom1,i*str_len,(uint8_t*)str_data, str_len);

  // read back the data 28 bytes at a time
  // reading data doesn't suffer from the page boundary rules
  Serial.println("DATA READ");
  for(i=0;i<20;i++) {
    readEEPROM(eeprom1, (i*28), rdata, 28);
    Serial.write(rdata,28);
  }

// write a structure
  typedef
  struct Foo {
    int x;
    float f;
    char c;
    char s[10];
  } Foo;
  Foo foo;

  foo.x = 61200;
  foo.f = 9e7;
  foo.c = 'a';
  strcpy(foo.s, "Hello");

  size_t len = sizeof(foo);
  Serial.print("sizeof(foo) = ");Serial.print(sizeof(foo));Serial.println();
  Serial.print("sizeof(int) = ");Serial.print(sizeof(int));Serial.println();
  Serial.print("sizeof(float) = ");Serial.print(sizeof(float));Serial.println();

  char hexchr[2]; // for printing
  uint8_t rdbuf[len];
  for (int j=0; j<len; j++) rdbuf[j]=0;
  for (int j=0; j<len; j++) {
    sprintf(hexchr, "%02X", rdbuf[j]);
    Serial.print(hexchr);
  }
  Serial.println();

  // some debugging
  uint8_t* p = (uint8_t*)&foo;
  for (int j=0; j<len; j++) {
    sprintf(hexchr, "%02X", p[j]);
    Serial.print(hexchr);
  }
  Serial.println();
  
  writeEEPROM(eeprom1,1000,p, len);
  readEEPROM(eeprom1,1000, rdbuf, len);

  Foo* bar = (Foo*) &rdbuf;
  Serial.print("bar->x = ");Serial.print(bar->x);Serial.println();
  Serial.print("bar->f = ");Serial.print(bar->f);Serial.println();
  Serial.print("bar->c = ");Serial.print(bar->c);Serial.println();
  Serial.print("bar->s = ");Serial.print(bar->s);Serial.println();

  // print rdbuf size
  for (size_t i = 0; i < len; ++i){
    sprintf(hexchr, "%02X", rdbuf[i]);
    Serial.print("rdbuf[");Serial.print(i);Serial.print("] = ");Serial.print(hexchr);Serial.println();
  }
}

void cleareeprom() {
  char Clear[] ={"000000000000000000000000000000000000000000000000000000000000000"};
  // Work out length of data
  char str_len=0;  
  do{ str_len++; } while(Clear[str_len]);  

  // Write out data several times consecutively starting at address 0
  for(size_t i=0;i<WRITE_CNT;i++) writeEEPROM(eeprom1,i*str_len,(uint8_t*)Clear, str_len);

}
 
void loop(){
}

