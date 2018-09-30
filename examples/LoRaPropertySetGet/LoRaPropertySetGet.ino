#include <SoftwareSerial.h>
#include "SNIPE.h" 
#define TXpin 11
#define RXpin 10
#define ATSerial Serial

String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  //16byte hex key

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

void setup() {
  ATSerial.begin(115200);

  // put your setup code here, to run once:
  while(ATSerial.read()>= 0) {}
  while(!ATSerial);

  DebugSerial.begin(115200);

  if (!SNIPE.lora_init()) {
    DebugSerial.println("SNIPE LoRa Initialization Fail!");
    while (1);
  }  
    
  DebugSerial.println("SNIPE LoRa StartUP");
}

void loop() {
  DebugSerial.println("--- SNIPE LoRaPropertySetGet loop ---");

  DebugSerial.println(SNIPE.lora_getFreq());
  delay(500);

  DebugSerial.println(SNIPE.lora_setFreq(LORA_CH_2)==true?"TRUE":"FALSE");
  delay(500);

  DebugSerial.println(SNIPE.lora_getTxp());
  delay(500);

  DebugSerial.println(SNIPE.lora_setTxp(5)==true?"TRUE":"FALSE"); //Tx Power (dBm)
  delay(500);
   
  DebugSerial.println(SNIPE.lora_getSf());
  delay(500);

  DebugSerial.println(SNIPE.lora_setSf(LORA_SF_12)==true?"TRUE":"FALSE");
  delay(500);
  
  DebugSerial.println(SNIPE.lora_getRxtout());
  delay(500);

  DebugSerial.println(SNIPE.lora_setRxtout(1000)==true?"TRUE":"FALSE");
  delay(500);

  DebugSerial.println(SNIPE.lora_getTxtout());
  delay(500);

  DebugSerial.println(SNIPE.lora_setTxtout(1000)==true?"TRUE":"FALSE");
  delay(500);

  DebugSerial.println(SNIPE.lora_setAESMode(LORA_AES_OFF)==true?"TRUE":"FALSE");
  delay(500);
  
  DebugSerial.println(SNIPE.lora_getAppKey());
  delay(500);

  DebugSerial.println(SNIPE.lora_setAppKey(lora_app_key)==true?"TRUE":"FALSE");
  delay(500);  
      
  DebugSerial.println(SNIPE.lora_getVersion());
  delay(500);
}
