/* 
 * RemoteSwitch library v3.1.1 Further extended by Jeroen Meijer
 * 3.1.1: added CnLedDim1 (chinese LED strip dimmer)
 * 3.1.0: ifdef around all types to reduce footprint, fixed ENER002 button 4 bug
 * 3.0.9: added ENER002 (= Action Eurodomest 972080)
 * 3.0.8: added EverFlourish and fixed a bg in ElroAb440 (on and off were swapped)
 * 3.0.7: added ElroAb440
 * 3.0.6: added HomeEasyEU
 * 3.0.5: debugged noname and changed interface
 * 3.0.4: added Blokker3
 * 3.0.3: added Kika2
 * 3.0.2: added noname
 * 3.0.1: added Blokker2
 * 3.0.0: made compile
 * Based on v2.0.0 made by Randy Simons http://randysimons.nl
 * See RemoteSwitchSender.h for details.
 *
 * License: "Free BSD license". See license.txt
 */

#include "RemoteSwitch.h"


/************
* RemoteSwitch
************/

RemoteSwitch::RemoteSwitch(unsigned short pin, unsigned int periodusec, unsigned short repeats) {
	_pin=pin;
	_periodusec=periodusec;
	_repeats=repeats;
	
	pinMode(_pin, OUTPUT);
}

unsigned long RemoteSwitch::encodeTelegram(unsigned short trits[]) {
	unsigned long data = 0;
		
	//Encode data
	for (unsigned short i=0;i<12;i++) {
		data*=3;
		data+=trits[i];
	}
	
	//Encode period duration
	data |= (unsigned long)_periodusec << 23;
	
	//Encode repeats
	data |= (unsigned long)_repeats << 20;
	
	return data;
}

#ifdef ModEncodingPT
void RemoteSwitch::sendTelegramPT(unsigned short trits[]) {
	sendTelegramPT(encodeTelegram(trits),_pin);	
}

/**
* Format data:
* pppppppp|prrrdddd|dddddddd|dddddddd (32 bit)
* p = perioud (9 bit unsigned int
* r = repeats as 2log. Thus, if r = 3, then signal is sent 2^3=8 times
* d = data
*/
void RemoteSwitch::sendTelegramPT(unsigned long data, unsigned short pin) {
	unsigned int periodusec = (unsigned long)data >> 23;
	unsigned short repeats = 1 << (((unsigned long)data >> 20) & B111);
	data = data & 0xfffff; //truncate to 20 bit
		
	//Convert the base3-code to base4, to avoid lengthy calculations when transmitting.. Messes op timings.
	unsigned long dataBase4 = 0;
	
	for (unsigned short i=0; i<12; i++) {
		dataBase4<<=2;
		dataBase4|=(data%3);
		data/=3;
	}
	
	for (unsigned short int j=0;j<repeats;j++) {		
		//Sent one telegram		
		
		//Use data-var as working var
		data=dataBase4;
		for (unsigned short i=0; i<12; i++) {
			switch (data & B11) {
				case 0:
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec*3);
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec*3);
					break;
				case 1:
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec*3);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec);
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec*3);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec);
					break;
				case 2: //AKA: X or float
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec*3);
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec*3);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec);
					break;
			}
			//Next trit
			data>>=2;
		}
		
		//Send termination/synchronisation-signal. Total length: 32 periods
		digitalWrite(pin, HIGH);
		delayMicroseconds(periodusec);
		digitalWrite(pin, LOW);
		delayMicroseconds(periodusec*31);
	}
}
#endif

#ifdef ModEncodingKaKu2
void RemoteSwitch::sendTelegramKaKu2(unsigned long data, unsigned int periodusec, unsigned short repeats) {
	sendTelegramKaKu2(data,periodusec,repeats,_pin);	
}

