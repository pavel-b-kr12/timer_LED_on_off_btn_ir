//C:\_wr\_Arduino\_3\Nixie\NixieTubeClock_DS3231_Arduino\NixieTubeClock_DS3231_Arduino git\NixieTubeClockArduino_Bro_M2\NixieTubeClockArduino_Bro_M2.ino

//TODO use multiple led arrays

//#define debug

uint16_t on_d=15000;
uint16_t off_d=120000;

//uint16_t on_d=15000;
//uint16_t off_d=60000;

#define btn				A0
#define  irLED			5 //optional if use IR Infrared Receiver (TL1838, TSOP...) instead of button
//optional switches (jumpers)
#define sw_10sec_on		A1
#define sw_20sec_on		A2
#define sw_60sec_off	A3
#define sw_180sec_off	A4
#define brightness33	A5

#include <FastLED.h>
#define DATA_PIN	4
#define NUM_LEDS	15
byte NUM_LEDS_now=15;
#define NUM_LEDS_sw_p	10  //default -- 15 LEDs (5 BG+10 signal), when sw -- 10 LEDs (5+5)

#define LEDs_BG_E 5 //{BG leds (5), signal LEDS - all last (10)}
CRGB leds[NUM_LEDS];

#define ttab	Serial.print("\t");

void setup() {
	#ifdef debug
	Serial.begin(115200); 
	#endif
	
	pinMode (btn, INPUT_PULLUP);
	pinMode (sw_10sec_on, INPUT_PULLUP);
	pinMode (sw_20sec_on, INPUT_PULLUP);
	pinMode (sw_60sec_off, INPUT_PULLUP);
	pinMode (sw_180sec_off, INPUT_PULLUP);
	pinMode (brightness33, INPUT_PULLUP);
	pinMode (NUM_LEDS_sw_p, INPUT_PULLUP);
	
	delay(100);
	if(!digitalRead(sw_10sec_on))
		on_d=10000;
	if(!digitalRead(sw_20sec_on))
		on_d=20000;
	if(!digitalRead(sw_60sec_off))
		off_d=60000;
	if(!digitalRead(sw_180sec_off))
		off_d=180000;
	
	if(!digitalRead(NUM_LEDS_sw_p))
		NUM_LEDS_now=10;


	if(!digitalRead(brightness33))
		FastLED.setBrightness(33);
	else FastLED.setBrightness(99);
		
		#ifdef irLED
		pinMode (irLED, OUTPUT);
		tone(irLED,38000); //!! test freq
		#endif
		
		FastLED.addLeds<WS2812B,DATA_PIN,GRB>(leds, NUM_LEDS_now)
        .setCorrection( TypicalLEDStrip );
		
		for( byte i = 0; i < NUM_LEDS_now; i++) {
			leds[i]=CRGB::White;
			FastLED.show();
			delay(20);
			leds[i]=CRGB::Black;
	}
	
	FastLED.clear();
	FastLED.show();
}

long next_allow_btn_t=0;
long signal_t=0;
long off_t=-1;
bool bSignal=true;
bool bBG_light=false;
bool bAllowBtn=true;
void loop() {
	digitalWrite(LED_BUILTIN, !digitalRead(btn));
	
	#ifdef debug
	// Serial.print(millis()); Serial.print("\t sig:");
	// Serial.print(signal_t); Serial.print("\t");
	// Serial.print(millis()>signal_t ); Serial.print("\t off:");
	// Serial.print(off_t); Serial.print("\t");
	// Serial.print(millis()>off_t ); Serial.print("\t");
	// Serial.println();
	#endif
	
	if(millis()>signal_t) //show, anim signal (that allow to press btn
	{
		#ifdef irLED
		pinMode (irLED, OUTPUT);
		#endif
		signal_on();
	}
	if(off_t>-1 && millis()<off_t) //anim //?? compare negative namber fail
	{
		LED_on();
	}
	
	bAllowBtn=(bSignal|| bBG_light);
	
	if( bAllowBtn &&   !digitalRead(btn)) //read btn after signal or while backlight is active
	{
		LED_on();
		signal_off();
		off_t=millis()+on_d;
		signal_t=millis()+on_d+off_d;
	}
	
	if((off_t>-1 && millis()>off_t))// || off_t-millis()> off_d
	{
		#ifdef irLED
		pinMode (irLED, INPUT);
		#endif
		signal_t=millis()+off_d;
		LED_off();
		signal_off();
		off_t=-1;
	}
	
}
void signal_on()
{
	bSignal=true;
	
	// for( byte i = LEDs_BG_E; i < NUM_LEDS_now; i++) {
		// leds[i]=millis()/300%2?CRGB(255,255,122):CRGB::Black;
	// }

	for( byte i = LEDs_BG_E; i < NUM_LEDS_now; i++) {
		leds[i]=CRGB::Black;
	}
	int v=millis()%800;
	//for( byte i = constrain(map(v,0,800, NUM_LEDS_now, LEDs_BG_E),LEDs_BG_E, NUM_LEDS_now); i < NUM_LEDS_now; i++) { //reverse
	for( byte i = LEDs_BG_E; i < constrain(map(v,0,800, LEDs_BG_E, NUM_LEDS_now),LEDs_BG_E, NUM_LEDS_now); i++) {
		leds[i]=CRGB(255,0+millis()%800/6,millis()%800/12);
		// Serial.print(i); ttab
		// Serial.print(v); ttab
		// Serial.print(constrain(map(v,0,800, LEDs_BG_E, NUM_LEDS_now),LEDs_BG_E, NUM_LEDS_now)); ttab
		// Serial.println();
		//leds[i]=CRGB(255,0+millis()%800/8,millis()%800/16);
	}
	//Serial.println();Serial.println();
	FastLED.show();
}
void signal_off()
{
	bSignal=false;
	
	for( byte i = LEDs_BG_E; i < NUM_LEDS_now; i++) {
		leds[i]=CRGB::Black;
	}
	FastLED.show();
}
void LED_on()
{
	bBG_light=true;
	for( byte i = 0; i < LEDs_BG_E; i++) {
		//leds[i]=CRGB::White;
		uint16_t c=map(off_t-millis(), off_d,0, 1000,0);
		if(c>255)c=255;
		//else anim fade off
		leds[i]=CRGB(c,c,c);
	}
	FastLED.show();
}
void LED_off()
{
	bBG_light=false;
	for( byte i = 0; i < LEDs_BG_E; i++) {
		leds[i]=CRGB::Black;
	}
	FastLED.show();
}