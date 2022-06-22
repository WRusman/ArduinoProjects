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

#ifndef RemoteSwitch_h
#define RemoteSwitch_h

#include "Arduino.h"

#define ModActionSwitch
//#define ModElroAb440Switch
//#define ModBlokkerSwitch
//#define ModBlokkerSwitch2
//#define ModBlokkerSwitch3
//#define ModKaKuSwitch
#define ModKaKuSwitch2
//#define ModNoname1Switch
//#define ModHomeEasyEUSwitch
//#define ModKEverFlourishSwitch
#define ModEner002Switch
#define ModCnLedDim1Switch


#ifdef ModActionSwitch
#define ModEncodingPT
#endif

#ifdef ModElroAb440Switch
#define ModEncodingPT
#endif

#ifdef ModBlokkerSwitch
#define ModEncodingPT
#endif

#ifdef ModBlokkerSwitch2
#define ModEncodingPT
#endif

#ifdef ModBlokkerSwitch3
#define ModEncodingPT
#endif

#ifdef ModKakuSwitch
#define ModEncodingPT
#endif

#ifdef ModKaKuSwitch2
#define ModEncodingKaKu2
#endif

#ifdef ModNoname1Switch
#define ModEncodingPT
#endif

#ifdef ModHomeEasyEUSwitch
#define ModEncodingHomeEasyEU
#endif

#ifdef ModEverFlourishSwitch
#define ModEverFlourishPT
#endif

#ifdef ModEner002Switch
#define ModEncodingEner002
#endif

#ifdef ModCnLedDim1Switch
#define ModEncodingEner002
#endif


/**
* RemoteSwitch provides a generic class for simulation of common RF remote controls, like the 'Klik aan Klik uit'-system 
* (http://www.klikaanklikuit.nl/), used to remotely switch lights etc.
*
* Many of these remotes seem to use a 433MHz SAW resonator and one of these chips: LP801B,  HX2262, PT2262, M3E.
* Datasheet for the HX2262/PT2262 ICs:
* http://www.princeton.com.tw/downloadprocess/downloadfile.asp?mydownload=PT2262_1.pdf
*
* Hardware required for this library: a 433MHz SAW oscillator transmitter, e.g.
* http://www.sparkfun.com/commerce/product_info.php?products_id=7815
* http://www.conrad.nl/goto/?product=130428
*
* Notes:
* - Since these chips use (and send!) tri-state inputs (low, high and floating) I use 'trits' instead of 'bits',
*   when appropriate.
* - I measured the period lengths with a scope.  Thus: they work for my remotes, but may fail for yours...
*   A better way would be to calculate the 'target'-timings using the  datasheets and the resistor-values on the remotes. 
*/
class RemoteSwitch {
	public:
		/**
		* Constructor.
		*
		* To obtain the correct period length, an oscilloscope is convenient, but you can also read the
		* datasheet of the transmitter, measure the resistor for the oscillator and calculate the freqency.
		*
		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	[0..511] Duration of one period, in microseconds. A trit is 6 periods.
		* @param repeats	[0..7] The 2log-Number of times the signal is repeated. The actual number of repeats will be 2^repeats. 3 would be a good start.
		*/		
		RemoteSwitch(unsigned short pin, unsigned int periodusec, unsigned short repeats);
		
		/**
		* Encodes the data base on the current object and the given trits. The data can be reused, e.g.
		* for use with the static version of sendTelegram, so you won't need to instantiate costly objects!
		*
		* @return The data suited for use with RemoteSwitch::sendTelegram.
		*/
		unsigned long encodeTelegram(unsigned short trits[]);
		
		/**
		* Send a telegram, including synchronisation-part.
		*
		* @param trits	Array of size 12. "trits" should be either 0, 1 or 2, where 2 indicaties "float"
		*/

#ifdef ModEncodingPT
		void sendTelegramPT(unsigned short trits[]);
		