void RemoteSwitch::sendTelegramKaKu2(unsigned long data, unsigned int periodusec, unsigned short repeats, unsigned short pin) {
	repeats = 1 << (repeats & B111);
	//Reverse the bits
	unsigned long dataBase2 = 0;
	
	for (unsigned short i=0; i<32; i++) {
		dataBase2<<=1;
		dataBase2|=(data & B1);
		data>>=1;
	}
	
	for (unsigned short int j=0;j<repeats;j++) {		
		digitalWrite(pin, HIGH);
		delayMicroseconds(periodusec);
		digitalWrite(pin, LOW);
		delayMicroseconds(periodusec*9);
		data = dataBase2;
		for (unsigned short i=0; i<32; i++) {
			switch (data & B1) {
				case 0:
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec);
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec*3);
					break;
				case 1:
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec*3);
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec);
					break;
			}
			//Next bit
			data>>=1;
		}
		
		//Send termination/synchronisation-signal. Total length: 32 periods
		digitalWrite(pin, HIGH);
		delayMicroseconds(periodusec);
		digitalWrite(pin, LOW);
		delayMicroseconds(periodusec*31);
	}
}
#endif

#ifdef ModEncodingHomeEasyEU
// Format HomeEasy EU
void RemoteSwitch::sendTelegramHomeEasyEU(unsigned long data1, unsigned long data2, unsigned int periodusec, unsigned short repeats) {
	sendTelegramHomeEasyEU(data1,data2,periodusec,repeats,_pin);	
}

void RemoteSwitch::sendTelegramHomeEasyEU(unsigned long data1, unsigned long data2, unsigned int periodusec, unsigned short repeats, unsigned short pin) {
	repeats = 1 << (repeats & B111);
	//Reverse the bits
	unsigned long data1Base2 = 0;
	unsigned long data2Base2 = 0;
	unsigned short i=0;
	
	for (i=0; i<32; i++) {
		data1Base2<<=1;
		data1Base2|=(data1 & B1);
		data1>>=1;
		data2Base2<<=1;
		data2Base2|=(data2 & B1);
		data2>>=1;
	}
	
	for (unsigned short int j=0;j<repeats;j++) {		
		data1 = data1Base2;
		for (i=0; i<57; i++) {
			if (i==32) {
				data1 = data2Base2;
			}
			switch (data1 & B1) {
				case 0:
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec*5);
					break;
				case 1:
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec);
					break;
			}
			//Next bit
			data1>>=1;
		}
		
		//Send termination/synchronisation-signal. Total length: 8 periods
		digitalWrite(pin, HIGH);
		delayMicroseconds(periodusec);
		digitalWrite(pin, LOW);
		delayMicroseconds(periodusec*8);
	}
}
#endif

#ifdef ModEncodingEner002
// Format ENER002
void RemoteSwitch::sendTelegramEner002(unsigned long data, unsigned int periodusec, unsigned short repeats) {
	sendTelegramEner002(data,periodusec,repeats,_pin);	
}

void RemoteSwitch::sendTelegramEner002(unsigned long data, unsigned int periodusec, unsigned short repeats, unsigned short pin) {
	repeats = 1 << (repeats & B111);
	//Reverse the bits
	unsigned long dataBase2 = 0;
	unsigned short i=0;
	
	for (unsigned short i=0; i<24; i++) {
		dataBase2<<=1;
		dataBase2|=(data & B1);
		data>>=1;
	}
	
	for (unsigned short int j=0;j<repeats;j++) {
		data = dataBase2;
		for (unsigned short i=0; i<24; i++) {
			switch (data & B1) {
				case 0:
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec*3);
					break;
				case 1:
					digitalWrite(pin, HIGH);
					delayMicroseconds(periodusec*3);
					digitalWrite(pin, LOW);
					delayMicroseconds(periodusec);
					break;
			}
			//Next bit
			data>>=1;
		}
		
		//Send termination/synchronisation-signal.
		digitalWrite(pin, HIGH);
		delayMicroseconds(periodusec);
		digitalWrite(pin, LOW);
		delayMicroseconds(periodusec*35); // 3 to finish 0-bit, then 32 periods (8 bits) silence
	}
}
#endif

boolean RemoteSwitch::isSameCode(unsigned long encodedTelegram, unsigned long receivedData) {
	return (receivedData==(encodedTelegram & 0xFFFFF)); //Compare the 20 LSB's
}

