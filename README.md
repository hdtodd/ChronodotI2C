# ChronodotI2C

#### v2.0, 2017-July-05

## Arduino Chronodot V2.0 RTC library accessed via the I2C library rather than the Wire library

## Summary

This is a library of functions to access an I2C-connected Chronodot real-time clock using Truchsess's I2C library (<http://dsscircuits.com/articles/86-articles/66-arduino-i2c-master-library>) rather than the Wire library.  

The code here is largely a straightforward adaptation of code written by JeeLabs and modified by Stephanie Maks.  Maks added to the original RTC the ability to read out the temperature as sensed by the Chronodot's internal temperature sensor.  The Adafruit RTClib (courtesy of Adafruit, <https://github.com/adafruit/RTClib>) is a fork of the JeeLabs code, too. 

This adaptation uses the I2C library rather than the Wire library but otherwise provides the same functionality as the extended RTC library of Maks/JeeLabs <https://github.com/Stephanie-Maks/Arduino-Chronodot>.

There appears to be no usage restriction (JeeLabs explicitly the code released to the public domain). 
 
## Motivation

While working on a package of Arduino routines that use both the MPL3115A2 altitude/barometer/temp sensor and the Chronodot RTC, I found that Wire library distributed with the Arduino IDE package (`<Wire.h>`) that is most commonly used for accessing sensors via I2C became unreliable over time. The problem manifested as sporadic incorrect readings of pressure or temperature, eventually failing to make any correct readings.  I initially attributed the problem to the MPL3115A2 itself, but researching, I found that other Freescale devices (as the MPL3115A2 is) were similarly unreliable over time because of I2C communication failures.  This is reportedly due to the use of repeated-starts by those devices, which Wire library apparently doesn't handle well.

I experimented with Wayne Truchsess's I2C library, which others had reported to have solved the problem.  I found that communication with the MPL3115A2 was stable over long periods of time with that library. Since my project involves several I2C devices, and I didn't want to include both the Wire library and the I2C library in my project, I adapted both the MPL3115A2 code and the Chronodot code to use I2C.  The MPL3115A2 code is also available on GitHub (<https://github.com/hdtodd/MPL3115A2>)

Original version (apparently) by JeeLabs, subsequently modified by Stephanie Maks. Revisions for Truchsess's I2C library by:

```
David Todd, Bozeman, MT: <hdtodd@gmail.com>  
August 3, 2015  
Revised February 9, 2016  
Re-documented July 5, 2017
```


## Changelog

* Feb 2016 [hdt] 
  * Revised the "isrunning" logic to mean what it says: "isrunning" true ==> hardware is present & running
  * Revised testing routine to use it correctly and invoke the pseudo clock "RTC_Millis" if hardware absent
  * Added version # to README and started with 2.0
* August 2015 [hdt] 
  * Converted to use Truchsess's I2C library rather than Wire library in order to be compatible with other devices that need repeated-starts, for which Wire fails.
  * Adaptation by hdtodd from work by JeeLabs and Stephanie Maks
* May 2012:    Added preprocessor directives in Chronodot.h which allows the file to be included multiple times
* February 2012:  
  * Fixed bug in isRunning function - was checking the wrong register!
  * Fixed variable type error - was using unsigned int for temperature, should have been int, to accomodate negative numbers.
* December 2011:   Updated for Arduino 1.0 compatibility

