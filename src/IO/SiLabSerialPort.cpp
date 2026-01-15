#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SiLabDriver.h"
#include "IO/SiLabSerialPort.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"
#if defined(_WIN32)
#include <windows.h>
#else
typedef struct
{
	UOSInt Internal;
	UOSInt InternalHigh;
	union
	{
		struct
		{
			UInt32 Offset;
			UInt32 OffsetHigh;
		};
		void *Pointer;
	};
	void *hEvent;
} OVERLAPPED;
#endif

IO::SiLabSerialPort::SiLabSerialPort(void *handle, IO::SiLabDriver *driver) : IO::Stream(CSTR("SiLabSerialPort"))
{
	this->driver = driver;
	this->handle = handle;
	this->reading = 0;
}

IO::SiLabSerialPort::~SiLabSerialPort()
{
	if (this->handle)
	{
		void *h = this->handle;
		this->handle = 0;
		this->driver->SI_CancelIo(h);
		this->driver->SI_Close(h);
	}
	this->rdEvt.Set();
	while (this->reading)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Bool IO::SiLabSerialPort::IsDown() const
{
	return this->handle == 0;
}

UOSInt IO::SiLabSerialPort::Read(const Data::ByteArray &buff)
{
	UInt32 readCnt;
	Bool ret;
	void *h;
	h = this->handle;
	if (h == 0)
		return 0;
	
	Sync::MutexUsage mutUsage(this->rdMut);
	OVERLAPPED ol;
	ol.hEvent = this->rdEvt.hand;
	ol.Internal = 0;
	ol.InternalHigh = 0;
	ol.Offset = 0;
	ol.OffsetHigh = 0;
	this->reading = true;
	this->driver->SI_Read(h, buff.Arr().Ptr(), (UInt32)buff.GetSize(), &readCnt, &ol);
#if defined(_WIN32) && !defined(_WIN32_WCE)
	this->rdEvt.Wait();
	if (this->handle == 0)
	{
		this->reading = false;
		return 0;
	}

	ret = (GetOverlappedResult(h, &ol, (DWORD*)&readCnt, TRUE) != 0);
#else
	ret = false;
#endif
	mutUsage.EndUse();
	this->reading = false;
	if (ret)
	{
		return readCnt;
	}
	else
	{
		return 0;
	}

}

UOSInt IO::SiLabSerialPort::Write(Data::ByteArrayR buff)
{
	UInt32 writeCnt;
	void *h = this->handle;
	if (h == 0)
		return 0;
#if defined(_WIN32) && !defined(_WIN32_WCE)
	OVERLAPPED ol;
	ol.hEvent = CreateEvent(0, TRUE, FALSE, 0);
	ol.Internal = 0;
	ol.InternalHigh = 0;
	ol.Offset = 0;
	ol.OffsetHigh = 0;
	this->driver->SI_Write(h, (void*)buff.Ptr(), (UInt32)buff.GetSize(), &writeCnt, &ol);
	if (GetOverlappedResult(h, &ol, (LPDWORD)&writeCnt, TRUE))
	{
		CloseHandle(ol.hEvent);
		return writeCnt;
	}
	else
	{
		CloseHandle(ol.hEvent);
		return 0;
	}
#else
	if (this->driver->SI_Write(h, (void*)buff.Ptr(), (UInt32)buff.GetSize(), &writeCnt, 0) == IO::SiLabDriver::SI_SUCCESS)
	{
		return writeCnt;
	}
	else
	{
		return 0;
	}
#endif
}

struct ReadEvent
{
	UnsafeArray<UInt8> buff;
	UOSInt size;	
	NN<Sync::Event> evt;
	UInt32 readSize;
	OVERLAPPED ol;
};

Optional<IO::StreamReadReq> IO::SiLabSerialPort::BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt)
{
	void *h = this->handle;
	if (h == 0)
		return nullptr;

	NN<ReadEvent> re = MemAllocNN(ReadEvent);
	re->buff = buff.Arr();
	re->evt = evt;
	re->size = buff.GetSize();
	re->ol.hEvent = evt->hand;
	re->ol.Internal = 0;
	re->ol.InternalHigh = 0;
	re->ol.Offset = 0;
	re->ol.OffsetHigh = 0;
	this->driver->SI_Read(h, (void*)buff.Arr().Ptr(), (UInt32)buff.GetSize(), &re->readSize, &re->ol);
	return NN<IO::StreamReadReq>::ConvertFrom(re);
}

UOSInt IO::SiLabSerialPort::EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete)
{
	NN<ReadEvent> re = NN<ReadEvent>::ConvertFrom(reqData);
#if defined(_WIN32) && !defined(_WIN32_WCE)
	DWORD retVal;
	Int32 result = GetOverlappedResult(this->handle, &re->ol, (DWORD*)&retVal, TRUE);
	MemFreeNN(re);
	if (result)
		return retVal;
	return 0;
#else
	UInt32 retVal = re->readSize;
	incomplete.Set(false);
	MemFreeNN(re);
	return retVal;
#endif
}

void IO::SiLabSerialPort::CancelRead(NN<IO::StreamReadReq> reqData)
{
//	ReadEvent *re = (ReadEvent*)reqData;
	this->driver->SI_CancelIo(this->handle);
	MemFreeNN(reqData);
//	PurgeComm(this->handle, PURGE_RXABORT);
}

Optional<IO::StreamWriteReq> IO::SiLabSerialPort::BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt)
{
	if (handle == 0)
		return nullptr;
	evt->Set();
	return (IO::StreamWriteReq*)Write(buff);
}

UOSInt IO::SiLabSerialPort::EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait)
{
	return (UOSInt)reqData.Ptr();
}

void IO::SiLabSerialPort::CancelWrite(NN<IO::StreamWriteReq> reqData)
{
}

Int32 IO::SiLabSerialPort::Flush()
{
	this->driver->SI_FlushBuffers(this->handle, 1, 1);
	return 0;
}

void IO::SiLabSerialPort::Close()
{
	if (this->handle)
	{
		void *h = this->handle;
		this->handle = 0;
		this->driver->SI_CancelIo(h);
		this->driver->SI_Close(h);
	}
	this->rdEvt.Set();
}

Bool IO::SiLabSerialPort::Recover()
{
	////////////////////////////////
	return false;
}

IO::StreamType IO::SiLabSerialPort::GetStreamType() const
{
	return IO::StreamType::USBxpress;
}