#ifdef ModActionSwitch
/************
* ActionSwitch
************/

ActionSwitch::ActionSwitch(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}


void ActionSwitch::sendSignal(unsigned short systemCode, char device, boolean on) {		
	sendTelegramPT(getTelegram(systemCode,device,on), _pin);
}

unsigned long ActionSwitch::getTelegram(unsigned short systemCode, char device, boolean on) {
	unsigned short trits[12];
	
	device-=65;
	
	for (unsigned short i=0; i<5; i++) {
		//bits 0-4 contain address (2^5=32 addresses)
		trits[i]=(systemCode & 1)?1:2;          
		systemCode>>=1;
		
		//bits 5-9 contain device. Only one trit has value 0, others have 2 (float)!
		trits[i+5]=(i==device?0:2);
	}
	
	//switch on or off
	trits[10]=(!on?0:2);
	trits[11]=(on?0:2);
	
	return encodeTelegram(trits);
}
#endif

#ifdef ModElroAb440Switch
/************
* ElroAb440Switch
************/

ElroAb440Switch::ElroAb440Switch(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}

void ElroAb440Switch::sendSignal(unsigned short systemCode, char device, boolean on) {		
	sendTelegramPT(getTelegram(systemCode,device,on), _pin);
}

unsigned long ElroAb440Switch::getTelegram(unsigned short systemCode, char device, boolean on) {
	unsigned short trits[12];
	
	device-=65;
	
	for (unsigned short i=0; i<5; i++) {
		//bits 0-4 contain address (2^5=32 addresses)
		trits[i]=(systemCode & 1)?0:2;          
		systemCode>>=1;
		
		//bits 5-9 contain device. Only one trit has value 0, others have 2 (float)!
		trits[i+5]=(i==device?0:2);
	}
	
	//switch on or off
	trits[10]=(on?0:2);
	trits[11]=(!on?0:2);
	
	return encodeTelegram(trits);
}
#endif

#ifdef ModBlokkerSwitch
/************
* BlokkerSwitch
************/

BlokkerSwitch::BlokkerSwitch(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}


void BlokkerSwitch::sendSignal(unsigned short device, boolean on) {
	sendTelegramPT(getTelegram(device,on), _pin);
}

unsigned long BlokkerSwitch::getTelegram(unsigned short device, boolean on) {
	unsigned short trits[12]={0};
	
	device--;
	
	for (unsigned short i=1; i<4; i++) {
		//Bits 1-3 contain device 
		trits[i]=(device & 1)?0:1;          
		device>>=1;
	}
	
	//switch on or off
	trits[8]=(on?1:0);
	
	return encodeTelegram(trits);
}
#endif

#ifdef ModBlokkerSwitch2
/************
* BlokkerSwitch2
************/

BlokkerSwitch2::BlokkerSwitch2(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}


void BlokkerSwitch2::sendSignal(char address, unsigned short device, boolean on) {
	sendTelegramPT(getTelegram(address, device,on), _pin);
}

unsigned long BlokkerSwitch2::getTelegram(char address, unsigned short device, boolean on) {
	unsigned short trits[12]={0};
	
	// address data  (L4 type chip)
	trits[0]=(address == 'A')?0:2;
	trits[1]=(address == 'B')?0:2;
	trits[2]=(address == 'C')?0:2;
	trits[3]=(address == 'D')?0:2;
	trits[4]=(device  ==  1 )?0:2;
	trits[5]=(device  ==  2 )?0:2;
	trits[6]=(device  ==  3 )?0:2;
	trits[7]=2;

	//latch data (L4 type chip)
	//first 3 data bits are simply ignored
	trits[11]=(on?1:0);
	
	return encodeTelegram(trits);
}
#endif

#ifdef ModBlokkerSwitch3
/************
* BlokkerSwitch3
************/

BlokkerSwitch3::BlokkerSwitch3(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}


void BlokkerSwitch3::sendSignal(unsigned short device, boolean on) {
	sendTelegramPT(getTelegram(device, on), _pin);
}

