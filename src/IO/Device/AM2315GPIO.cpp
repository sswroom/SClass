#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "IO/Device/AM2315GPIO.h"
#include "Sync/SimpleThread.h"
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
		Sync::SimpleThread::Sleepus(4); //4.0
		this->sdaPin->SetPinState(b & v);
		this->sclPin->SetPinState(true);
		Sync::SimpleThread::Sleepus(5); //4.7
		this->sclPin->SetPinState(false);
		v = (UInt8)(v >> 1);
	}
	this->sdaPin->SetPinOutput(false);
	Sync::SimpleThread::Sleepus(4); //4.0
	this->sclPin->SetPinState(true);
	if (this->sdaPin->IsPinHigh()) //NACK
	{
		ret = true;
	}
	Sync::SimpleThread::Sleepus(5); //4.7
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
		Sync::SimpleThread::Sleepus(4); //4.0
		this->sclPin->SetPinState(true);
		if (this->sdaPin->IsPinHigh())
		{
			by = by | v;
		}
		Sync::SimpleThread::Sleepus(5); //4.7
		this->sclPin->SetPinState(false);
		v = (UInt8)(v >> 1);
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
	Sync::SimpleThread::Sleepus(4); //4.0
	this->sclPin->SetPinState(true);
	Sync::SimpleThread::Sleepus(5); //4.7
	this->sclPin->SetPinState(false);
	return true;
}

Bool IO::Device::AM2315GPIO::I2CEnd()
{
	printf("I2CEnd\r\n");
	Sync::SimpleThread::Sleepus(4); //4.0
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
	sb.AppendC(UTF8STRC("Read size = "));
	sb.AppendOSInt(i);
	sb.AppendC(UTF8STRC(": "));
	if (i > 0)
	{
		sb.AppendHexBuff(buff, (UOSInt)i, ' ', Text::LineBreakType::None);
	}
	printf("%s\r\n", sb.ToPtr());
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
	Sync::SimpleThread::Sleep(1);
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
	this->crc.Clear();
	this->crc.Calc(buff, 4);
	this->crc.GetValue((UInt8*)&crcVal);
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

IO::Device::AM2315GPIO::AM2315GPIO(NN<IO::IOPin> sdaPin, NN<IO::IOPin> sclPin)
{
	this->sdaPin = sdaPin;
	this->sclPin = sclPin;
	this->error = false;

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
		this->error = true;
		return;
	}
	this->Wait();
	if (this->DirectRead(buff, 3) != 3)
	{
		printf("Read != 3\r\n");
		this->error = true;
		return;
	}
	if (buff[0] != 3 || buff[1] != 7)
	{
		printf("Read content error\r\n");
		this->error = true;
		return;
	}
}

IO::Device::AM2315GPIO::~AM2315GPIO()
{
}

Bool IO::Device::AM2315GPIO::IsError()
{
	return this->error;
}

void IO::Device::AM2315GPIO::Wakeup()
{
	printf("Wakeup\r\n");
	if (!this->I2CStart())
		return;
	this->I2CWriteByte(0xB8);
	Sync::SimpleThread::Sleepus(800);
	this->I2CEnd();
}

Bool IO::Device::AM2315GPIO::ReadTemperature(OutParam<Double> temp)
{
	UInt8 buff[2];
	if (this->IsError())
		return false;
	this->Wakeup();
	if (this->ReadWord(2, buff))
	{
		temp.Set(ReadMInt16(buff) * 0.1);
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::AM2315GPIO::ReadRH(OutParam<Double> rh)
{
	UInt8 buff[2];
	if (this->IsError())
		return false;
	this->Wakeup();
	if (this->ReadWord(0, buff))
	{
		rh.Set(ReadMUInt16(buff) * 0.1);
		return true;
	}
	else
	{
		return false;
	}
}

