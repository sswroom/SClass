#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.hpp"
#include "IO/GSMMux.h"
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Sync/SimpleThread.h"
#include <stdlib.h>

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

Int32 IO::GSMMux::SendATCommand(UnsafeArray<UInt8> buffer, Int32 buffSize, UnsafeArrayOpt<UInt8> outBuffer, OptOut<UIntOS> outSize)
{
	NN<IO::Stream> stm;
	if (!this->stm.SetTo(stm))
		return 2;
	NN<Sync::Event> evt;
	NEW_CLASSNN(evt, Sync::Event());
	UInt8 readBuff[1024];

	NN<IO::StreamReadReq> readData;
	if (stm->BeginRead(Data::ByteArray(readBuff, 1024), evt).SetTo(readData))
	{
		stm->Write(Data::ByteArrayR(buffer, buffSize));
		if (evt->Wait(1000))
		{
			evt.Delete();
			stm->CancelRead(readData);
			return 1;
		}
		else
		{
			Bool incomplete;
			UIntOS readSize = stm->EndRead(readData, true, incomplete);
			evt.Delete();
			UnsafeArray<UInt8> nnoutBuffer;
			if (outBuffer.SetTo(nnoutBuffer));
			{
				outSize.Set(readSize);
				Int32 i = readSize;
				UInt8 *src;
				UnsafeArray<UInt8> dest;
				dest = nnoutBuffer;
				src = readBuff;
				while (i--)
					*dest++ = *src++;
				return 0;	
			}
			return 0;
		}
	}
	return 0;
}

UInt8 IO::GSMMux::CalCheck(UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
	UInt8 fcs = 0xff;
	UIntOS i = buffSize;
	while (i-- > 0)
	{
		fcs = GSM_crctable[fcs ^ *buff++];
	}
	return (0xff - fcs);
}

