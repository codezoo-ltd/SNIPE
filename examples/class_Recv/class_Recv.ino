#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include "SNIPE.h"

#define TXpin 9
#define RXpin 8
#define ATSerial Serial

char classCallValue[5];	/* 0:class1, 1:class2, 2:class3, 3:class4, 4:class5 */

//16byte hex key
String lora_app_key = "11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff 00";

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial DebugSerial(RXpin, TXpin);
SNIPE SNIPE(ATSerial);
char buffer[8];
int addr;
int callStatus;

void setup() {
	ATSerial.begin(115200);

	// put your setup code here, to run once:
	while (ATSerial.read() >= 0) {}
	while (!ATSerial);

	DebugSerial.begin(115200);

	// classCallValue initialization (all Zero)
	for (int i = 0; i < 5; i++) {
		classCallValue[i] = '0';
	}

	// initialize the LCD
	lcd.begin();

	// Turn on the blacklight and print a message.
	lcd.backlight();
	lcd.setCursor(0, 0);
	lcd.print("1:");
	lcd.setCursor(2, 0);
	lcd.write(classCallValue[0]);

	lcd.setCursor(4, 0);
	lcd.print("2:");
	lcd.setCursor(6, 0);
	lcd.write(classCallValue[1]);

	lcd.setCursor(8, 0);
	lcd.print("3:");
	lcd.setCursor(10, 0);
	lcd.write(classCallValue[2]);

	lcd.setCursor(0, 1);
	lcd.print("4:");
	lcd.setCursor(2, 1);
	lcd.write(classCallValue[3]);

	lcd.setCursor(4, 1);
	lcd.print("5:");
	lcd.setCursor(6, 1);
	lcd.write(classCallValue[4]);

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
	   If you select LORA_SF_12,
	   RX Timout use a value greater than 5000
	 */
	if (!SNIPE.lora_setRxtout(5000)) {
		DebugSerial.println("SNIPE LoRa Rx Timout value has not been changed");
	}

	DebugSerial.println("Class Recv");

}

void loop() {
	String ver = SNIPE.lora_recv();

	if (ver != "AT_RX_TIMEOUT" && ver != "AT_RX_ERROR")
	{
		DebugSerial.println(SNIPE.lora_getRssi());
		DebugSerial.println(SNIPE.lora_getSnr());

		DebugSerial.println(ver);
		memset(buffer, 0x0, sizeof(buffer));
		ver.toCharArray(buffer, sizeof(buffer));
		sscanf(buffer, "%d:%d", &addr, &callStatus);

		/* value check */
		if ( addr > 0 && addr < 6 && callStatus > 0 && callStatus < 3 ) {
			/* addr = 1~5 */
			if(callStatus == 1){
				classCallValue[addr-1] = '1'; 
			}else if(callStatus == 2){
				classCallValue[addr-1] = '0';
			}

			lcd.clear();

			lcd.backlight();
			lcd.setCursor(0, 0);
			lcd.print("1:");
			lcd.setCursor(2, 0);
			lcd.write(classCallValue[0]);

			lcd.setCursor(4, 0);
			lcd.print("2:");
			lcd.setCursor(6, 0);
			lcd.write(classCallValue[1]);

			lcd.setCursor(8, 0);
			lcd.print("3:");
			lcd.setCursor(10, 0);
			lcd.write(classCallValue[2]);

			lcd.setCursor(0, 1);
			lcd.print("4:");
			lcd.setCursor(2, 1);
			lcd.write(classCallValue[3]);

			lcd.setCursor(4, 1);
			lcd.print("5:");
			lcd.setCursor(6, 1);
			lcd.write(classCallValue[4]);


			String send_value;
			memset(buffer, 0x0, sizeof(buffer));

			sprintf(buffer, "%d:%d", addr, callStatus);
			send_value = String(buffer);

			DebugSerial.println("-----LoRa_send_value-----");
			DebugSerial.println(send_value);

			if (SNIPE.lora_send(send_value))
			{
				DebugSerial.println("send success");
			}
			else
			{
				DebugSerial.println("send fail");
				delay(500);
			}
		}

	} else
		DebugSerial.println("Recv Ready...");
}
