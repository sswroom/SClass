#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SerialPort.h"

extern "C"
{
	void SerialPort_Init(Int32 baudRate, IO::SerialPort::ParityType parity);
	void SerialPort_Write(UInt8 b);
	void SerialPort_Flush();
	Bool SerialPort_Available();
	UInt8 SerialPort_Read();
};

IO::SerialPort::SerialPort(UOSInt portNum, UInt32 baudRate, IO::SerialPort::ParityType parity, Bool flowCtrl) : IO::Stream(CSTR("SerialPort"))
{
	this->handle = 0;
	this->rdEvt = 0;
	this->rdMut = 0;
	this->reading = 0;
	this->portNum = portNum;
	this->baudRate = baudRate;
	this->parity = parity;
	this->flowCtrl = flowCtrl;

	if (this->portNum == 1)
	{
		SerialPort_Init(baudRate, parity);
	}
	else
	{
		this->portNum = 0;
	}
}

IO::SerialPort::~SerialPort()
{
}

Bool IO::SerialPort::IsDown() const
{
	return this->IsError();
}

UOSInt IO::SerialPort::Read(UInt8 *buff, UOSInt size)
{
	UOSInt retSize = 1;
	*buff++ = SerialPort_Read();
	while (retSize < size && SerialPort_Available())
	{
		*buff++ = SerialPort_Read();
		retSize++;
	}
	return retSize;
}

UOSInt IO::SerialPort::Write(const UInt8 *buff, UOSInt size)
{
	UOSInt i = size;
	while (size > 0)
	{
		SerialPort_Write(*buff++);
		size--;
	}
	return i;
}

Bool IO::SerialPort::HasData()
{
	return SerialPort_Available();
}

void *IO::SerialPort::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	return 0;
}

UOSInt IO::SerialPort::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
	if (incomplete) *incomplete = false;
	return 0;
}

void IO::SerialPort::CancelRead(void *reqData)
{
}

void *IO::SerialPort::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
}

UOSInt IO::SerialPort::EndWrite(void *reqData, Bool toWait)
{
	return 0;
}

void IO::SerialPort::CancelWrite(void *reqData)
{
}

Int32 IO::SerialPort::Flush()
{
	return 0;
}

void IO::SerialPort::Close()
{
}

Bool IO::SerialPort::Recover()
{
	return true;
}

IO::StreamType IO::SerialPort::GetStreamType() const
{
	return IO::StreamType::SerialPort;
}

Bool IO::SerialPort::IsError() const
{
	return this->portNum == 0;
}

