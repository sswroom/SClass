#include "stdafx.h"
#include "MyMemory.h"
#include "Sync/Event.h"
#include "Data/ArrayList.h"
#include "IO/Stream.h"
#include "IO/GSMMux.h"
#include <stdlib.h>
#include <windows.h>

Int32 GSMSpeed[] = {0, 9600, 19200, 38400, 57600, 115200, 230400, 460800};
#define MAX_GSMPORT 4

#define CR 2
#define EA 1

#define C_CLD 193
#define C_TEST 33
#define C_MSC 225
#define C_NSC 17

#define F_FLAG 0xF9

const UInt8 GSM_crctable[256] = { //reversed, 8-bit, poly=0x07 
  0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 
  0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B, 
  0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 
  0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67, 
  0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 
  0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43, 
  0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 
  0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F, 
  0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 
  0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B, 
  0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 
  0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17, 
  0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 
  0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33, 
  0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 
  0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F, 
  0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 
  0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B, 
  0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 
  0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87, 
  0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 
  0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3, 
  0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 
  0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF, 
  0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 
  0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB, 
  0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 
  0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7, 
  0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 
  0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3, 
  0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 
  0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF };

IO::GSMMux::GSMMux()
{
}

Int32 IO::GSMMux::SendATCommand(UInt8 *buffer, Int32 buffSize, UInt8 *outBuffer, Int32 *outSize)
{
	if (this->stm == 0)
		return 2;
	Sync::Event *evt;
	NEW_CLASS(evt, Sync::Event(L"IO.GSMMux.SendATCommand.evt"));
	UInt8 readBuff[1024];

	void *readData = stm->BeginRead(readBuff, 1024, evt);
	stm->Write(buffer, buffSize);
	if (evt->Wait(1000))
	{
		delete evt;
		stm->CancelRead(readData);
		return 1;
	}
	else
	{
		Int32 readSize = stm->EndRead(readData, true);
		DEL_CLASS(evt);
		if (outBuffer)
		{
			*outSize = 0;
			*outSize += readSize;
			Int32 i = readSize;
			UInt8 *src;
			UInt8 *dest;
			dest = outBuffer;
			src = readBuff;
			while (i--)
				*dest++ = *src++;
			return 0;	
		}
		return 0;
	}
}

UInt8 IO::GSMMux::CalCheck(UInt8 *buff, Int32 buffSize)
{
	UInt8 fcs = 0xff;
	Int32 i = buffSize;
	while (i-- > 0)
	{
		fcs = GSM_crctable[fcs ^ *buff++];
	}
	return (0xff - fcs);
}

IO::GSMMux::GSMMux(Stream *stm, Int32 baudRate)
{
	Int32 i;
	NEW_CLASS(this->readEvt, Sync::Event(L"IO.GSMMux.readEvt"));
	this->ports = MemAlloc(GSMPort, MAX_GSMPORT);
	this->readBuff = MemAlloc(UInt8, 1024);
	this->readBuffSize = 0;
	this->reading = false;
	this->checking = false;
	this->closing = false;
	i = MAX_GSMPORT;
	while (i-- > 0)
	{
		this->ports[i].portId = i;
		this->ports[i].opened = false;
		this->ports[i].obj = 0;
		this->ports[i].evt = 0;
		NEW_CLASS(this->ports[i].data, Data::ArrayList<UInt8*>());
	}
	this->ports[0].opened = true;

	i = sizeof(GSMSpeed) / sizeof(GSMSpeed[0]);
	while (i--)
	{
		if (baudRate == GSMSpeed[i])
		{
			break;
		}
	}
	if (i <= 0)
	{
		this->stm = 0;
		return;
	}

	UInt8 buff[] = "AT+CMUX=0,0,0\r\n";
	buff[12] = 0x30 + i;

	this->stm = stm;
	Int32 retryCnt = 3;

	while (this->SendATCommand(buff, 15, 0, 0) != 0)
	{
		if (retryCnt-- <= 0)
		{
			this->stm = 0;
			return;
		}
		UInt8 buff[2];
		buff[0] = C_CLD | CR;
		buff[1] = 1;
		this->SendFrame(0, buff, 2, UIH);

	}

	this->reading = true;
	this->readReq = stm->BeginRead(readBuff, 1024, this->readEvt);
	this->SendFrame(0, 0, 0, (GSMFrType)(SABM | PF));
}

IO::GSMMux::~GSMMux()
{
	this->closing = true;

	if (this->reading)
	{
		this->reading = false;
		this->readEvt->Set();
		this->stm->CancelRead(readReq);
		this->readReq = 0;
	}

	while (this->reading || this->checking)
	{
		Sleep(10);
	}

	if (this->ports)
	{
		Int32 i = MAX_GSMPORT;
		Int32 j;
		while (i-- > 0)
		{
			if (this->ports[i].obj)
			{
				delete this->ports[i].obj;
			}
			if (this->ports[i].data)
			{
				j = this->ports[i].data->GetCount();
				while (j-- > 0)
				{
					MemFree(this->ports[i].data->GetItem(j));
					this->ports[i].data->RemoveAt(j);
				}
				delete this->ports[i].data;
			}
			
		}
		MemFree(this->ports);
		this->ports = 0;
	}
	
	if (this->stm)
	{
		UInt8 buff[2];
		buff[0] = C_CLD | CR;
		buff[1] = 1;
		this->SendFrame(0, buff, 2, UIH);
	}

	if (this->readEvt)
	{
		delete this->readEvt;
		this->readEvt = 0;
	}

	if (this->readBuff)
	{
		MemFree(this->readBuff);
		this->readBuff = 0;
	}
}

