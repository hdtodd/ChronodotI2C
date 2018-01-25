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

#include "Arduino.h"
#include "ChronodotI2C.h"
#include <I2C.h>

////////////////////////////////////////////////////////////////////////////////
// utility code, some of this could be exposed in the DateTime API if needed

const uint8_t daysInMonth [] PROGMEM = { 31,28,31,30,31,30,31,31,30,31,30,31 };

// convert a two-digit char string to an unsigned integer; no error control
uint8_t xconv2d(char* p) {
  return 10*(uint8_t)( (*p>='0')&&(*p<='9') ? (*p-'0') : 0) + (uint8_t)(( (*(p+1)>='0')&&(*(p+1)<='9')) ? (*(p+1)-'0') : 0);
  };

// number of days since 2000/01/01, valid for 2001..2099
static uint16_t date2days(uint16_t y, uint8_t m, uint8_t d) {
    if (y >= 2000)
        y -= 2000;
    uint16_t days = d;
    for (uint8_t i = 1; i < m; ++i)
        days += pgm_read_byte(daysInMonth + i - 1);
    if (m > 2 && y % 4 == 0)
        ++days;
    return days + 365 * y + (y + 3) / 4 - 1;
}

static long time2long(uint16_t days, uint8_t h, uint8_t m, uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

////////////////////////////////////////////////////////////////////////////////
// DateTime implementation - ignores time zones and DST changes
// NOTE: also ignores leap seconds, see http://en.wikipedia.org/wiki/Leap_second

DateTime::DateTime (uint32_t t) {
  t -= SECONDS_FROM_1970_TO_2000;    // bring to 2000 timestamp from 1970

    ss = t % 60;
    t /= 60;
    mm = t % 60;
    t /= 60;
    hh = t % 24;
    uint16_t days = t / 24;
    uint8_t leap;
    for (yOff = 0; ; ++yOff) {
        leap = yOff % 4 == 0;
        if (days < uint16_t(365) + leap)
            break;
        days -= 365 + leap;
    }
    for (m = 1; ; ++m) {
        uint8_t daysPerMonth = pgm_read_byte(daysInMonth + m - 1);
        if (leap && m == 2)
            ++daysPerMonth;
        if (days < daysPerMonth)
            break;
        days -= daysPerMonth;
    }
    d = days + 1;
}

DateTime::DateTime (uint16_t year, uint8_t month, uint8_t day, uint8_t hour, uint8_t min, uint8_t sec, float tempF, float tempC) {
    if (year >= 2000)
        year -= 2000;
    yOff = year;
    m = month;
    d = day;
    hh = hour;
    mm = min;
    ss = sec;
    ttf = tempF;
    ttc = tempC;
}


// A convenient constructor for using "the compiler's time":
//   DateTime now (__DATE__, __TIME__);
// NOTE: using PSTR would further reduce the RAM footprint
DateTime::DateTime (char* date, char* time) {
    // sample input: date = "Dec 26 2009", time = "12:34:56"
    yOff = xconv2d(date + 9);
    // Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec 
    switch (date[0]) {
    case 'J': m = (date[1] == 'a') ? 1 : ( (date[2] == 'n') ? 6 : 7); break;
        case 'F': m = 2; break;
        case 'A': m = date[2] == 'r' ? 4 : 8; break;
        case 'M': m = date[2] == 'r' ? 3 : 5; break;
        case 'S': m = 9; break;
        case 'O': m = 10; break;
        case 'N': m = 11; break;
        case 'D': m = 12; break;
    }
    d = xconv2d(date + 4);
    hh = xconv2d(time);
    mm = xconv2d(time + 3);
    ss = xconv2d(time + 6);
    ttf = ttc = 0;
}

uint8_t DateTime::dayOfWeek() const {    
    uint16_t day = date2days(yOff, m, d);
    return (day + 6) % 7; // Jan 1, 2000 is a Saturday, i.e. returns 6
}

long DateTime::secondstime(void) const {
  long t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  return t;
}

uint32_t DateTime::unixtime(void) const {
  uint32_t t;
  uint16_t days = date2days(yOff, m, d);
  t = time2long(days, hh, mm, ss);
  t += SECONDS_FROM_1970_TO_2000;  // seconds from 1970 to 2000
  return t;
}

////////////////////////////////////////////////////////////////////////////////
// I2C communication routines
//
// Routine to write control reg, with error detection
// Parameters:
//    reg     register to be written
//    value   value to write
//    caller  char string identifying calling routine for error msgs
// Returns:
//    eC      0 if no error; otherwise Atmel I2C error code
uint8_t writeCtl(uint8_t reg, uint8_t value, char *caller) {
  uint8_t eC;

  eC = I2c.write((uint8_t) CHRONODOT, (uint8_t) reg, (uint8_t) value);
  if (eC != 0) {
    Serial.print("I2C communication write error in "); Serial.print(caller); 
    Serial.print("; Atmel I2C error code = 0x"); Serial.println(eC, HEX);
  }
  return(eC);
}

// Routine to read control reg, with error detection
// Parameters:
//    reg     register to be written
//    value   address to store value read
//    caller  char string identifying calling routine for error msgs
// Returns:
//    eC      0 if no error; otherwise Atmel I2C error code
uint8_t readStatus(uint8_t reg, uint8_t *datain, char *caller) {
  uint8_t eC;

  eC = I2c.read(CHRONODOT, reg, 1, datain);
  if ( eC != 0 ) {
    Serial.print("[%Chronodot] I2C communication read error in "); Serial.print(caller); 
    Serial.print("; I2C error code = 0x"); Serial.println(eC, HEX);    
  }
  return(eC);
}

////////////////////////////////////////////////////////////////////////////////

// RTC_DS3231 implementation

static uint8_t bcd2bin (uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t bin2bcd (uint8_t val) { return val + 6 * (val / 10); }

uint8_t Chronodot::begin(void) {
  return 1;
}

//[hdt] reversed meaning of boolean: isrunning TRUE ==> is running
// returns the Oscillator Stop Flag (OSF) bit of the status reg
// to confirm that the clock is running (1 ==> running)
uint8_t Chronodot::isrunning(void) {
  uint8_t ss=0;

  if ( readStatus(CD_STATUS, &ss, (char *) "CD::isrunning()") == 0 ) 
    return 1;
  else
    return 0;
}

// send new date & time to chronodot
void Chronodot::adjust(const DateTime& dt) {
  uint8_t ss;

  // do this the hard way, byte by byte, for the first try
  //   write each of the first six CD registers individually

  writeCtl(CD_SEC, bin2bcd(dt.second()), (char *) "CD::adjust");
  writeCtl(CD_MIN, bin2bcd(dt.minute()), (char *) "CD::adjust");
  writeCtl(CD_HRS, bin2bcd(dt.hour()), (char *) "CD::adjust");
  writeCtl(CD_DAY, bin2bcd(0), (char *) "CD::adjust");
  writeCtl(CD_DAT, bin2bcd(dt.day()), (char *) "CD::adjust");
  writeCtl(CD_MON, bin2bcd(dt.month()), (char *) "CD::adjust");
  writeCtl(CD_YR,  bin2bcd(dt.year() - 2000), (char *) "CD::adjust");

// now get the control byte - we need to set bit 7 to zero
  readStatus(CD_STATUS, &ss, (char *) "CD:adjust()");

  ss &= ~(1 << 7);				// clear OSF bit
  writeCtl(CD_STATUS, ss, (char *) "CD::adjust()");
}

//  Get current date/time from Chronodot
//  See data sheet for structure of regs, but essentially
//  the first 7 regs contain the time in BCD
//  and the last two contain the temp as integer in the MSB
//  and 1/4 degree in the left two bits of the LSB
DateTime Chronodot::now() {
  uint8_t tB[7];               // tB = time/temp buffer string
  uint8_t ss, mm, hh, d, m;
  uint16_t y;
  const float tempLow2[4] = { 0.0, 0.25, 0.50, 0.75 }; 

  // Get the time fields into the buffer for conversion
  I2c.read((int)CHRONODOT, (int)CD_SEC, (int)7, tB);
  ss = bcd2bin(tB[0] & 0x7F);
  mm = bcd2bin(tB[1]);
  hh = bcd2bin(tB[2]); 
  d =  bcd2bin(tB[4]);
  m =  bcd2bin(tB[5]);
  y =  bcd2bin(tB[6]);

  //  Now get the temp and convert it
  I2c.read((int)CHRONODOT, (int)MSB_TEMP, (int)2, tB);
  float ttc = (float)(int)tB[0];
  ttc += tempLow2[tB[1]>>6];
  float ttf  = (((ttc * 9.0) / 5.0) + 32.0);

  // Return the values as fields in the structure
  return DateTime (y, m, d, hh, mm, ss, ttf, ttc);
}

////////////////////////////////////////////////////////////////////////////////
// RTC_Millis implementation

long RTC_Millis::offset = 0;

void RTC_Millis::adjust(const DateTime& dt) {
    offset = dt.unixtime() - millis() / 1000;
}

DateTime RTC_Millis::now() {
  return (uint32_t)(offset + millis() / 1000);
}

////////////////////////////////////////////////////////////////////////////////
