## JWSNano
Prayer Times Schedule using Arduino Nano, LED Panel P10, RTC DS1302, and BUZZER

### Reference
- JWS - Jadwal Waktu Sholat : https://github.com/busel7/Arduino

### Feature
- Big Times Display
- 5 Prayer Times Schedule and Tanbih Imsak
- Alarm for Adzan and Tanbih Imsak
- Date
- Iqomah Countdown
- Small Clock on the top line and Update scroll the text on the bottom line


### Pin on DMD LED P10 Panel

| DMD P10 | Nano  | 
| ------- | ----- |
| A       | D6    |                                                 
| B       | D7    |                                                  
| CLK     | D13   |                           
| SCK     | D8    |                           
| R       | D11   |
| NOE     | D9    |
| GND     | GND   |

### Pin on RTC DS1302

| DS3231 | Nano |
| ------ | ---- |
| SCL    | A5   |
| SDA    | A4   |
| VCC    | 3V   |
| GND    | GND  |

### Pin on Buzzer

| Buzzer | Nano  |
| ------ | ----- |
| +      | D2    |
| -      | GND   |


### Eksternal Library
- DMD : https://github.com/freetronics/DMD
- PrayerTime : https://github.com/asmaklad/Arduino-Prayer-Times
- RTC DS1302 : https://github.com/Makuna/Rtc