		/**
		* Send a telegram, including synchronisation-part. The data-param encodes the period duration, number of repeats and the actual data.
		* Note: static method, which allows for use in low-mem situations.
		*
		* Format data:
		* pppppppp|prrrdddd|dddddddd|dddddddd (32 bit)
		* p = perioud (9 bit unsigned int
		* r = repeats as 2log. Thus, if r = 3, then signal is sent 2^3=8 times
		* d = data
		*
		* @param data data, period and repeats.
		* @param pin Pin number of the transmitter.
		*/
		static void sendTelegramPT(unsigned long data, unsigned short pin);
#endif
#ifdef ModEncodingKaKu2
		void sendTelegramKaKu2(unsigned long data, unsigned int periodusec, unsigned short repeats);
		static void sendTelegramKaKu2(unsigned long data, unsigned int periodusec, unsigned short repeats, unsigned short pin);
#endif
#ifdef ModEncodingHomeEasyEU		
		void sendTelegramHomeEasyEU(unsigned long data1, unsigned long data2, unsigned int periodusec, unsigned short repeats);
		static void sendTelegramHomeEasyEU(unsigned long data1, unsigned long data2, unsigned int periodusec, unsigned short repeats, unsigned short pin);
#endif
#ifdef ModEncodingEner002
		void sendTelegramEner002(unsigned long data, unsigned int periodusec, unsigned short repeats);
		static void sendTelegramEner002(unsigned long data, unsigned int periodusec, unsigned short repeats, unsigned short pin);
#endif
		/**
		* Compares the data received with RemoteReceive with the data obtained by one of the getTelegram-functions.
		* Period duration and repetitions are ignored by this function; only the data-payload is compared.
		*
		* @return true, if the codes are identical (the 20 least significant bits match)
		*/
		static boolean isSameCode(unsigned long encodedTelegram, unsigned long receivedData);
		
	protected:
		unsigned short _pin;		//Radio output pin
		unsigned int _periodusec;	//oscillator period in microseconds
		unsigned short _repeats;		//Number over repetitions of one telegram	
};

#ifdef ModActionSwitch
/**
* ActionSwitch simulatos a remote, as sold in the Dutch 'Action' stores. But there are many similar systems on the market.
* If your remote has setting for 5 address bits, and can control 5 devices on or off, then you can try to use the ActionSwitch
*/
class ActionSwitch: RemoteSwitch {
	public:
		/**
		* Constructor
		*
		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 190usec
		* @see RemoteSwitch
		*/
		ActionSwitch(unsigned short pin, unsigned int periodusec=190);
		
		/**
		* Send a on or off signal to a device.
		*
		* @param systemCode	5-bit addres (dip switches in remote). Range [0..31]
		* @param device	Device to switch. Range: [A..E] (case sensitive!)
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(unsigned short systemCode, char device, boolean on);
		
		/**
		* Generates the telegram (data) which can be used for RemoteSwitch::sendTelegram.
		* See sendSignal for details on the parameters
		*
		* @return Encoded data, including repeats and period duration.
		*/
		unsigned long getTelegram(unsigned short systemCode, char device, boolean on);
};
#endif

#ifdef ModElroAb440Switch
/**
* ElroAb440Switch simulatos a remote sold in Blokker stores. It is very similar to Action, butonly has 3 rows of buttons
*/
class ElroAb440Switch: RemoteSwitch {
	public:
		/**

		* Constructor
		*

		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 190usec
		* @see RemoteSwitch
		*/
		ElroAb440Switch(unsigned short pin, unsigned int periodusec=190);
		
		/**
		* Send a on or off signal to a device.
		*
		* @param systemCode	5-bit addres (dip switches in remote). Range [0..31]

		* @param device	Device to switch. Range: [A..E] (case sensitive!)
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(unsigned short systemCode, char device, boolean on);
		
		/**
		* Generates the telegram (data) which can be used for RemoteSwitch::sendTelegram.
		* See sendSignal for details on the parameters
		*
		* @return Encoded data, including repeats and period duration.

		*/
		unsigned long getTelegram(unsigned short systemCode, char device, boolean on);
};
#endif

#ifdef ModBlokkerSwitch
/**
* BlokkerSwitch simulates a remote, as sold in the Dutch 'Blokker' stores. But there are many similar systems on the market.
* These remotes have 4 on, 4 off buttons and a switch to switch between device 1-4 and 5-8. No futher configuration
* possible.
*/
class BlokkerSwitch: RemoteSwitch {
	public:
		/**
		* Constructor
		*
		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 307usec
		* @see RemoteSwitch
		*/
		BlokkerSwitch(unsigned short pin, unsigned int periodusec=230);
		
		/**
		* Send a on or off signal to a device.
		*		
		* @param device	Device to switch. Range: [1..8]
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(unsigned short device, boolean on);
		
		/**
		* @see RemoteSwitch::getTelegram
		*/
		unsigned long getTelegram(unsigned short device, boolean on);
};
#endif

#ifdef ModBlokkerSwitch2
/**
* BlokkerSwitch2 simulates a remote, as sold in the Dutch 'Blokker' stores. The small receivers are called SelectRemote No 1728029
* These remotes have 3 on, 3 off buttons and a rotary switch to switch between systemcode A, B, C or D.
* The receivers have a 12 position dial. The system uses L4 (latch 4 data bits) chips.
* 
*/
class BlokkerSwitch2: RemoteSwitch {
	public:
		/**
		* Constructor
		*

		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 307usec
		* @see RemoteSwitch
		*/
		BlokkerSwitch2(unsigned short pin, unsigned int periodusec=400);
		
