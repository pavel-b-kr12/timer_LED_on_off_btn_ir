# timer_LED_on_off_btn_ir
timer arduino based (low memory atmega168 compatible)
* on , off intervals, 
* signal to allow press button
* LED indicators (WS2812b) with animation
* jumpers to do fast settings without flashing


## Connection

#define btn				A0

#define  irLED			5 //optional if use IR Infrared Receiver (TL1838, TSOP...) instead of button

//optional switches (jumpers)

#define sw_10sec_on		A1

#define sw_20sec_on		A2

#define sw_60sec_off	A3

#define sw_180sec_off	A4

#define brightness33	A5

## TODO
* photo, video
