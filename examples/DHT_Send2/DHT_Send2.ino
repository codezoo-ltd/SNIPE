#include <SoftwareSerial.h>
#include "SNIPE.h"

#define ADDR  10 //(0x00 ~ 0xFF)
#define TXpin 11
#define RXpin 10
#define ATSerial Serial

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

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
    
  DebugSerial.println("SNIPE LoRa PingPong Test");
}

void loop() {

  char addr[2];
  String msg;

  sprintf(addr, "%d", ADDR);
  msg = (String)addr;
  
  if (SNIPE.lora_send(msg))
  {
    DebugSerial.println("send success");
    
    String ver = SNIPE.lora_recv();

    if (ver != "AT_RX_TIMEOUT" && ver != "AT_RX_ERROR")
    {
      DebugSerial.println(ver);
      DebugSerial.println(SNIPE.lora_getRssi());
      DebugSerial.println(SNIPE.lora_getSnr());            
    }
    else
    {
      DebugSerial.println("recv fail");
      delay(500);
    }
  }
 delay(1000);

}
