// Date, time and temperature functions using 
//   a Chronodot RTC connected via I2C and 
//   accessed using Truchsess's I2C library
// [hdt, 2016\02\05] Updated to handle no-hardware situation correctly
// Adapted from original Adafruit demo with
//   code from Jeelabs and Stephanie Maks
// This adaptation by hdtodd, August, 2015
// Thanks to Wayne Truchsess for the I2C library!

#include <I2C.h>
#include "ChronodotI2C.h"

boolean haveRTC;

Chronodot RTC;          // Hardware Chronodot real-time clock
RTC_Millis Timer;       // Clock simulated from millisec timer + compile-time of program

void setup () {
  Serial.begin(9600);

  Serial.println("Initializing Chronodot using I2C Master Library.");
  I2c.begin();
  I2c.setSpeed(1);    // set I2C to FAST for testing

  RTC.begin();
  if ( RTC.isrunning() ) {
    haveRTC=true;
  }
  else {
    haveRTC=false;
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    Timer.adjust(DateTime(__DATE__, __TIME__));
    Serial.print("Starting Date="); Serial.print(__DATE__); Serial.print("\tTime="); Serial.println(__TIME__);
  }
}

void loop () {
    DateTime now;
    if ( haveRTC ) 
      now = RTC.now();			// use hardware
    else
      now = Timer.now();		// use Uno millis() timer
    
    Serial.print(now.year(), DEC); Serial.print('\\');
    if(now.month() < 10) Serial.print("0");Serial.print(now.month(), DEC); Serial.print('\\');
    if(now.day() < 10) Serial.print("0"); Serial.print(now.day(), DEC); Serial.print(' ');
    if(now.hour() < 10) Serial.print("0"); Serial.print(now.hour(), DEC); Serial.print(':');
    if(now.minute() < 10) Serial.print("0"); Serial.print(now.minute(), DEC); Serial.print(':');
    if(now.second() < 10) Serial.print("0"); Serial.print(now.second(), DEC); Serial.print("\t");

    Serial.print(now.tempC(), 2); Serial.print(" C = "); 
    Serial.print(now.tempF(), 1); Serial.println(" F");
    
    delay(5000);
}