unsigned long BlokkerSwitch3::getTelegram(unsigned short device, boolean on) {
	unsigned short trits[12]={0};
	device = 8 - device; // 1 -> 7, 2 -> 6 ...

	// address data  (L4 type chip)
	trits[1]=(device & B001)?1:0;
	trits[2]=(device & B010)?1:0;
	trits[3]=(device & B100)?1:0;

	//latch data (L4 type chip)
	//last 3 data bits are simply ignored
	trits[8]=(on?1:0);
	
	return encodeTelegram(trits);
}
#endif

#ifdef ModKaKSwitch
/************
* KaKuSwitch
************/

KaKuSwitch::KaKuSwitch(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}

void KaKuSwitch::sendSignal(char address, unsigned short device, boolean on) {
	sendTelegramPT(getTelegram(address, device, on), _pin);
}

unsigned long KaKuSwitch::getTelegram(char address, unsigned short device, boolean on) {
	unsigned short trits[12];
	
	address-=65;
	device-=1;
	
	for (unsigned short i=0; i<4; i++) {
		//bits 0-3 contain address (2^4 = 16 addresses)
		trits[i]=(address & 1)?2:0;          
		address>>=1;
		
		//bits 4-8 contain device (2^4 = 16 addresses)
		trits[i+4]=(device & 1)?2:0;          
		device>>=1;
	}
	
	//bits 8-10 seem to be fixed
	trits[8]=0;
	trits[9]=2;
	trits[10]=2;
	
	//switch on or off
	trits[11]=(on?2:0);
	
	return encodeTelegram(trits);
}

void KaKuSwitch::sendSignal(char address, unsigned short group, unsigned short device, boolean on) {
	sendTelegram(getTelegram(address, group, on), _pin);
}

unsigned long KaKuSwitch::getTelegram(char address, unsigned short group, unsigned short device, boolean on) {
	unsigned short trits[12], i;
	
	address-=65;
	group-=1;
	device-=1;
	
	//address. M3E Pin A0-A3
	for (i=0; i<4; i++) {
		//bits 0-3 contain address (2^4 = 16 addresses)
		trits[i]=(address & 1)?2:0;          
		address>>=1;		
    }
		
	//device. M3E Pin A4-A5
	for (; i<6; i++) {
		trits[i]=(device & 1)?2:0;          
		device>>=1;
	}
	
	//group. M3E Pin A6-A7
	for (; i<8; i++) {
		trits[i]=(group & 1)?2:0;          
		group>>=1;
	}
	
	//bits 8-10 are be fixed. M3E Pin A8/D0-A10/D2
	trits[8]=0;
	trits[9]=2;
	trits[10]=2;
	
	//switch on or off, M3E Pin A11/D3
	trits[11]=(on?2:0);
	
	return encodeTelegram(trits);
}
#endif

#ifdef ModKaKuSwitch2
/************
* KaKuSwitch2
************/

KaKuSwitch2::KaKuSwitch2(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}

void KaKuSwitch2::sendSignal(unsigned long address, boolean group, unsigned short device, boolean on) {
	unsigned long data;
	data = (address & 0x3ffffff) << 6; // 26 bits address
	data |= (group & B1) << 5;
	data |= (on & B1) << 4;
	data |= (device & 0xf);
	sendTelegramKaKu2(data, _periodusec, _repeats, _pin);
}
#endif

#ifdef ModNoName1Switch
/************
* NoName1Switch
************/

NoName1Switch::NoName1Switch(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}


void NoName1Switch::sendSignal(unsigned short button) {		
	switch (button) {
		case 0:
			sendTelegramPT(getTelegram(12), _pin);	// switch both channels off
			break;
		case 1:
			sendTelegramPT(getTelegram(12), _pin);	// switch both channels off
			delay (20);
			sendTelegram(getTelegram(4), _pin);	// switch channel one on
			break;
		case 2:
			sendTelegramPT(getTelegram(12), _pin);	// switch both channels off
			delay (20);
			sendTelegramPT(getTelegram(2), _pin);	// switch channel two on
			break;
		case 3:
			sendTelegramPT(getTelegram(3), _pin);	// switch both channels on
			break;
	}
}

