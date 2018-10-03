#include <SoftwareSerial.h>
#include "SNIPE.h" 
#define TXpin 9
#define RXpin 8
#define ATSerial Serial

String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  //16byte hex key

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

int red_Input;
int green_Input;
int blue_Input;
char buffer[16];

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

  pinMode(13, OUTPUT);  //Green
  pinMode(12, OUTPUT);  //Red
  pinMode(11, OUTPUT);  //Blue      
  DebugSerial.println("SNIPE LoRa StartUP");
} 

void loop() {
  String ver = SNIPE.lora_recv();

  if (ver != "AT_RX_TIMEOUT" && ver != "AT_RX_ERROR")
  {
    DebugSerial.println(ver);
    memset(buffer, 0x0, sizeof(buffer));
    ver.toCharArray(buffer, sizeof(buffer));
    sscanf(buffer, "%d:%d:%d", &red_Input,&green_Input,&blue_Input);
    analogWrite(12, red_Input);
    analogWrite(13, green_Input);
    analogWrite(11, blue_Input);    
        
    DebugSerial.println(SNIPE.lora_getRssi());
    DebugSerial.println(SNIPE.lora_getSnr());
  }
  else
  {
    DebugSerial.println(ver);
  }
  delay(500);
}
