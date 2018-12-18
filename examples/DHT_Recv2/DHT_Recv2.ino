// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#include "DHT.h"
#include <SoftwareSerial.h>
#include "SNIPE.h"

#define DHTPIN 2     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

#define ADDR  10 //(0x00 ~ 0xFF)
#define TXpin 11
#define RXpin 10
#define ATSerial Serial

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
// NOTE: For working with a faster chip, like an Arduino Due or Teensy, you
// might need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// Example to initialize DHT sensor for Arduino Due:
//DHT dht(DHTPIN, DHTTYPE, 30);

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);
char buffer[20];
int addr;

void setup() {
  ATSerial.begin(115200);

  // put your setup code here, to run once:
  while(ATSerial.read()>= 0) {}
  while(!ATSerial);

  DebugSerial.begin(115200);

  /* SNIPE LoRa Initialization */
  if (!SNIPE.lora_init()) {
    DebugSerial.println("SNIPE LoRa Initialization Fail!");
    while (1);
  }

  /* SNIPE LoRa Set AppKey */
  if (!SNIPE.lora_setAppKey(lora_app_key)) {
    DebugSerial.println("SNIPE LoRa app key value has not been changed");
  }  

  /* SNIPE LoRa Set Frequency */
  if (!SNIPE.lora_setFreq(LORA_CH_1)) {
    DebugSerial.println("SNIPE LoRa Frequency value has not been changed");
  }

  /* SNIPE LoRa Set Spreading Factor */
  if (!SNIPE.lora_setSf(LORA_SF_7)) {
    DebugSerial.println("SNIPE LoRa Sf value has not been changed");
  }

  /* SNIPE LoRa Set Rx Timeout 
   * If you select LORA_SF_12, 
   * RX Timout use a value greater than 5000  
  */
  if (!SNIPE.lora_setRxtout(5000)) {
    DebugSerial.println("SNIPE LoRa Rx Timout value has not been changed");
  }   
    
  DebugSerial.println("SNIPE LoRa DHT22 Recv");
 
  dht.begin();
}

void loop() {
  char temp[8];
  char humi[8];
  String ver = SNIPE.lora_recv();

  if (ver != "AT_RX_TIMEOUT" && ver != "AT_RX_ERROR")
  {
    DebugSerial.println(ver);
    memset(buffer, 0x0, sizeof(buffer));
    ver.toCharArray(buffer, sizeof(buffer));
    sscanf(buffer, "%d", &addr);

    if(addr == ADDR){
      String send_value;
      DebugSerial.println(SNIPE.lora_getRssi());
      DebugSerial.println(SNIPE.lora_getSnr());
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit
      float f = dht.readTemperature(true);
    
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
        DebugSerial.println("Failed to read from DHT sensor!");
        return;
      }
  
      // Compute heat index
      // Must send in temp in Fahrenheit!
      float hi = dht.computeHeatIndex(f, h);

      memset(buffer, 0x0, sizeof(buffer));
      dtostrf(t, 5, 2, temp);
      dtostrf(h, 5, 2, humi);

      sprintf(buffer, "%d:%s:%s", addr, temp, humi);
      send_value = String(buffer);

      DebugSerial.println("-----LoRa_send_value-----");
      DebugSerial.println(send_value);
  
      DebugSerial.print("Humidity: "); 
      DebugSerial.print(h);
      DebugSerial.print(" %\t");
      DebugSerial.print("Temperature: "); 
      DebugSerial.print(t);
      DebugSerial.print(" *C ");
  
      if(SNIPE.lora_send(send_value))
      {
        DebugSerial.println("send success");
      }
      else
      {
        DebugSerial.println("send fail");
        delay(500);
      }

    }
    else
      DebugSerial.println("It is not my address!!!");
  }
  delay(1000);
}
