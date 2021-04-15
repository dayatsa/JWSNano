

#include <DMD3.h>
#include <TimerOne.h>
#include <Wire.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include "PrayerTimes.h"
#include <SystemFont5x7.h>
#include <angka6x13.h>
#include <angka_2.h>
#include <huruf_kecilku.h>
#include <Font3x5.h>
#include <Arial_Black_16.h>
#include "ElektronMart6x8.h"
#include "ElektronMart6x16.h"
#include "ElektronMart5x6.h"
#include "ElektronMartArabic6x16.h"
#include "ElektronMartArabic5x6.h"
#include <SPI.h>
#include <EEPROM.h>

char weekDay[][7] = {" AHAD ", "SENIN ", "SELASA", " RABU ", "KAMIS ", "JUM'AT", "SABTU", " AHAD "}; // array hari, dihitung mulai dari Ahad, hari Ahad angka nya =0.
char namaBulan[][13] = {" ", "JAN", "FEB", "MAR", "APR", "MEI", "JUN", "JUL", "AGU", "SEP", "OKT", "NOV", "DES"}; // Urutan nama bulan dihitung dari 0 sampai 12
int langkah;
int lama_tampilnya;
boolean Waduh_Setting = false;
//
int address = 0;
byte value_ihti = 2, value_iqmh_subuh = 10, value_iqmh_dzuhur = 8, value_iqmh_ashar = 7, value_iqmh_maghrib = 7, value_iqmh_isya = 7, value_hari, m_iqmh;
int addr = 0;
byte S_IQMH = 0, S_IHTI = 0, S_HARI = 0;

//BUZZER PIN
const int buzzer = A0; // Pin GPIO Buzzer - D2

///////////////////////////////// Mendifinisikan Pin Tombol ////////////////////////////////////////
#define Jam_bUP A1                  // Tombol setting sesuaikan dengan PIN di arduino anda mana  klo ini terhubung dengan PIN 3 Digital
#define tombol_bOK A2             // Tombol OK sesuaikan dengan PIN di arduino anda mana  klo ini terhubung dengan PIN 5 Digital
#define Jam_bDN A3           //tombol setting   sesuaikan dengan PIN di arduino anda mana  klo ini terhubung dengan PIN 4 Digital
///////////////////koding ini untuk setting rtc sesuai dengan laptop//// /////


ThreeWire myWire(4, 5, 2); // IO, SCLK, CE
RtcDS1302<ThreeWire> rtc(myWire);
RtcDateTime now;

DMD3 dmd; //untuk mengatur jumlah panel yang kamu pakai
double times[sizeof(TimeName) / sizeof(char*)];
void ScanDMD()
{
  dmd.refresh();
}
//

bool ganti = true;


void setup()
{
  Serial.begin(9600);
  pinMode(buzzer, OUTPUT);//----pin 2 untuk alarm adzan maghrib ---hubungkan dengan buzzer
  langkah = 0;
  rtcInit();
  //rtc.setDateTime(dt); //Adjust date-time as defined 'dt' above (ini untuk setting jam)
  Wire.begin();
  Timer1.initialize( 2000 );
  Timer1.attachInterrupt( ScanDMD );
  Timer1.pwm(9, 50);
  dmd.clear();


  pinMode(tombol_bOK, INPUT_PULLUP);                                 // Mode Pin Sebagai Input dengan Pull Up Internal
  pinMode(Jam_bUP, INPUT_PULLUP);                                    // Mode Pin Sebagai Input dengan Pull Up Internal
  pinMode(Jam_bDN, INPUT_PULLUP);                                    // Mode Pin Sebagai Input dengan Pull Up Internal
  //
  attachInterrupt(0, Setting, FALLING);
  BuzzerPendek();
  branding();
  //  animLogoX();
}
////////////////////////////////// Fungsi Looping/Pengulangan ///////////////////////////////////////
void loop() {
  if (Waduh_Setting) {
    MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual
  }
  mulai();
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
}


void textCenter(int y, String Msg) {

  int center = int((dmd.width() - dmd.textWidth(Msg)) / 2);
  dmd.drawText(center, y, Msg);

}


