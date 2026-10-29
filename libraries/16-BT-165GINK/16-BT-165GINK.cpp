#include <SPI.h>
#include "16-BT-165GINK.h"


void vfd_init(){
	char c;
	SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
    digitalWrite(3, LOW);  //CS pin to low, enable data read
    delayMicroseconds(1);
    c = 0b00000111;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00001111;
    SPI.transfer(c);
    delay(1);
    digitalWrite(3, HIGH);   //CS pin to high, disable data read
    SPI.endTransaction();
    delayMicroseconds(5);
    //[ % 0x07 0xF0 % ]


    //[ % 0x27 0x04 % ]
    SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
    digitalWrite(3, LOW);
    delayMicroseconds(1);
    c = 0b00100111;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000100;
    SPI.transfer(c);
    delay(1);
    digitalWrite(3, HIGH);
    SPI.endTransaction();
    delayMicroseconds(5);
    //[ % 0x27 0x04 % ]


    //[ % 0x05 0x07 0x07 % ]
    SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
    digitalWrite(3, LOW);
    delayMicroseconds(1);
    c = 0b00000101;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000111;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000111;
    SPI.transfer(c);
    delay(1);
    digitalWrite(3, HIGH);
    SPI.endTransaction();
    delayMicroseconds(5);
    //[ % 0x05 0x07 0x07 % ]


    //[ % 0x03 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 % ]
    SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
    digitalWrite(3, LOW);
    delayMicroseconds(1);
    c = 0b00000011;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x1
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x2
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x3
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x4
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x5
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x6
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x7
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x8
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x9
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x10
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x11
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x12
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x13
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x15
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x15
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b11000000;             //x16
    SPI.transfer(c);
    delay(1);
    digitalWrite(3, HIGH);
    SPI.endTransaction();
    delayMicroseconds(5);
    //[ % 0x03 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 0xC0 % ]


    //[ % 0x62 0xAA 0x54 0xAB 0x5C 0xAA 0x54 0xAA 0x54 0xAA 0x54 % ]
    SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
    digitalWrite(3, LOW);
    delayMicroseconds(1);
    c = 0b01100010;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b10101010;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b01010100;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b10101011;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b01011100;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b10101010;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b01010100;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b10101010;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b01010100;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b10101010;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b01010100;
    SPI.transfer(c);
    delay(1);
    digitalWrite(3, HIGH);
    SPI.endTransaction();
    delayMicroseconds(5);
    //[ % 0x62 0xAA 0x54 0xAB 0x5C 0xAA 0x54 0xAA 0x54 0xAA 0x54 % ]


    //[ % 0x17 % ]
    SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
    digitalWrite(3, LOW);
    delayMicroseconds(1);
    c = 0b00010111;
    SPI.transfer(c);
    delay(1);
    digitalWrite(3, HIGH);
    delay (1);
    digitalWrite(3, LOW);
    SPI.endTransaction();
    delayMicroseconds(5);
    //[ % 0x17 % ]


    //[ % 0x27 0x04 % ]
    SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
    digitalWrite(3, LOW);
    delayMicroseconds(1);
    c = 0b00100111;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000100;
    SPI.transfer(c);
    delay(1);
    digitalWrite(3, HIGH);
    SPI.endTransaction();
    delayMicroseconds(5);
    //[ % 0x27 0x04 % ]


    //[ % 0x42 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 % ]
    SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
    digitalWrite(3, LOW);
    delayMicroseconds(1);
    c = 0b01000010;
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x1
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x2
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x3
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x4
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x5
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x6
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x7
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x8
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x9
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x10
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x11
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x12
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x13
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                    //x15
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x15
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x16
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x17
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                   //x18
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                 //x19
    SPI.transfer(c);
    delayMicroseconds(15);
    c = 0b00000000;                 //x20
    SPI.transfer(c);
    delay(1);
    digitalWrite(3, HIGH);
    SPI.endTransaction();
    delayMicroseconds(5);
    //[ % 0x42 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 % ]
}




void vfd_write_caracter(const char *str, int pos)
{
	SPI.beginTransaction(SPISettings(400000, MSBFIRST, SPI_MODE0));
	digitalWrite(3, LOW);
	delayMicroseconds(1);
	
	if(pos == 0){
		SPI.transfer(0b00000100);
		delayMicroseconds(15);
		SPI.transfer(0b00100000);
		delayMicroseconds(15);
	}
	if(pos == 1){
		SPI.transfer(0b00000000);
		delayMicroseconds(15);
		SPI.transfer(0b00100000);
		delayMicroseconds(15);
	}
	
	while(*str){ 
    int n[8];
    for (int i = 0; i < 8; i++){
    n[i] = bitRead(*str, i);
    }
    byte together = 0;
    for (int i = 0; i < 8; i++){
    together += (n[i] << (7-i));
    }
    SPI.transfer(together);
    delayMicroseconds(15);
		str++; 
 }
	SPI.transfer(0b01000000);
	
	delay(1);
	digitalWrite(3, HIGH);
	SPI.endTransaction();
	delayMicroseconds(5);
}



	
