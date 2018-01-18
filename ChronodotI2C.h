// Code by JeeLabs http://news.jeelabs.org/code/
// Released to the public domain! Enjoy!
//
// Modified for Chronodot / DS3132 with 
//   temperature data by Stephanie Maks
//   http://planetstephanie.net/
//
// By HDTodd@gmail.com
// August, 2015
//   Revised to use I2C library rather than <Wire.h>
//   Removed support for Arduino < 100
//   Added register definition defines
// January, 2018
//   Added xconv2d 


#ifndef CHRONODOT_H
#define CHRONODOT_H

#define CHRONODOT  0x68
#define SECONDS_PER_DAY 86400L
#define SECONDS_FROM_1970_TO_2000 946684800

#define CD_SEC    0x00
#define CD_MIN    0x01
#define CD_HRS    0x02
#define CD_DAY    0x03
#define CD_DAT    0x04
#define CD_MON    0x05
#define CD_YR     0x06
#define AL1_SEC   0x07
#define AL1_MIN   0x08
#define AL1_HR    0x09
#define AL1_DYDT  0x0A
#define AL2_MIN   0x0B
#define AL2_HR    0x0C
#define AL2_DYDT  0x0D
#define CD_CTRL   0x0E
#define CD_STATUS 0x0F
#define AGE_OFF   0x10
#define MSB_TEMP  0x11
#define LSB_TEMP  0x12

uint8_t xconv2d(char *p);

// Simple general-purpose date/time class (no TZ / DST / leap second handling!)
class DateTime {
public:
    DateTime (uint32_t t =0);
    DateTime (uint16_t year, uint8_t month, uint8_t day,
                uint8_t hour =0, uint8_t min =0, uint8_t sec =0, float tempF = 0.0, float tempC = 0.0);
    DateTime (char* date, char* time);
    uint16_t year() const       { return 2000 + yOff; }
    uint8_t month() const       { return m; }
    uint8_t day() const         { return d; }
    uint8_t hour() const        { return hh; }
    uint8_t minute() const      { return mm; }
    uint8_t second() const      { return ss; }
    float tempF() const		{ return ttf; }
    float tempC() const		{ return ttc; }
    uint8_t dayOfWeek() const;

    // 32-bit times as seconds since 1/1/2000
    long secondstime() const;   
    // 32-bit times as seconds since 1/1/1970
    uint32_t unixtime(void) const;

protected:
    uint8_t yOff, m, d, hh, mm, ss;
    float ttf;
    float ttc;
};

// Chronodot based on the DS3231 chip connected via I2C and the I2C Master Library
class Chronodot {
public:
  static uint8_t begin(void);
    static void adjust(const DateTime& dt);
    uint8_t isrunning(void);
    static DateTime now();
};

// RTC using the internal millis() clock, has to be initialized before use
// NOTE: this clock won't be correct once the millis() timer rolls over (>49d?)
class RTC_Millis {
public:
    static void begin(const DateTime& dt) { adjust(dt); }
    static void adjust(const DateTime& dt);
    static DateTime now();

protected:
    static long offset;
};

#endif