void rtcInit() {
  Serial.print("compiled: ");
  Serial.print(__DATE__);
  Serial.println(__TIME__);

  rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();

  if (!rtc.IsDateTimeValid())
  {
    // Common Causes:
    //    1) first time you ran and the device wasn't running yet
    //    2) the battery on the device is low or even missing

    Serial.println("RTC lost confidence in the DateTime!");
    rtc.SetDateTime(compiled);
  }

  if (rtc.GetIsWriteProtected())
  {
    Serial.println("RTC was write protected, enabling writing now");
    rtc.SetIsWriteProtected(false);
  }

  if (!rtc.GetIsRunning())
  {
    Serial.println("RTC was not actively running, starting now");
    rtc.SetIsRunning(true);
  }

  RtcDateTime now = rtc.GetDateTime();
  if (now < compiled)
  {
    Serial.println("RTC is older than compile time!  (Updating DateTime)");
    rtc.SetDateTime(compiled);
  }
  else if (now > compiled)
  {
    Serial.println("RTC is newer than compile time. (this is expected)");
//        rtc.SetDateTime(compiled);
    //ganti
  }
  else if (now == compiled)
  {
    Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }
}

/////////////////////////////////// Interrupt Button Setting ////////////////////////////////////////
void Setting() {
  Waduh_Setting = true;                                                // Aktifkan Menu Setting
}
////////////////////////////////////// Fungsi Menu Setting //////////
void MenuSetting() {
}


//////////////////////==========================Program Jam==============

void mulai()
{
  //  m_iqmh = 1;    //Saat Subuh tambah 2 menit waktu Iqomah
  //    Iqomah();
  animLogoX();
  AlarmSholat();

  JamJatuhPulse();
  AlarmSholat();

  TampilTanggal();
  AlarmSholat();

  TampilJadwalSholat();
  AlarmSholat();

  RunningText();
  AlarmSholat();

  RtcDateTime st = rtc.GetDateTime();
  if (st.Day()%7 == 0 && st.Hour() == 1 && ganti) {
    st = RtcDateTime(st.Year(), st.Month(), st.Day(), st.Hour(), st.Minute() + 2, st.Second());
    rtc.SetDateTime(st);
    ganti = false;
  }
  if (ganti == false && st.Hour() == 3) ganti = true;
}


//----------------------------------------------------------------------
//TAMPILKAN JAM BESAR

void JamJatuhPulse() {
  dmd.clear();
  while (1) {
    AlarmSholat();

    static uint8_t y;
    static uint8_t d;
    static uint32_t pM;
    uint32_t cM = millis();

    static uint32_t pMPulse;
    static uint8_t pulse;


    if (cM - pMPulse >= 100) {
      pMPulse = cM;
      pulse++;
    }

    if (pulse > 8) {
      pulse = 0;
    }

    if (cM - pM > 25) {
      if (d == 0 and y < 32) {
        pM = cM;
        y++;
      }
      if (d  == 1 and y > 0) {
        pM = cM;
        y--;
      }
    }

    if (cM - pM > 30000 and y == 32) {
      d = 1;
    }

    if (y == 32) {
      dmd.drawRect(15, 3 + pulse, 18, 11 - pulse, 0, 1);
    }

    if (y < 32) {
      dmd.drawRect(15, 3, 18, 11, 0, 0);
    }

    if (y == 0 and d == 1) {
      d = 0;
      dmd.clear();
      //      tampilanjam = 2;
      break;
    }

    TampilJamDinamis(y - 32);
  }
}

void TampilJamDinamis(uint32_t y) {

  RtcDateTime now = rtc.GetDateTime();
  char jam[3];
  char menit[3];
  char detik[3];
  sprintf(jam, "%02d", now.Hour());
  sprintf(menit, "%02d", now.Minute());
  sprintf(detik, "%02d", now.Second());
  Serial.print(jam);
  Serial.print(menit);
  Serial.println(detik);
  delay(10);
  //JAM
  dmd.setFont(ElektronMart6x16);
  dmd.drawText(1, y, jam);

  //MENIT
  dmd.setFont(ElektronMart5x6);
  dmd.drawText(20, y, menit);

  //DETIK
  dmd.setFont(ElektronMart5x6);
  dmd.drawText(20, y + 8, detik);


}

