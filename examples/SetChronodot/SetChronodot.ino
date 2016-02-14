/*
 * NAME: SetChronodot
 * DATE: 2016-02-14
 *
 * PURPOSE:
 * Set the time on a Chronodot RTC hardware device based on the
 * compile time of this program.  Uses the I2C interface to the Uno
 * to communicate with the Chronodot.
 *
 * If Chronodot hardware isn't present, simulate using the millis() 
 * clock built into the Uno.
 *
 * HDTodd, 2016\02\14 to adapt to ChronodotI2C
 *
 * This code is in the public domain.
 *
 */

#include <I2C.h>
#include "ChronodotI2C.h"

boolean    haveRTC;
Chronodot  RTC;         // Hardware Chronodot real-time clock
RTC_Millis Timer;       // Clock simulated from millisec timer + compile time of program

/*
 * SETUP ---------------------------------------------------------------------------------
 */
void setup()
{
  Serial.begin(9600);
  Serial.println("Set Chronodot time from compile time");
  Serial.print("Starting Date="); Serial.print(__DATE__); Serial.print("\tTime="); Serial.println(__TIME__);

  I2c.begin();
  I2c.setSpeed(1);    // set I2C to FAST

  RTC.begin();
  if ( RTC.isrunning() ) {
    haveRTC=true;
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust( DateTime(__DATE__, __TIME__) );    // set the time on the Chronodot
  }
  else {
    haveRTC=false;
    Serial.println("RTC is NOT running!  Using Uno millsec timer.");
    Timer.adjust(DateTime(__DATE__, __TIME__));    // set the time in the simulated clock
    Serial.print("Starting Date="); Serial.print(__DATE__); Serial.print("\tTime="); Serial.println(__TIME__);
  }

  Serial.println("-----------------------------------------------\n");
}

/*
 * LOOP  ---------------------------------------------------------------------
 */
void loop()
{
    DateTime now;
    if ( haveRTC ) 
      now = RTC.now();			// use hardware
    else
      now = Timer.now();		// use Uno millis() timer
    
    Serial.print(now.year(), DEC); Serial.print('\\');
    if (now.month() < 10) Serial.print("0");Serial.print(now.month(), DEC); Serial.print('\\');
    if (now.day() < 10) Serial.print("0"); Serial.print(now.day(), DEC); Serial.print(' ');
    if (now.hour() < 10) Serial.print("0"); Serial.print(now.hour(), DEC); Serial.print(':');
    if (now.minute() < 10) Serial.print("0"); Serial.print(now.minute(), DEC); Serial.print(':');
    if (now.second() < 10) Serial.print("0"); Serial.print(now.second(), DEC); Serial.print("\t");

    Serial.print(now.tempC(), 2); Serial.print(" C = "); 
    Serial.print(now.tempF(), 1); Serial.println(" F");
    
    delay(5000);
}

