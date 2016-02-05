Arduino Chronodot RTC library and example using I2C library rather than Wire library

While working on a package of Arduino routines that use both the MPL3115A2 altitude/barometer/temp sensor and the Chronodot RTC, I found that Wire library that is most commonly used for accessing sensors via I2C was unreliable over time. The problem manifested as sporadic incorrect reading of pressure or temperature, eventually failing to make any correct readings. I initially attributed the problem to the MPL3115A2 itself, but researching, I found that other Freescale devices (as the MPL3115A2 is) were similarly unreliable over time because of I2C communication failures. This is reportedly due to the use of repeated-starts by those devices, which Wire library apparently doesn't handle.

I experimented with Wayne Truchsess's I2C library, which others had reported to have solved the problem, and I found that communicating with the MPL3115A2 was stable over long periods of time with that library. Since my project involves several I2C devices, and I didn't want to include both the Wire library and the I2C library in my project, I adapted both the MPL3115A2 code and the Chronodot code to use I2C. (I'll post the MPL3115A2 code when I've finished cleaning it up ... it's a bit more complicated).

The code here is largely a straightforward adaptation of code written by JeeLabs and modified by Stephanie Maks. There appears to be no usage restriction (JeeLabs explicitly released to the public domain). 

HDTodd,
Bozeman, MT
August 3, 2015
