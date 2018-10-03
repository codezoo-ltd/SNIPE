#include <SoftwareSerial.h>
#include "SNIPE.h" 
#define TXpin 11
#define RXpin 10
#define ATSerial Serial

String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";  //16byte hex key

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

int red_Input;
int green_Input;
int blue_Input;

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
  red_Input = analogRead(A0);
  green_Input = analogRead(A1);
  blue_Input = analogRead(A2);

  red_Input = red_Input/4; // 가변저항의 입력값(0-1023사이의 값)을 RGB값(0-255)의 값으로 보정
  green_Input = green_Input/4; // 가변저항의 입력값(0-1023사이의 값)을 RGB값(0-255)의 값으로 보정
  blue_Input = blue_Input/4; // 가변저항의 입력값(0-1023사이의 값)을 RGB값(0-255)의 값으로 보정

  //String rgb_value = "RGB Red:" + (String)red_Input + "   Green:" + (String)green_Input + "   Blue:" + (String)blue_Input;
  String rgb_value = (String)red_Input + ":" + (String)green_Input + ":" + (String)blue_Input;
  DebugSerial.println(rgb_value);

  //LoRa Send : HelloWorld
  if (SNIPE.lora_send(rgb_value))
  {
    DebugSerial.println("send success");
  }
  else
  {
    DebugSerial.println("send fail");
    delay(500);
  }
  delay(500);
}