IO::GSMMuxPort *IO::GSMMux::OpenVPort()
{
	Int32 ch = 1;
	while (ch <= MAX_GSMPORT)
	{
		if (this->ports[ch].opened == false && this->ports[ch].obj == 0)
		{
			GSMMuxPort *port = new GSMMuxPort(this, &this->ports[ch]);
			return port;
		}
		ch++;
	}
	return 0;
}

void IO::GSMMux::CloseVPort(GSMMuxPort *port)
{
	delete port;
}

Bool IO::GSMMux::IsError()
{
	return this->stm == 0;
}

Int32 IO::GSMMux::SendFrame(Int32 channel, const UInt8 *buffer, Int32 size, GSMFrType frType)
{
	if (this->stm == 0)
		return 1;
	UInt8 prefix[5] = { F_FLAG, EA | CR, 0, 0, 0 };
	UInt8 postfix[2] = {0xff, F_FLAG};
	Int32 prefixLeng = 4;
	prefix[1] = prefix[1] | ((63 & channel) << 2);
	prefix[2] = frType;

	if (size > 127)
	{
		prefixLeng = 5;
		prefix[3] = ((127 & size) << 1);
		prefix[4] = (32640 & size) >> 7;
	}
	else
	{
		prefix[3] = 1 | (size << 1);
	}
	postfix[0] = CalCheck(prefix + 1, prefixLeng - 1);
	stm->Write(prefix, prefixLeng);
	if (size > 0)
	{
		stm->Write(buffer, size);
	}
	stm->Write(postfix, 2);
	return 0;
}

Bool IO::GSMMux::CheckEvents(Int32 timeout)
{
	if (this->stm == 0)
		return true;
	if (this->closing)
		return true;
	if (this->checking)
		return false;

	this->checking = true;

	if (this->reading)
	{
		Bool hasData = !this->readEvt->Wait(timeout);
		if (hasData && this->reading)
		{
			Int32 dataSize = this->stm->EndRead(this->readReq, true);
			this->readBuffSize += dataSize;
			this->reading = false;

			ParseCommData();
		}
	}

	if (this->closing)
	{
		this->checking = false;
		return true;
	}

	if (!this->reading)
	{
		this->reading = true;
		this->readReq = stm->BeginRead(&readBuff[readBuffSize], 1024 - readBuffSize, this->readEvt);
	}
	this->checking = false;
	return false;
}

void IO::GSMMux::ParseCommData()
{
	Int32 i;
	while (this->readBuffSize > 0)
	{
		i = 0;
		while (i < this->readBuffSize)
		{
			if (this->readBuff[i] == F_FLAG)
			{
				if ((i + 6) > this->readBuffSize)
				{
					MemCopy(this->readBuff, &this->readBuff[i], this->readBuffSize - i);
					this->readBuffSize -= i;
					return;
				}

				Int32 frSize;
				Int32 frOfst;
				if (this->readBuff[i + 3] & 1)
				{
					frSize = (this->readBuff[i + 3] >> 1) & 0x7f;
					frOfst = i + 4;
				}
				else
				{
					frSize = ((this->readBuff[i + 3] >> 1) & 0x7f) | (this->readBuff[i + 4] << 7);
					frOfst = i + 5;
				}

				if (frOfst + frSize + 2 > this->readBuffSize)
				{
					MemCopy(this->readBuff, &this->readBuff[i], this->readBuffSize - i);
					this->readBuffSize -= i;
					return;
				}

				UInt8 ch = (this->readBuff[i + 1] >> 2) & 63;
				UInt8 typ = (this->readBuff[i + 2] & ~PF);

				if (typ == UI || typ == UIH)
				{
					///////////////////////////////////////////////////////////////////////////
					UInt8 *data = MemAlloc(UInt8, frSize + 2);
					MemCopy(&data[2], &this->readBuff[frOfst], frSize);
					*(Int16*)data = frSize;
					this->ports[ch].data->Add(data);
					if (this->ports[ch].evt)
						this->ports[ch].evt->Set();
				}
				else if (typ == UA)
				{
					///////////////////////////////////////////////////////////////////////////
				}
				else if (typ == DM)
				{
					///////////////////////////////////////////////////////////////////////////
				}
				else if (typ == DISC)
				{
					///////////////////////////////////////////////////////////////////////////
				}
				else if (typ == SABM)
				{
					///////////////////////////////////////////////////////////////////////////
				}

				i = frOfst + frSize + 2;
				break;
			}
			i++;
		}

		if (i >= this->readBuffSize)
		{
			this->readBuffSize = 0;
			break;
		}
		else
		{
			MemCopy(this->readBuff, &this->readBuff[i], this->readBuffSize - i);
			this->readBuffSize -= i;
		}
	}
}

