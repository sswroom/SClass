#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Device/AM2315GPIO.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>

Bool IO::Device::AM2315GPIO::I2CStart()
{
	if (!this->sdaPin->IsPinHigh())
	{
		printf("Error in I2CStart\r\n");
		return false;
	}
	printf("I2CStart\r\n");

	this->sdaPin->SetPinOutput(true);
	this->sdaPin->SetPinState(false);
	this->sclPin->SetPinOutput(true);
	this->sclPin->SetPinState(false);
	return true;
}

Bool IO::Device::AM2315GPIO::I2CWriteByte(UInt8 b)
{
	Bool ret = false;
	UInt8 v;
	OSInt j;
	v = 0x80;
	j = 8;
	while (j-- > 0)
	{
		Sync::Thread::Sleepus(4); //4.0
		this->sdaPin->SetPinState(b & v);
		this->sclPin->SetPinState(true);
		Sync::Thread::Sleepus(5); //4.7
		this->sclPin->SetPinState(false);
		v = v >> 1;
	}
	this->sdaPin->SetPinOutput(false);
	Sync::Thread::Sleepus(4); //4.0
	this->sclPin->SetPinState(true);
	if (this->sdaPin->IsPinHigh()) //NACK
	{
		ret = true;
	}
	Sync::Thread::Sleepus(5); //4.7
	this->sclPin->SetPinState(false);
	this->sdaPin->SetPinOutput(true);
	this->sdaPin->SetPinState(false);
	return ret;
}

Bool IO::Device::AM2315GPIO::I2CReadByte(UInt8 *b, Bool isLast)
{
	UInt8 v;
	UInt8 by;
	OSInt j;
	this->sdaPin->SetPinOutput(false);
	v = 0x80;
	by = 0;
	j = 8;
	while (j-- > 0)
	{
		Sync::Thread::Sleepus(4); //4.0
		this->sclPin->SetPinState(true);
		if (this->sdaPin->IsPinHigh())
		{
			by = by | v;
		}
		Sync::Thread::Sleepus(5); //4.7
		this->sclPin->SetPinState(false);
		v = v >> 1;
	}

	*b = by;
	this->sdaPin->SetPinOutput(true);
	if (isLast)
	{
		this->sdaPin->SetPinState(true);
	}
	else
	{
		this->sdaPin->SetPinState(false);
	}
	Sync::Thread::Sleepus(4); //4.0
	this->sclPin->SetPinState(true);
	Sync::Thread::Sleepus(5); //4.7
	this->sclPin->SetPinState(false);
	return true;
}

Bool IO::Device::AM2315GPIO::I2CEnd()
{
	printf("I2CEnd\r\n");
	Sync::Thread::Sleepus(4); //4.0
	this->sclPin->SetPinOutput(false);
	this->sdaPin->SetPinOutput(false);
	return true;
}

OSInt IO::Device::AM2315GPIO::DirectRead(UInt8 *buff, OSInt readSize)
{
	Text::StringBuilderUTF8 sb;
	if (!this->I2CStart())
		return 0;
	if (this->I2CWriteByte(0xB9))
	{
		this->I2CEnd();
		return 0;
	}
	OSInt i = 0;
	while (i < readSize)
	{
		this->I2CReadByte(&buff[i], i >= readSize - 1);
		i++;
	}
	this->I2CEnd();
	sb.Append((const UTF8Char*)"Read size = ");
	sb.AppendOSInt(i);
	sb.Append((const UTF8Char*)": ");
	if (i > 0)
	{
		sb.AppendHex(buff, i, ' ', Text::LBT_NONE);
	}
	printf("%s\r\n", sb.ToString());
	return i;
}

OSInt IO::Device::AM2315GPIO::DirectWrite(const UInt8 *buff, OSInt writeSize)
{
	if (!this->I2CStart())
		return 0;
	if (this->I2CWriteByte(0xB8))
	{
		this->I2CEnd();
		return 0;
	}
	OSInt i = 0;
	while (i < writeSize)
	{
		if (this->I2CWriteByte(buff[i]))
		{
			i++;
			break;
		}
		i++;
	}
	this->I2CEnd();
	return i;
}

void IO::Device::AM2315GPIO::Wait()
{
	Sync::Thread::Sleep(1);
}

Bool IO::Device::AM2315GPIO::ReadWord(UInt8 regAddr, UInt8 *data)
{
	UInt8 buff[6];
	buff[0] = 3;
	buff[1] = regAddr;
	buff[2] = 2;
	if (DirectWrite(buff, 3) != 3)
	{
		printf("ReadWord: Write Error\r\n");
		return false;
	}
	Wait();
	if (DirectRead(buff, 6) != 6)
	{
		printf("ReadWord: Read Error\r\n");
		return false;
	}
	UInt16 crcVal;
	this->crc->Clear();
	this->crc->Calc(buff, 4);
	this->crc->GetValue((UInt8*)&crcVal);
	if (((UInt16)~crcVal) == ReadUInt16(&buff[4]))
	{
		data[0] = buff[2];
		data[1] = buff[3];
		return true;
	}
	else
	{
		printf("ReadWord: CRC Error %X != %X\r\n", ((UInt16)~crcVal), ReadUInt16(&buff[4]));
		return false;
	}
}

IO::Device::AM2315GPIO::AM2315GPIO(IO::IOPin *sdaPin, IO::IOPin *sclPin)
{
	this->sdaPin = sdaPin;
	this->sclPin = sclPin;
	NEW_CLASS(this->crc, Crypto::Hash::CRC16R());

	this->sdaPin->SetPinOutput(false);
	this->sclPin->SetPinOutput(false);

	UInt8 buff[3];
	buff[0] = 3;
	buff[1] = 8;
	buff[2] = 7;
	this->Wakeup();
	if (this->DirectWrite(buff, 3) != 3)
	{
		printf("Write != 3\r\n");
		this->sdaPin = 0;
		this->sclPin = 0;
		return;
	}
	this->Wait();
	if (this->DirectRead(buff, 3) != 3)
	{
		printf("Read != 3\r\n");
		this->sdaPin = 0;
		this->sclPin = 0;
		return;
	}
	if (buff[0] != 3 || buff[1] != 7)
	{
		printf("Read content error\r\n");
		this->sdaPin = 0;
		this->sclPin = 0;
		return;
	}
}

IO::Device::AM2315GPIO::~AM2315GPIO()
{
	SDEL_CLASS(this->crc);
}

Bool IO::Device::AM2315GPIO::IsError()
{
	return this->sdaPin == 0 || this->sclPin == 0;
}

void IO::Device::AM2315GPIO::Wakeup()
{
	printf("Wakeup\r\n");
	if (!this->I2CStart())
		return;
	this->I2CWriteByte(0xB8);
	Sync::Thread::Sleepus(800);
	this->I2CEnd();
}

Bool IO::Device::AM2315GPIO::ReadTemperature(Double *temp)
{
	UInt8 buff[2];
	if (this->IsError())
		return false;
	this->Wakeup();
	if (this->ReadWord(2, buff))
	{
		*temp = ReadMInt16(buff) * 0.1;
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::AM2315GPIO::ReadRH(Double *rh)
{
	UInt8 buff[2];
	if (this->IsError())
		return false;
	this->Wakeup();
	if (this->ReadWord(0, buff))
	{
		*rh = ReadMUInt16(buff) * 0.1;
		return true;
	}
	else
	{
		return false;
	}
}