void TampilJam() {
  while (1) {
    AlarmSholat();
    static uint8_t d;
    static uint32_t pM;
    static uint32_t pMJam;
    static uint32_t pMKedip;
    uint32_t cM = millis();
    static boolean kedip;

    RtcDateTime now = rtc.GetDateTime();
    char jam[3];
    char menit[3];
    char detik[3];

    if (cM - pMJam >= 1000) {

      pMJam = cM;
      d++;

      //JAM
      sprintf(jam, "%02d", now.Hour());
      dmd.setFont(ElektronMart6x16);
      dmd.drawText(3, 0, jam);

      //MENIT
      sprintf(menit, "%02d", now.Minute());
      dmd.setFont(ElektronMart5x6);
      dmd.drawText(22, 0, menit);

      //DETIK
      sprintf(detik, "%02d", now.Second());
      dmd.setFont(ElektronMart5x6);
      dmd.drawText(22, 8, detik);

      Serial.print(jam);
      Serial.print(menit);
      Serial.println(detik);

      if (d >= 10) {
        d = 0;
        dmd.clear();
        break;
      }

    }

    //KEDIP DETIK
    if (millis() - pMKedip >= 500) {
      pMKedip = millis();
      kedip = !kedip;
    }

    if (kedip) {
      dmd.drawRect(18, 3, 19, 5, 1, 1);
      dmd.drawRect(18, 9, 19, 11, 1, 1);
    } else {
      dmd.drawRect(18, 3, 19, 5, 0, 0);
      dmd.drawRect(18, 9, 19, 11, 0, 0);
    }
  }
}


//------------------------------------------------------
//RUNNING TEXT

void RunningText() {
  dmd.clear();
  RtcDateTime now = rtc.GetDateTime();

  detachInterrupt(0);// Matikan interrupt "0"
  ////   // Tampilkan hari
  dmd.setFont(ElektronMart5x6);
  String hariku = Konversi(now.Hour()) + ":" + Konversi(now.Minute()) ;
  int dowo = hariku.length() + 1;
  char detikanku[dowo];
  hariku.toCharArray(detikanku, dowo);
  //  dmd.drawString(2, 0, detikanku, dowo, 0);
  textCenter(0, detikanku);


  static const char message[] = "Mushola Al-Hidayah." ; ///tampilkan teks
  int width = dmd.width();
  dmd.setFont(SystemFont5x7);
  int msgWidth = dmd.textWidth(message);
  int fullScroll = msgWidth + width + 1;
  for (int x = 0; x < fullScroll; ++x) {
    //    dmd.clear();
    dmd.drawText(width - x, 8, message);
    delay(50);
  }

}


//----------------------------------------------------------------------
//JADWAL SHOLAT

void JadwalSholat() {

  RtcDateTime now = rtc.GetDateTime();

  int tahun = now.Year();
  int bulan = now.Month();
  int tanggal = now.Day();

  int dst = 7; // TimeZone

  set_calc_method(ISNA);
  set_asr_method(Shafii);
  set_high_lats_adjust_method(AngleBased);
  set_fajr_angle(20);
  set_isha_angle(18);

  //SETTING LOKASI DAN WAKTU Masjid Miftahul Jannah
  float latitude = -7.683590;
  float longitude = 110.495479;

  get_prayer_times(tahun, bulan, tanggal, latitude, longitude, dst, times);

}