IO::GSMMuxPort *IO::GSMMux::HasAnyData()
{
	if (this->closing)
		return 0;
	Int32 i = MAX_GSMPORT;
	while (i-- > 0)
	{
		if (this->ports[i].data->GetCount() > 0)
		{
			if (this->ports[i].obj)
				return this->ports[i].obj;
		}
	}
	return 0;
}

IO::GSMMuxPort::GSMMuxPort(GSMMux *mux, GSMMux::GSMPort *portInfo)
{
	this->port = portInfo;
	this->mux = mux;
	this->reading = false;
	this->port->opened = true;
	this->port->obj = this;
	mux->SendFrame(this->port->portId, 0, 0, (GSMMux::GSMFrType)(GSMMux::SABM | GSMMux::PF));
}

IO::GSMMuxPort::~GSMMuxPort()
{
	this->Close();
	this->port->obj = 0;
	while (this->reading)
	{
		Sleep(10);
	}
}

Int32 IO::GSMMuxPort::GetChannel()
{
	return port->portId;
}

Int32 IO::GSMMuxPort::Read(UInt8 *buff, Int32 size)
{
	while (this->reading)
	{
		if (!this->port->opened)
			return 0;
	}
	this->reading = true;
	while (this->port->data->GetCount() == 0)
	{
		this->mux->CheckEvents(20);
		if (!this->port->opened)
		{
			this->reading = false;
			return 0;
		}
	}
	UInt8 *data = this->port->data->GetItem(0);
	Int32 dataSize = *(Int16*)data;
	if (dataSize > size)
	{
		MemCopy(buff, &data[2], dataSize);
		MemCopy(&data[2], &data[2 + size], *(Int16*)data = dataSize - size);
		this->reading = false;
		return size;
	}
	else
	{
		this->port->data->RemoveAt(0);
		MemCopy(buff, &data[2], dataSize);
		this->reading = false;
		MemFree(data);
		return dataSize;
	}
}

Int32 IO::GSMMuxPort::Write(const UInt8 *buff, Int32 size)
{
	if (mux->SendFrame(this->port->portId, buff, size, GSMMux::UIH) == 0)
		return size;
	else
		return 0;
}

void *IO::GSMMuxPort::BeginRead(UInt8 *buff, Int32 size, Sync::Event *evt)
{
	while (this->port->evt)
	{
		Sleep(10);
	}

	this->port->evt = evt;
	Int32 *reqData = MemAlloc(Int32, 3);
	reqData[0] = (Int32)buff;
	reqData[1] = size;
	reqData[2] = (Int32)evt;
	if (this->port->data->GetCount() > 0)
	{
		evt->Set();
	}
	return reqData;
}

Int32 IO::GSMMuxPort::EndRead(void *reqData, Bool toWait)
{
	Int32 *rdata = (Int32*)reqData;
	while (this->port->opened)
	{
		if (this->port->data->GetCount() > 0)
			 break;
		if (!toWait)
			break;
	}
	if (!this->port->opened)
	{
		MemFree(rdata);
		return 0;
	}
	if (this->port->data->GetCount() == 0)
	{
		return 0;
	}

	Int32 size = rdata[1];
	UInt8 *buff = (UInt8*)rdata[0];
	UInt8 *data = (UInt8*)this->port->data->GetItem(0);
	Int32 dataSize = *(Int16*)data;
	MemFree(rdata);
	if (dataSize > size)
	{
		MemCopy(buff, &data[2], dataSize);
		MemCopy(&data[2], &data[2 + size], *(Int16*)data = dataSize - size);
		return size;
	}
	else
	{
		this->port->data->RemoveAt(0);
		MemCopy(buff, &data[2], dataSize);
		return dataSize;
	}
}

void IO::GSMMuxPort::CancelRead(void *reqData)
{
	Int32 *data = (Int32*)reqData;
	if (this->port->evt == (Sync::Event*)data[2])
	{
		this->port->evt = 0;
	}
	MemFree(data);
}

void *IO::GSMMuxPort::BeginWrite(const UInt8 *buff, Int32 size, Sync::Event *evt)
{
	if (mux->SendFrame(this->port->portId, buff, size, GSMMux::UIH) == 0)
	{
		evt->Set();
		return (void*)size;
	}
	else
	{
		evt->Set();
		return 0;
	}
}

Int32 IO::GSMMuxPort::EndWrite(void *reqData, Bool toWait)
{
	return (Int32)reqData;
}

void IO::GSMMuxPort::CancelWrite(void *reqData)
{
}

Int32 IO::GSMMuxPort::Flush()
{
	return 0;
}

void IO::GSMMuxPort::Close()
{
	if (port->opened)
	{
		mux->SendFrame(port->portId, 0, 0, (GSMMux::GSMFrType)(GSMMux::DISC | GSMMux::PF));
		port->opened = false;
	}
}