		/**
		* Send a on or off signal to a device.
		*		

		* @param address	Address. Range: [A-D]
		* @param device		Device to switch. Range: [1..3]
		* @param on		True, to switch on. False to switch off,
		*/
		void sendSignal(char address, unsigned short device, boolean on);
		
		/**
		* @see RemoteSwitch::getTelegram
		*/
		unsigned long getTelegram(char address, unsigned short device, boolean on);
};
#endif

#ifdef ModBlokkerSwitch3
/**
* BlokkerSwitch3 simulates a remote, as sold in the Dutch 'Blokker' stores. The bulky receivers are also called SelectRemote No 1728029
* These remotes have 5 on, 5 off buttons. There is no system address
* The receivers have a 5 position rotary switch between device 1, 2, 3, 4 or 5. The system uses L4 (latch 4 data bits) chips.
* 
*/
class BlokkerSwitch3: RemoteSwitch {
	public:
		/**

		* Constructor
		*

		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 307usec

		* @see RemoteSwitch
		*/
		BlokkerSwitch3(unsigned short pin, unsigned int periodusec=230);
		
		/**

		* Send a on or off signal to a device.
		*		

		* @param address	Address. Range: [A-D]

		* @param device		Device to switch. Range: [1..3]
		* @param on		True, to switch on. False to switch off,
		*/
		void sendSignal(unsigned short device, boolean on);
		
		/**
		* @see RemoteSwitch::getTelegram
		*/
		unsigned long getTelegram(unsigned short device, boolean on);
};
#endif

#ifdef ModKaKuSwitch
/**
* KaKuSwitch simulates a KlikAanKlikUit-remote, but there are many clones.
* If your transmitter has a address dial with the characters A till P, you can try this class.
*/
class KaKuSwitch: RemoteSwitch {
	public:
		/**
		* Constructor
		*
		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 375usec
		* @see RemoteSwitch
		*/
		KaKuSwitch(unsigned short pin, unsigned int periodusec=375);
		
		/**
		* Send a on or off signal to a device.
		*
		* @param address	addres (dial switches in remote). Range [A..P] (case sensitive!)
		* @param group	Group to switch. Range: [1..4]
		* @param device	Device to switch. Range: [1..4]
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(char address, unsigned short group, unsigned short device, boolean on);
		
		/**
		* Send a on or off signal to a device.
		*
		* @param address	addres (dip switches in remote). Range [A..P] (case sensitive!)
		* @param device	device (dial switches in remote). Range [1..16]
		* @param on	True, to switch on. False to switch off,
		*/
		void sendSignal(char address, unsigned short device, boolean on);
		
		/**
		* @see RemoteSwitch::getTelegram
		*/
		unsigned long getTelegram(char address, unsigned short group, unsigned short device, boolean on);
		
		/**
		* @see RemoteSwitch::getTelegram
		*/
		unsigned long getTelegram(char address, unsigned short device, boolean on);
};
#endif

#ifdef ModKaKuSwitch2
/**
* KaKuSwitch2 simulates a KlikAanKlikUit-remote, this is the new series of remotes without dials and learning
* receivers. Actually, every transmitter simply has a huge 26 bits random address
*/
class KaKuSwitch2: RemoteSwitch {
	public:
		/**
		* Constructor
		*
		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 375usec
		* @see RemoteSwitch
		*/
		KaKuSwitch2(unsigned short pin, unsigned int periodusec=335);
		
		/**

		* Send a on or off signal to a device.
		*

		* @param address	addres 26 bits
		* @param group	Group bit
		* @param device	Device to switch. Range: [1..16]
		* @param on	True, to switch on. False to switch off,

		*/
		void sendSignal(unsigned long address, boolean group, unsigned short device, boolean on);
};
#endif

#ifdef ModNoname1Switch
/**

* Noname1Switch simulates a Chinese no brand remote, with just 5 buttons and no addressing
*/
class NoName1Switch: RemoteSwitch {
	public:
		/**
		* Constructor
		*
		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 375usec

		* @see RemoteSwitch
		*/
		NoName1Switch(unsigned short pin, unsigned int periodusec=270);
		
		/**
		* Send a on or off signal to a device.
		*
		* @param button		0 1: toggle channel 1
		*			1 2: toggle channel 2
		*			2 on: switch both channels on
		*			3 off: switch both channels off
		*			4 sleep: switches both channels off after a 10 second delay
		*/
		void sendSignal(unsigned short button);
		
		/**
		* @see RemoteSwitch::getTelegram

		*/
		unsigned long getTelegram(unsigned short button);
};
#endif

