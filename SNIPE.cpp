/*
 * A library for controlling SNIPE LoRa radio.
 *
 * @Author Rooney.Jang
 * @Date 08/28/2018
 *
 */

#include "Arduino.h"
#include "SNIPE.h"

extern "C" {
#include "string.h"
#include "stdlib.h"
}

/*
   @param serial Needs to be an already opened Stream ({Software/Hardware}Serial) to write to and read from.
 */
SNIPE::SNIPE(Stream& serial):
	_serial(serial)
{
	_timeOut = 2000;	/* default Timeout */
	_serial.setTimeout( (_timeOut+500) );	/* +500ms, LoRa TX/RX Timeout default 2000 */
}

bool SNIPE::lora_init()
{
	int cnt = 10;	//retry count

	String ret = sendRawCommand(F("AT"));
	if (!ret.startsWith("OK"))
	{
		while(cnt){
			cnt--;
			delay(1000);

			ret = sendRawCommand(F("AT"));
			if (ret.startsWith("OK"))
				break;
		}
		if(cnt)
			return true;
		else
			return false;
	}
	else 
	{
		return true;
	}
}

void SNIPE::lora_reset()
{
	sendRawCommand(F("ATZ"));
}


String SNIPE::lora_getFreq()
{
	String ret = sendRawCommand(F("AT+FREQ=?"));

	ret.trim();

	return ret;
}

bool SNIPE::lora_setFreq(int freq)
{
	String ret = sendRawCommand("AT+FREQ=" + (String)freq);
	if (ret.startsWith("OK"))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

String SNIPE::lora_getTxp()
{
	String ret = sendRawCommand(F("AT+TXP=?"));
	ret.trim();

	return ret;
}

bool SNIPE::lora_setTxp(int txp)
{
	String ret = sendRawCommand("AT+TXP=" + (String)txp);
	if (ret.startsWith("OK"))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

String SNIPE::lora_getSf()
{
	String ret = sendRawCommand(F("AT+SF=?"));
	ret.trim();

	return ret;
}

bool SNIPE::lora_setSf(int sf)
{
	String ret = sendRawCommand("AT+SF=" + (String)sf);
	if (ret.startsWith("OK"))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

String SNIPE::lora_getRxtout()
{
	String ret = sendRawCommand(F("AT+RXTOUT=?"));
	ret.trim();

	return ret;
}

bool SNIPE::lora_setRxtout(int rxtout)
{
	/* Serial timeout calibration */
	if(_timeOut < rxtout){
		_timeOut = rxtout;
		_serial.setTimeout( (_timeOut+500) );
	}

	String ret = sendRawCommand("AT+RXTOUT=" + (String)rxtout);
	if (ret.startsWith("OK"))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

String SNIPE::lora_getTxtout()
{
	String ret = sendRawCommand(F("AT+TXTOUT=?"));
	ret.trim();

	return ret;
}

bool SNIPE::lora_setTxtout(int txtout)
{
	/* Serial timeout calibration */
	if(_timeOut < txtout){
		_timeOut = txtout;
		_serial.setTimeout( (_timeOut+500) );
	}

	String ret = sendRawCommand("AT+TXTOUT=" + (String)txtout);
	if (ret.startsWith("OK"))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool SNIPE::lora_setAESMode(int mode)
{
	String ver;
	switch (mode)
	{
		case 0:
			ver = sendRawCommand(F("AT+USEAES=0")); //Set AES Disable mode.
			break;
		case 1:
			ver = sendRawCommand(F("AT+USEAES=1")); //Set AES Enable mode.
			break;
		default:
			return false;
	}
	if (ver.startsWith("OK"))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

String SNIPE::lora_getAppKey()
{
	String ret = sendRawCommand(F("AT+APPKEY=?"));
	ret.trim();

	return ret;
}

bool SNIPE::lora_setAppKey(String appKEY)
{
	String command = "";

	if (appKEY.length() != 47 )
	{
//		Serial.println("The parameter appKEY is set incorrectly!");
		return false;
	}

	command = "AT+APPKEY=" + appKEY;
	//  Serial.println(command);
	String ret = sendRawCommand(command);
	if (ret.startsWith("OK"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

String SNIPE::lora_getRssi()
{
	String ret = sendRawCommand(F("AT+RSSI=?"));
	ret.trim();

	return ret;
}

String SNIPE::lora_getSnr()
{
	String ret = sendRawCommand(F("AT+SNR=?"));
	ret.trim();

	return ret;
}

String SNIPE::lora_getVersion()
{
	String ret = sendRawCommand(F("AT+VER=?"));
	ret.trim();

	return ret;
}

bool SNIPE::lora_setConf()
{
	String ret = sendRawCommand("AT+LORASET");
	if (ret.startsWith("OK"))
	{
		return true;
	}
	else 
	{
		return false;
	}
}

bool SNIPE::lora_send(String Text)
{
	String command = "";

	if (Text.length() > 120 )
	{
		Serial.println("The number of characters exceeded 120!");
		return false;
	}

	command = "AT+SEND=" + Text;
	//  Serial.println(command);
	String ret = sendRawCommand(command);
	if (ret.startsWith("OK"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool SNIPE::lora_sendBinary(String Hex)
{
	String command = "";

	if (Hex.length() > 240 )
	{
		Serial.println("The number of hex characters exceeded 240!");
		return false;
	}

	command = "AT+SENDB=" + Hex;
	//  Serial.println(command);
	String ret = sendRawCommand(command);
	if (ret.startsWith("OK"))
	{
		return true;
	}
	else
	{
		return false;
	}
}

String SNIPE::lora_recv()
{
	String ret = sendRawCommand(F("AT+RECV"));
	ret.trim();

	return ret;
}

String SNIPE::lora_recvBinary()
{
	String ret = sendRawCommand(F("AT+RECVB"));
	ret.trim();

	return ret;
}

String SNIPE::sendRawCommand(String command)
{
	String ret;

	delay(100);

	while(_serial.available())
		_serial.read();

	_serial.println(command);
	ret = _serial.readStringUntil('\n');
	ret.trim();

	return ret;
}
