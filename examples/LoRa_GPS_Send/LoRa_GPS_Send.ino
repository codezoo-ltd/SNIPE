/*
   0. https://github.com/mikalhart/TinyGPS
   1. Please Click Clone or download button
   2. Download ZIP
   3. Install "TinyGPS-master.zip" into the the Arduino IDE
   4. https://www.geoplaner.com/ Please Check the latitude and longitude values from Arduino. 
*/
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "SNIPE.h"
#define TXpin 12
#define RXpin 11
#define GPS_TX 9
#define GPS_RX 8

#define ATSerial Serial

String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  //16byte hex key

SoftwareSerial DebugSerial(RXpin, TXpin);
SNIPE SNIPE(ATSerial);
TinyGPS gps;
SoftwareSerial ss(GPS_RX, GPS_TX);

void setup() {
  ATSerial.begin(115200);

  // put your setup code here, to run once:
  while (ATSerial.read() >= 0) {}
  while (!ATSerial);

  DebugSerial.begin(115200);
  ss.begin(9600);

  if (!SNIPE.lora_init()) {
    DebugSerial.println("SNIPE LoRa Initialization Fail!");
    while (1);
  }

}

void loop()
{
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;

  // For one second we parse GPS data and report some key values
  for (unsigned long start = millis(); millis() - start < 1000;)
  {
    while (ss.available())
    {
      char c = ss.read();
      // Serial.write(c); // uncomment this line if you want to see the GPS data flowing
      if (gps.encode(c)) // Did a new valid sentence come in?
        newData = true;
    }
  }

  if (newData)
  {
    float flat, flon;
    unsigned long age;
    char gps_buf[32];
    char gps_lat[12];
    char gps_lon[12];
    String gps_value;
    memset(gps_buf, 0x0, sizeof(gps_buf));

    gps.f_get_position(&flat, &flon, &age);
    DebugSerial.print("LAT=");
    DebugSerial.print(flat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flat, 6);
    DebugSerial.print(" LON=");
    DebugSerial.print(flon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : flon, 6);
    DebugSerial.print(" SAT=");
    DebugSerial.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
    DebugSerial.print(" PREC=");
    DebugSerial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
 
    if (flat != TinyGPS::GPS_INVALID_F_ANGLE &&
      flon != TinyGPS::GPS_INVALID_F_ANGLE){
        dtostrf(flat, 9, 6, gps_lat);
        dtostrf(flon, 10, 6, gps_lon);
        sprintf(gps_buf, "%s:%s", gps_lat, gps_lon);
        gps_value = (String)gps_buf;
        
        DebugSerial.println("-----gps_send_value-----");
        DebugSerial.println(gps_value);

        //LoRa Send : GPS value
        if (SNIPE.lora_send(gps_value))
        {
          DebugSerial.println("send success");
        }
        else
        {
          DebugSerial.println("send fail");
          delay(500);
        }
    } 
  }

  gps.stats(&chars, &sentences, &failed);
  DebugSerial.print(" CHARS=");
  DebugSerial.print(chars);
  DebugSerial.print(" SENTENCES=");
  DebugSerial.print(sentences);
  DebugSerial.print(" CSUM ERR=");
  DebugSerial.println(failed);
  if (chars == 0)
    DebugSerial.println("** No characters received from GPS: check wiring **");
}