#ifdef ModHomeEasyEUSwitch
/**
* HomeEasyEU simulates a Home Easy, EU variant, remote (UK is the same as KlikAanKlikUit new series). This is the new series of remotes without dials and learning
* receivers. Actually, every transmitter simply has a huge 26 bits random address

*/
class HomeEasyEUSwitch: RemoteSwitch {
	public:
		/**

		* Constructor
		*
		* @param pin		output pin on Arduino to which the transmitter is connected

		* @param periodsec	Duration of one period, in microseconds. Default is 375usec
		* @see RemoteSwitch
		*/
		HomeEasyEUSwitch(unsigned short pin, unsigned int periodusec=275);
		
		/**

		* Send a on or off signal to a device.
		*

		* @param address	addres 26 bits
		* @param group	Group bit
		* @param device	Device to switch. Range: [1..16]

		* @param on	True, to switch on. False to switch off,

		*/
		void sendSignal(unsigned long address, boolean group, unsigned short device, boolean on);
};
#endif

#ifdef ModKEverFlourishSwitch
/**

* EverFlourish is sold by Praxis stores in the Netherlands. It is technically almost the same as Blokker2, only difference is trits 8-10 are wired to common.
* Spotted and tested by Ed Winter
* These remotes have 3 on, 3 off buttons and a rotary switch to switch between systemcode A, B, C or D.
* The receivers have a 12 position dial. The system uses L4 (latch 4 data bits) chips.
* 
*/
class EverFlourishSwitch: RemoteSwitch {
	public:
		/**
		* Constructor
		*

		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 307usec
		* @see RemoteSwitch
		*/
		EverFlourishSwitch(unsigned short pin, unsigned int periodusec=400);
		
		/**
		* Send a on or off signal to a device.
		*		

		* @param address	Address. Range: [A-D]
		* @param device		Device to switch. Range: [1..3]
		* @param on		True, to switch on. False to switch off,
		*/
		void sendSignal(char address, unsigned short device, boolean on);
		
		/**
		* @see RemoteSwitch::getTelegram
		*/
		unsigned long getTelegram(char address, unsigned short device, boolean on);
};
#endif

#ifdef ModEner002Switch
/**

* ENER002 is sold as EuroDomest by Action stores in the Netherlands. It is not a PT chipset, but a HS2303
* Extremly simple see coding https://www.pitt-pladdy.com/blog/_20140111-171131_0000_energenie_Remote_Control_Socket_Protocol_ENER002_/
* First spotted and tested by Ed Winter
* These remotes have 4 on, 4 off buttons and an on/off all pair.
* Each transmitter has a random, fixed address, comparable with the KaKu2

* 
*/
class Ener002Switch: RemoteSwitch {
	public:
		/**
		* Constructor
		*

		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 307usec
		* @see RemoteSwitch
		*/
		Ener002Switch(unsigned short pin, unsigned int periodusec=320);
		
		/**
		* Send a on or off signal to a device.
		*		
		* @param address	address 20 bits
		* @param device		Device to switch. Range: [1..4]. Any other value means "all"
		* @param on		True, to switch on. False to switch off,

		*/
		void sendSignal(unsigned long address, unsigned short device, boolean on);
		
		/**

		* @see RemoteSwitch::getTelegram

		*/
		unsigned long getTelegram(char address, unsigned short device, boolean on);
};
#endif

#ifdef ModCnLedDim1Switch
/**

* ModCnLedDim1 is a very cheap in-line PCM dimmer. It uses 1527 alike encoding

* The very flat remotes 11 buttons, in a 3 x 6 matrix in pos 1, 4-9, 11, 13, 15, 17
* Each transmitter has a random, fixed address, I assume 19 bits


* 
*/
class CnLedDim1Switch: RemoteSwitch {
	public:
		/**
		* Constructor
		*


		* @param pin		output pin on Arduino to which the transmitter is connected
		* @param periodsec	Duration of one period, in microseconds. Default is 307usec
		* @see RemoteSwitch

		*/
		CnLedDim1Switch(unsigned short pin, unsigned int periodusec=410);
		
		/**

		* Send a on or off signal to a device.
		*		
		* @param address	address 19 bits
		* @param button		1:pwr, 4:Light, 5:Bright+, 6:Bright-, 7:100%, 8:50%, 9:25%, 11:Mode+, 13:Speed-, 15:Speed+, 17:Mode-

		*/
		void sendSignal(unsigned long address, unsigned short button);
		
		/**


		* @see RemoteSwitch::getTelegram

		*/
		//unsigned long getTelegram(char address, unsigned short device, boolean on);
};
#endif

#endif