IO::GSMMux::GSMMux(NN<Stream> stm, Int32 baudRate)
{
	Int32 i;
	NEW_CLASSNN(this->readEvt, Sync::Event());
	this->ports = MemAllocArr(GSMPort, MAX_GSMPORT);
	this->readBuff = MemAllocArr(UInt8, 1024);
	this->readBuffSize = 0;
	this->readReq = 0;
	this->checking = false;
	this->closing = false;
	i = MAX_GSMPORT;
	while (i-- > 0)
	{
		this->ports[i].portId = i;
		this->ports[i].opened = false;
		this->ports[i].obj = 0;
		this->ports[i].evt = 0;
		NEW_CLASSNN(this->ports[i].data, Data::ArrayListArr<UInt8>());
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

	this->readReq = stm->BeginRead(Data::ByteArray(readBuff, 1024), this->readEvt);
	this->SendFrame(0, buff, 0, (GSMFrType)(SABM | PF));
}

IO::GSMMux::~GSMMux()
{
	NN<IO::Stream> stm;
	NN<IO::StreamReadReq> readReq;
	this->closing = true;

	if (this->readReq.SetTo(readReq) && this->stm.SetTo(stm))
	{
		this->readEvt->Set();
		stm->CancelRead(readReq);
		this->readReq = 0;
	}

	while (this->readReq.NotNull() || this->checking)
	{
		Sync::SimpleThread::Sleep(10);
	}

	Int32 i = MAX_GSMPORT;
	UIntOS j;
	while (i-- > 0)
	{
		this->ports[i].obj.Delete();
		j = this->ports[i].data->GetCount();
		while (j-- > 0)
		{
			MemFreeArr(this->ports[i].data->GetItemNoCheck(j));
		}
		this->ports[i].data.Delete();
		
	}
	MemFreeArr(this->ports);
	
	if (this->stm.NotNull())
	{
		UInt8 buff[2];
		buff[0] = C_CLD | CR;
		buff[1] = 1;
		this->SendFrame(0, buff, 2, UIH);
	}

	this->readEvt.Delete();
	MemFreeArr(this->readBuff);
}

Optional<IO::GSMMuxPort> IO::GSMMux::OpenVPort()
{
	Int32 ch = 1;
	while (ch <= MAX_GSMPORT)
	{
		if (this->ports[ch].opened == false && this->ports[ch].obj.IsNull())
		{
			NN<GSMMuxPort> port;
			NEW_CLASSNN(port, GSMMuxPort(*this, this->ports[ch]));
			return port;
		}
		ch++;
	}
	return 0;
}

void IO::GSMMux::CloseVPort(NN<GSMMuxPort> port)
{
	port.Delete();
}

Bool IO::GSMMux::IsError()
{
	return this->stm.IsNull();
}

Int32 IO::GSMMux::SendFrame(Int32 channel, UnsafeArray<const UInt8> buffer, UIntOS size, GSMFrType frType)
{
	NN<IO::Stream> stm;
	if (!this->stm.SetTo(stm))
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
	stm->Write(Data::ByteArrayR(prefix, prefixLeng));
	if (size > 0)
	{
		stm->Write(Data::ByteArrayR(buffer, size));
	}
	stm->Write(Data::ByteArrayR(postfix, 2));
	return 0;
}

Bool IO::GSMMux::CheckEvents(Int32 timeout)
{
	NN<IO::Stream> stm;
	if (!this->stm.SetTo(stm))
		return true;
	if (this->closing)
		return true;
	if (this->checking)
		return false;

	this->checking = true;

	if (this->readReq.NotNull())
	{
		NN<IO::StreamReadReq> readReq;
		Bool hasData = !this->readEvt->Wait(timeout);
		if (hasData && this->readReq.SetTo(readReq))
		{
			Bool incomplete;
			UIntOS dataSize = stm->EndRead(readReq, true, incomplete);
			this->readBuffSize += dataSize;
			this->readReq = 0;

			ParseCommData();
		}
	}

	if (this->closing)
	{
		this->checking = false;
		return true;
	}

	if (this->readReq.IsNull())
	{
		this->readReq = stm->BeginRead(Data::ByteArray(&readBuff[readBuffSize], 1024 - readBuffSize), this->readEvt);
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
					MemCopyO(&this->readBuff[0], &this->readBuff[i], this->readBuffSize - i);
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
					MemCopyO(&this->readBuff[0], &this->readBuff[i], this->readBuffSize - i);
					this->readBuffSize -= i;
					return;
				}

				UInt8 ch = (this->readBuff[i + 1] >> 2) & 63;
				UInt8 typ = (this->readBuff[i + 2] & ~PF);

				if (typ == UI || typ == UIH)
				{
					///////////////////////////////////////////////////////////////////////////
					NN<Sync::Event> evt;
					UInt8 *data = MemAlloc(UInt8, frSize + 2);
					MemCopyNO(&data[2], &this->readBuff[frOfst], frSize);
					*(Int16*)data = frSize;
					this->ports[ch].data->Add(data);
					if (this->ports[ch].evt.SetTo(evt))
						evt->Set();
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
			MemCopyO(&this->readBuff[0], &this->readBuff[i], this->readBuffSize - i);
			this->readBuffSize -= i;
		}
	}
}

Optional<IO::GSMMuxPort> IO::GSMMux::HasAnyData()
{
	if (this->closing)
		return 0;
	Int32 i = MAX_GSMPORT;
	while (i-- > 0)
	{
		if (this->ports[i].data->GetCount() > 0)
		{
			if (this->ports[i].obj.NotNull())
				return this->ports[i].obj;
		}
	}
	return 0;
}

IO::GSMMuxPort::GSMMuxPort(NN<GSMMux> mux, NN<GSMMux::GSMPort> portInfo) : IO::Stream(CSTR("GSMMuxPort"))
{
	UInt8 buff[1];
	this->port = portInfo;
	this->mux = mux;
	this->reading = false;
	this->port->opened = true;
	this->port->obj = this;
	mux->SendFrame(this->port->portId, buff, 0, (GSMMux::GSMFrType)(GSMMux::SABM | GSMMux::PF));
}

IO::GSMMuxPort::~GSMMuxPort()
{
	this->Close();
	this->port->obj = 0;
	while (this->reading)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Int32 IO::GSMMuxPort::GetChannel()
{
	return port->portId;
}

Bool IO::GSMMuxPort::IsDown() const
{
	return !this->port->opened;
}

UIntOS IO::GSMMuxPort::Read(const Data::ByteArray &buff)
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
	UnsafeArray<UInt8> data = this->port->data->GetItemNoCheck(0);
	UIntOS dataSize = *(Int16*)&data[0];
	if (dataSize > buff.GetSize())
	{
		MemCopyNO(buff.Arr().Ptr(), &data[2], dataSize);
		MemCopyO(&data[2], &data[2 + buff.GetSize()], *(Int16*)data.Ptr() = dataSize - buff.GetSize());
		this->reading = false;
		return buff.GetSize();
	}
	else
	{
		this->port->data->RemoveAt(0);
		MemCopyNO(buff.Arr().Ptr(), &data[2], dataSize);
		this->reading = false;
		MemFreeArr(data);
		return dataSize;
	}
}