void TampilJadwalSholat() {

  JadwalSholat();

  char sholat[7];
  char jam[5];
  char TimeName[][8] = {"SUBUH", "TERBIT", "DZUHUR", "ASHAR", "TERBENAM", "MAGHRIB", "ISYA", "WAKTU"};
  int hours, minutes;

  for (int i = 0; i < 8; i++) {

    get_float_time_parts(times[i], hours, minutes);

    value_ihti = 2; // value_ihti = EEPROM.read(0);
    minutes = minutes + value_ihti;

    if (minutes >= 60) {
      minutes = minutes - 60;
      hours ++;
    }
    else {
      ;
    }

    if (i == 0 || i == 2 || i == 3 || i == 5 || i == 6) { //Tampilkan hanya Subuh, Dzuhur, Ashar, Maghrib, Isya
      sprintf(sholat, "%s", TimeName[i]);
      sprintf(jam, "%02d:%02d", hours, minutes);
      dmd.clear();
      dmd.setFont(ElektronMart5x6);
      textCenter(0, sholat);
      dmd.setFont(ElektronMart6x8);
      textCenter(8, jam);
      Serial.println(sholat);
      Serial.println(" : ");
      Serial.println(jam);
      delay(2000);

      ///////////////interupsi tombol menu-----------
      if (digitalRead(tombol_bOK) == LOW) {
        MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual

      }

    }
  }

  //Tambahan Waktu Tanbih (Peringatan 10 menit sebelum mulai puasa) yang biasa disebut Imsak

  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + value_ihti;
  if (minutes < 11) {
    minutes = 60 - minutes;
    hours --;
  } else {
    minutes = minutes - 10 ;
  }
  sprintf(jam, "%02d:%02d", hours, minutes);
  dmd.clear();
  dmd.setFont(ElektronMart5x6);
  textCenter(0, "TANBIH");
  dmd.setFont(ElektronMart6x8);
  textCenter(8, jam);
  Serial.print("TANBIH");
  Serial.println(" : ");
  Serial.println(jam);
  delay(2000);

  ///////////////interupsi tombol menu-----------
  if (digitalRead(tombol_bOK) == LOW) {
    MenuSetting(); // Jika Tombol OK/Interrupt ditekan maka masuk menu setting Manual

  }

}


//--------------------------------------------------
//ALARM ADZAN WAKTU SHOLAT

void AlarmSholat() {

  RtcDateTime now = rtc.GetDateTime();

  int Hari = now.DayOfWeek();
  int Hor = now.Hour();
  int Min = now.Minute();
  int Sec = now.Second();

  JadwalSholat();

  int hours, minutes;

  // Tanbih atau Imsak
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes < 11) {
    minutes = 60 - minutes;
    hours --;
  } else {
    minutes = minutes - 10 ;
  }

  if (Hor == hours && Min == minutes) {
    dmd.clear();
    dmd.setFont(ElektronMart5x6);
    textCenter(0, "TANBIH");
    textCenter(8, "IMSAK");
    BuzzerPendek();
    Serial.println("TANBIH");
    delay(60000);
  }

  // Subuh
  get_float_time_parts(times[0], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes) {
    dmd.clear();
    dmd.setFont(ElektronMart5x6);
    textCenter(0, "ADZAN");
    textCenter(8, "SUBUH");
    BuzzerPanjang();
    Serial.println("SUBUH");
    delay(180000);//180 detik atau 3 menit untuk adzan

    BuzzerPendek();
    m_iqmh = 8;    //Saat Subuh tambah 2 menit waktu Iqomah
    Iqomah();
  }

  // Dzuhur
  get_float_time_parts(times[2], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes && Hari != 5) {

    dmd.clear();
    dmd.setFont(ElektronMart5x6);
    textCenter(0, "ADZAN");
    textCenter(8, "DZUHUR");
    BuzzerPanjang();
    Serial.println("DZUHUR");
    delay(180000);//180 detik atau 3 menit untuk adzan

    BuzzerPendek();
    m_iqmh = 8;
    Iqomah();

  } else if (Hor == hours && Min == minutes && Hari == 5) {

    dmd.clear();
    dmd.setFont(ElektronMart5x6);
    textCenter(0, "ADZAN");
    textCenter(8, "JUM'AT");
    BuzzerPanjang();
    Serial.println("Adzan Jum'at");
    delay(180000);//180 detik atau 3 menit untuk adzan
  }

  // Ashar
  get_float_time_parts(times[3], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes) {
    dmd.clear();
    dmd.setFont(ElektronMart5x6);
    textCenter(0, "ADZAN");
    textCenter(8, "ASHAR");
    BuzzerPanjang();
    Serial.println("ASHAR");
    delay(180000);//180 detik atau 3 menit untuk adzan

    BuzzerPendek();
    m_iqmh = 8;
    Iqomah();
  }

  // Maghrib
  get_float_time_parts(times[5], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes) {
    dmd.clear();
    dmd.setFont(ElektronMart5x6);
    textCenter(0, "ADZAN");
    textCenter(8, "MAGHRIB");
    BuzzerPanjang();
    Serial.println("MAGHRIB");
    delay(180000);//180 detik atau 3 menit untuk adzan

    BuzzerPendek();
    m_iqmh = 8;
    Iqomah();
  }

  // Isya'
  get_float_time_parts(times[6], hours, minutes);
  minutes = minutes + value_ihti;

  if (minutes >= 60) {
    minutes = minutes - 60;
    hours ++;
  }

  if (Hor == hours && Min == minutes) {
    dmd.clear();
    dmd.setFont(ElektronMart5x6);
    textCenter(0, "ADZAN");
    textCenter(8, "ISYA'");
    BuzzerPanjang();
    Serial.println("ISYA");
    delay(180000);//180 detik atau 3 menit untuk adzan

    BuzzerPendek();
    m_iqmh = 8;
    Iqomah();
  }

}


