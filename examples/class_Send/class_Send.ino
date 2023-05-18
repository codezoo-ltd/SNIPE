#include <SoftwareSerial.h>
#include "SNIPE.h"

#define BUTTON_CALL		2
#define BUTTON_CLEAR	3
#define USER_LED A5
#define WAITING_TIME 3000	/* 3 Seconds */	

int btn1 = 0;
unsigned long button_time1 = 0;
unsigned long last_button_time1 = 0;

int btn2 = 0;
unsigned long button_time2 = 0;
unsigned long last_button_time2 = 0;

int callStatus = 0;		/* 0: default, 1: CALL, 2: CLEAR */
char recvBuffer[8];
int  address;
int  chkCallStatus;

//Pressed for more than 300ms
void SW01_ISR(){
	button_time1 = millis();
	if (button_time1 - last_button_time1 > 300)
	{
		btn1 = 1;
		last_button_time1 = button_time1;
	}
}

void SW02_ISR(){
	button_time2 = millis();
	if (button_time2 - last_button_time2 > 300)
	{
		btn2 = 1;
		last_button_time2 = button_time2;
	}
}
#define ADDR  1 //(0x00 ~ 0xFF)
#define TXpin 11
#define RXpin 10
#define ATSerial Serial

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";

SoftwareSerial DebugSerial(RXpin,TXpin);
SNIPE SNIPE(ATSerial);

void setup() {
	ATSerial.begin(115200);

	while(ATSerial.read()>= 0) {}
	while(!ATSerial);

	DebugSerial.begin(115200);

	pinMode(USER_LED, OUTPUT);
	pinMode(BUTTON_CALL, INPUT_PULLUP);
	pinMode(BUTTON_CLEAR, INPUT_PULLUP);

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

	attachInterrupt(digitalPinToInterrupt(BUTTON_CALL), SW01_ISR, FALLING);
	attachInterrupt(digitalPinToInterrupt(BUTTON_CLEAR), SW02_ISR, FALLING);

	DebugSerial.println("Class Send Test");
}

void loop() {

	// put your main code here, to run repeatedly:
	if(btn1 == 1)   //SW1 interrupt
	{
		btn1 = 0;
		callStatus = 1;	//1: CALL
		digitalWrite(USER_LED, HIGH);
	}

	if(btn2 == 1)   //SW2 interrupt
	{
		btn2 = 0;
		callStatus = 2;	//2: CLEAR
		digitalWrite(USER_LED, HIGH);
	}

	if(callStatus){
		char addr[8];
		String msg;

		sprintf(addr, "%d:%d", ADDR, callStatus);
		msg = (String)addr;
		DebugSerial.println("[Send Message]");
		DebugSerial.println(msg);

		if (SNIPE.lora_send(msg))
		{
			DebugSerial.println("send success");

			String ver = SNIPE.lora_recv();

			if (ver != "AT_RX_TIMEOUT" && ver != "AT_RX_ERROR")
			{
				/* Receive Status */
				DebugSerial.println(SNIPE.lora_getRssi());
				DebugSerial.println(SNIPE.lora_getSnr());
				DebugSerial.println(ver);

				/* if (addr == ADDR) */
				memset(recvBuffer, 0x0, sizeof(recvBuffer));
				ver.toCharArray(recvBuffer, sizeof(recvBuffer));
				sscanf(recvBuffer, "%d:%d", &address, &chkCallStatus);

				if(address == ADDR && chkCallStatus == callStatus){
					callStatus = 0; /* 0: default */
					digitalWrite(USER_LED, LOW);
				}
			}
			else
			{
				DebugSerial.println("recv fail");
			}
		}
		delay(WAITING_TIME);
	}
}