unsigned long NoName1Switch::getTelegram(unsigned short button) {
	unsigned short trits[12];

	//address
	trits[0]=1;
	trits[1]=2;
	trits[2]=0;
	trits[3]=2;
	trits[4]=0;
	trits[5]=1;
	trits[6]=1;
	trits[7]=1;
	// data
	trits[ 8] = (button & B1000)?1:0;
	trits[ 9] = (button & B0100)?1:0;
	trits[10] = (button & B0010)?1:0;
	trits[11] = (button & B0001)?1:0;


	return encodeTelegram(trits);
}
#endif

#ifdef ModHomeEasyEUSwitch
/************
* HomeEasyEU
************/

HomeEasyEUSwitch::HomeEasyEUSwitch(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}

void HomeEasyEUSwitch::sendSignal(unsigned long address, boolean group, unsigned short device, boolean on) {
	unsigned long data1;
	unsigned long data2;

	data1  = 0x38600000;			// preamble		bits 31-21
	data1 |= address >> 11;			//			bits 20-00

	data2 = (address & 0x000007ff) << 21;	//			bits 31-21
	data2 |= 0x00080000;			// CCCC group 0100	bits 20-17
	data2 |= on?0:0x00010000;		// DD group		bits 16-15
	data2 |= on?0x00008000:0;
	data2 |= group?0:0x00004000;		// EE group		bits 14-13
	data2 |= 0;
	data2 |= (device & 0x3f) << 7;		// FF group		bits 12-07
	sendTelegramHomeEasyEU(data1, data2, _periodusec, _repeats, _pin);
}
#endif

#ifdef ModEverFlourishSwitch2
/************
* EverFlourish
************/

EverFlourishSwitch::EverFlourishSwitch(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}


void EverFlourishSwitch::sendSignal(char address, unsigned short device, boolean on) {
	sendTelegramPT(getTelegram(address, device,on), _pin);
}

unsigned long EverFlourishSwitch::getTelegram(char address, unsigned short device, boolean on) {
	unsigned short trits[12]={0};
	
	// address data  (L4 type chip)
	trits[0]=(address == 'A')?0:2;
	trits[1]=(address == 'B')?0:2;
	trits[2]=(address == 'C')?0:2;
	trits[3]=(address == 'D')?0:2;
	trits[4]=(device  ==  1 )?0:2;
	trits[5]=(device  ==  2 )?0:2;
	trits[6]=(device  ==  3 )?0:2;
	trits[7]=2;
	trits[8]=2;
	trits[9]=2;
	trits[10]=2;
	//first 3 data bits are simply ignored
	trits[11]=(on?1:0);
	
	return encodeTelegram(trits);
}
#endif

#ifdef ModEner002Switch
/************
* Ener002
************/

Ener002Switch::Ener002Switch(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}

void Ener002Switch::sendSignal(unsigned long address, unsigned short device, boolean on) {
	unsigned long data;
	data = (address & 0x00fffff) << 4; // 20 bits address

	switch (device) {
		case 1:
			data |= B1110;
			data |= (on & B1);
			break;
		case 2:
			data |= B1100;
			data |= (on & B1);
			break;
		case 3:
			data |= B1010;
			data |= (on & B1);
			break;
		case 4:
			data |= B0110;
			data |= (on & B1);
			break;
		default:
			data |= (on & B1) ? B0001 : B0010;
			break;
	}
	sendTelegramEner002(data, _periodusec, _repeats, _pin);
}
#endif

#ifdef ModCnLedDim1Switch
/************
* CnLedDim1
************/

CnLedDim1Switch::CnLedDim1Switch(unsigned short pin, unsigned int periodusec) : RemoteSwitch(pin,periodusec,3) {
	//Call contructor
}

void CnLedDim1Switch::sendSignal(unsigned long address, unsigned short button) {
	unsigned long data;
	data = (address & 0x008ffff) << 5; // 19 bits address
	data |= (button & 0x1f);
	sendTelegramEner002(data, _periodusec, _repeats, _pin);
}
#endif