//--------------------------------------------------
//IQOMAH

void Iqomah() {

  RtcDateTime now = rtc.GetDateTime();
  //iqomah----------------------------------
  int langkah;
  int waktu_iqomah;
  int panjang_iqomah;
  int iqomah;
  static char hitungmundur[6];
  dmd.clear();
  dmd.setFont(ElektronMart5x6);
  textCenter(0, "IQOMAH");
  int tampil;
  int detik = 0, menit = m_iqmh;
  for (detik = 0; detik >= 0; detik--) {
    delay(1000);
    //    String iqomah = Konversi(menit) + ":" + Konversi(detik);
    //    int panjang_iqomah = iqomah.length() + 1;
    //    char waktu_iqomah[panjang_iqomah];
    //    iqomah.toCharArray(waktu_iqomah, panjang_iqomah);
    ////    dmd.setFont(SystemFont5x7);
    //    dmd.drawString(1, 9, waktu_iqomah, panjang_iqomah, 0);
    sprintf(hitungmundur, "%02d:%02d", menit, detik);
    dmd.setFont(ElektronMart5x6);
    textCenter(8, hitungmundur);
    if (detik <= 0) {
      detik = 60;
      menit = menit - 1;
    }
    if (menit <= 0 && detik <= 1) {
      dmd.clear();
      digitalWrite(buzzer, HIGH);//alarm Iqomah
      delay(2000);
      digitalWrite(buzzer, LOW);//alarm Iqomah
      delay(50);
      dmd.clear();
      textCenter(0, "LURUS 8");
      textCenter(9, "RAPAT");
      delay(10000);
      for (tampil = 0; tampil < 300 ; tampil++) { //< tampil selama 300 detik waktu saat sholat
        menit = 0;
        detik = 0;
        dmd.clear();
        dmd.setFont(ElektronMart5x6);
        textCenter(0, "SHOLAT");
        //        dmd.drawString(4, -2, "SHOLAT", 6, 0); //koordinat tampilan
        ///////////////////////
        now = rtc.GetDateTime();
        String xjam = Konversi(now.Hour()) + ":" + Konversi(now.Minute()) + ":" + Konversi(now.Second()) ; //tampilan jam
        int pjg = xjam.length() + 1;
        char sjam[pjg];
        xjam.toCharArray(sjam, pjg);
        dmd.setFont(Font3x5);
        textCenter(7, sjam);
        /////////////////////
        delay (1000);
      }
    }
  }
}



// Tampil Tanggal Baris Atas, Tampil Hari Baris Bawah

