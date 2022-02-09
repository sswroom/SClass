#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/SiLabDriver.h"
#include "IO/SiLabSerialPort.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
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
	this->rdEvt = 0;
	this->rdMut = 0;
	this->reading = 0;
	NEW_CLASS(this->rdEvt, Sync::Event());
	NEW_CLASS(this->rdMut, Sync::Mutex());
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
	if (this->rdEvt)
		this->rdEvt->Set();
	while (this->reading)
	{
		Sync::Thread::Sleep(10);
	}

	if (this->rdEvt)
	{
		DEL_CLASS(this->rdEvt);
		this->rdEvt = 0;
	}
	if (this->rdMut)
	{
		DEL_CLASS(this->rdMut);
		this->rdMut = 0;
	}
}

Bool IO::SiLabSerialPort::IsDown()
{
	return this->handle == 0;
}

UOSInt IO::SiLabSerialPort::Read(UInt8 *buff, UOSInt size)
{
	UInt32 readCnt;
	Bool ret;
	void *h;
	h = this->handle;
	if (h == 0)
		return 0;
	
	Sync::MutexUsage mutUsage(this->rdMut);
	OVERLAPPED ol;
	ol.hEvent = this->rdEvt->hand;
	ol.Internal = 0;
	ol.InternalHigh = 0;
	ol.Offset = 0;
	ol.OffsetHigh = 0;
	this->reading = true;
	this->driver->SI_Read(h, buff, (UInt32)size, &readCnt, &ol);
#if defined(_WIN32) && !defined(_WIN32_WCE)
	this->rdEvt->Wait();
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

UOSInt IO::SiLabSerialPort::Write(const UInt8 *buff, UOSInt size)
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
	this->driver->SI_Write(h, (void*)buff, (UInt32)size, &writeCnt, &ol);
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
	if (this->driver->SI_Write(h, (void*)buff, (UInt32)size, &writeCnt, 0) == IO::SiLabDriver::SI_SUCCESS)
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
	UInt8 *buff;
	UOSInt size;	
	Sync::Event *evt;
	UInt32 readSize;
	OVERLAPPED ol;
};

void *IO::SiLabSerialPort::BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	void *h = this->handle;
	if (h == 0)
		return 0;

	ReadEvent *re = MemAlloc(ReadEvent, 1);
	re->buff = buff;
	re->evt = evt;
	re->size = size;
	re->ol.hEvent = evt->hand;
	re->ol.Internal = 0;
	re->ol.InternalHigh = 0;
	re->ol.Offset = 0;
	re->ol.OffsetHigh = 0;
	this->driver->SI_Read(h, (void*)buff, (UInt32)size, &re->readSize, &re->ol);
	return re;
}

UOSInt IO::SiLabSerialPort::EndRead(void *reqData)
{
	ReadEvent *re = (ReadEvent*)reqData;
#if defined(_WIN32) && !defined(_WIN32_WCE)
	DWORD retVal;
	Int32 result = GetOverlappedResult(this->handle, &re->ol, (DWORD*)&retVal, TRUE);
	MemFree(re);
	if (result)
		return retVal;
	return 0;
#else
	UInt32 retVal = re->readSize;
	MemFree(re);
	return retVal;
#endif
}

void IO::SiLabSerialPort::CancelRead(void *reqData)
{
//	ReadEvent *re = (ReadEvent*)reqData;
	this->driver->SI_CancelIo(this->handle);
	MemFree(reqData);
//	PurgeComm(this->handle, PURGE_RXABORT);
}

void *IO::SiLabSerialPort::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	evt->Set();
	if (handle == 0)
		return 0;
	return (void*)Write(buff, size);
}

UOSInt IO::SiLabSerialPort::EndWrite(void *reqData)
{
	return (UOSInt)reqData;
}

void IO::SiLabSerialPort::CancelWrite(void *reqData)
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
	if (this->rdEvt)
	{
		this->rdEvt->Set();
	}
}

Bool IO::SiLabSerialPort::Recover()
{
	////////////////////////////////
	return false;
}