UIntOS IO::GSMMuxPort::Write(Data::ByteArrayR buff)
{
	if (mux->SendFrame(this->port->portId, buff.Arr(), buff.GetSize(), GSMMux::UIH) == 0)
		return buff.GetSize();
	else
		return 0;
}

Optional<IO::StreamReadReq> IO::GSMMuxPort::BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt)
{
	while (this->port->evt.NotNull())
	{
		Sync::SimpleThread::Sleep(10);
	}

	this->port->evt = evt;
	UnsafeArray<IntOS> reqData = MemAllocArr(IntOS, 3);
	reqData[0] = (IntOS)buff.Ptr();
	reqData[1] = (IntOS)buff.GetSize();
	reqData[2] = (IntOS)evt.Ptr();
	if (this->port->data->GetCount() > 0)
	{
		evt->Set();
	}
	return (IO::StreamReadReq*)reqData.Ptr();
}

UIntOS IO::GSMMuxPort::EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete)
{
	UnsafeArray<IntOS> rdata = (IntOS*)reqData.Ptr();
	while (this->port->opened)
	{
		if (this->port->data->GetCount() > 0)
			 break;
		if (!toWait)
			break;
	}
	if (!this->port->opened)
	{
		MemFreeArr(rdata);
		return 0;
	}
	if (this->port->data->GetCount() == 0)
	{
		return 0;
	}

	UIntOS size = (UIntOS)rdata[1];
	UInt8 *buff = (UInt8*)rdata[0];
	UnsafeArray<UInt8> data = this->port->data->GetItemNoCheck(0);
	Int32 dataSize = *(Int16*)&data[0];
	MemFreeArr(rdata);
	if (dataSize > size)
	{
		MemCopyNO(buff, &data[2], dataSize);
		MemCopyO(&data[2], &data[2 + size], *(Int16*)data.Ptr() = dataSize - size);
		return size;
	}
	else
	{
		this->port->data->RemoveAt(0);
		MemCopyNO(buff, &data[2], dataSize);
		return dataSize;
	}
}

void IO::GSMMuxPort::CancelRead(NN<IO::StreamReadReq> reqData)
{
	IntOS *data = (IntOS*)reqData.Ptr();
	if (this->port->evt == (Sync::Event*)data[2])
	{
		this->port->evt = 0;
	}
	MemFree(data);
}

Optional<IO::StreamWriteReq> IO::GSMMuxPort::BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt)
{
	if (mux->SendFrame(this->port->portId, buff.Arr(), buff.GetSize(), GSMMux::UIH) == 0)
	{
		evt->Set();
		return (IO::StreamWriteReq*)buff.GetSize();
	}
	else
	{
		evt->Set();
		return 0;
	}
}

UIntOS IO::GSMMuxPort::EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait)
{
	return (UIntOS)reqData.Ptr();
}

void IO::GSMMuxPort::CancelWrite(NN<IO::StreamWriteReq> reqData)
{
}

Int32 IO::GSMMuxPort::Flush()
{
	return 0;
}

void IO::GSMMuxPort::Close()
{
	UInt8 buff[1];
	if (port->opened)
	{
		mux->SendFrame(port->portId, buff, 0, (GSMMux::GSMFrType)(GSMMux::DISC | GSMMux::PF));
		port->opened = false;
	}
}

Bool IO::GSMMuxPort::Recover()
{
	UInt8 buff[1];
	if (!port->opened)
	{
		this->port->opened = true;
		this->port->obj = this;
		this->mux->SendFrame(this->port->portId, buff, 0, (GSMMux::GSMFrType)(GSMMux::SABM | GSMMux::PF));
		return true;
	}
	return false;
}

IO::StreamType IO::GSMMuxPort::GetStreamType() const
{
	return IO::StreamType::GSMMuxPort;
}