void TampilTanggal() {

  RtcDateTime now = rtc.GetDateTime();

  char tanggalan[5];

  sprintf(tanggalan, "%02d %s", now.Day(), namaBulan[now.Month()]);

  dmd.clear();
  dmd.setFont(ElektronMart5x6);
  textCenter(0, weekDay[now.DayOfWeek()]);
  textCenter(8, tanggalan);

  delay(3000);
}

void TampilSuhu() {
  dmd.clear();
  char suhu[2];
  int koreksisuhu = 2; // Perkiraan selisih suhu ruangan dan luar ruangan

  static uint8_t d;
  static uint32_t pM;
  uint32_t cM = millis();
  int celsius = 27;

  if (cM - pM > 2000) {
    pM = cM;
    d++;

    dmd.setFont(ElektronMart5x6);
    textCenter(0, "SUHU");
    sprintf(suhu, "%dC*", celsius - koreksisuhu);
    dmd.setFont(ElektronMart6x8);
    textCenter(8, suhu);

    if (d >= 2) {
      d = 0;
      dmd.clear();
    }
  }
}


//----------------------------------------------------------------------
// BUNYIKAN BEEP BUZZER

void BuzzerPanjang() {
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(1000);
  digitalWrite(buzzer, HIGH);
  delay(1000);
  digitalWrite(buzzer, LOW);
  delay(50);
}

void BuzzerPendek() {
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(100);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  delay(50);
}


//----------------------------------------------------------------------
///konversi angka agar ada nol didepannya jika diawah 10

String Konversi(int sInput) {
  if (sInput < 10)
  {
    return"0" + String(sInput);
  }
  else
  {
    return String(sInput);
  }
}


void animLogoX() {
  dmd.clear();
  while (1) {
    static uint8_t x;
    static uint8_t s; // 0=in, 1=out
    static uint32_t pM;
    uint32_t cM = millis();

    if ((cM - pM) > 35) {
      if (s == 0 and x < 16) {
        pM = cM;
        x++;
      }
      if (s == 1 and x > 0) {
        pM = cM;
        x--;
      }
    }

    if ((cM - pM) > 2000 and x == 16) {
      s = 1;
    }

    if (x == 0 and s == 1) {
      s = 0;
      break;
    }

    logoax(x - 16);
    logobx(32 - x);
  }
}


void logoax(uint32_t x) {
  static const uint8_t logoax[] PROGMEM = {
    16, 16,
    B00000000, B00000000,
    B01100110, B01111110,
    B01100110, B01111110,
    B01100110, B01100110,
    B01100110, B01100110,
    B01111110, B01111110,
    B01111110, B01111110,
    B01100000, B01100000,
    B01100000, B01100000,
    B01111110, B01111110,
    B01111110, B01111110,
    B01100110, B00000110,
    B01100110, B00000110,
    B01111111, B11111110,
    B01111111, B11111110,
    B00000000, B00000000
  };
  dmd.drawBitmap(x, 0, logoax);
}

void logobx(uint32_t x) {
  static const uint8_t logobx[] PROGMEM = {
    16, 16,
    B00000000, B00000000,
    B01111111, B11111110,
    B01111111, B11111110,
    B00000000, B00000000,
    B00000000, B00000000,
    B01111110, B01100110,
    B01111110, B01100110,
    B00000110, B01100110,
    B00000110, B01100110,
    B01111110, B01100110,
    B01111110, B01100110,
    B01100110, B01100110,
    B01100110, B01100110,
    B01111111, B11111110,
    B01111111, B11111110,
    B00000000, B00000000
  };
  dmd.drawBitmap(x, 0, logobx);
}

void branding() {

  dmd.clear();
  dmd.setFont(ElektronMart5x6);
  dmd.drawText(4, 0, "GROBAK");
  dmd.drawText(2, 7, ".NET");
  delay(1000);

  dmd.clear();
  dmd.setFont(Arial_Black_16);
  dmd.drawText(-2, 1, "JWS");
  delay(1000);

  dmd.clear();
  dmd.setFont(ElektronMart5x6);
  textCenter(3, "VER.3");
  delay(1000);
  dmd.clear();

}
