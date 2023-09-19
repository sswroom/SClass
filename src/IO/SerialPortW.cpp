#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Registry.h"
#include "IO/Stream.h"
#include "IO/SerialPort.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include <windows.h>
#define OPEN_EXISITING 3

#define ADDMESSAGE(msg)
#ifdef PARITY_ODD
#undef PARITY_ODD
#endif
#ifdef PARITY_EVEN
#undef PARITY_EVEN
#endif
#ifdef PARITY_NONE
#undef PARITY_NONE
#endif

Bool IO::SerialPort::InitStream()
{
	if (portNum <= 0 || portNum > 99)
	{
		return false;
	}

	WChar buff[16];
	DCB dcb;
	if (portNum >= 10)
	{
		Text::StrUOSInt(Text::StrConcat(buff, L"\\\\.\\COM"), portNum);
	}
	else
	{
#ifdef _WIN32_WCE
		Text::StrConcat(Text::StrInt32(Text::StrConcat(buff, L"COM"), portNum), L":");
#else
		Text::StrUOSInt(Text::StrConcat(buff, L"COM"), portNum);
#endif
	}

	void *hand;
	this->handle = 0;
#ifdef _WIN32_WCE
	hand = CreateFileW(buff, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISITING, 0, 0);
#else
	hand = CreateFileW(buff, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISITING, FILE_FLAG_OVERLAPPED, 0);
#endif
	if (hand == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	ZeroMemory(&dcb, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);
	switch (baudRate)
	{
	case 110:
		dcb.BaudRate = CBR_110;
		break;
	case 300:
		dcb.BaudRate = CBR_300;
		break;
	case 600:
		dcb.BaudRate = CBR_600;
		break;
	case 1200:
		dcb.BaudRate = CBR_1200;
		break;
	case 2400:
		dcb.BaudRate = CBR_2400;
		break;
	case 4800:
		dcb.BaudRate = CBR_4800;
		break;
	case 9600:
		dcb.BaudRate = CBR_9600;
		break;
	case 14400:
		dcb.BaudRate = CBR_14400;
		break;
	case 19200:
		dcb.BaudRate = CBR_19200;
		break;
	case 38400:
		dcb.BaudRate = CBR_38400;
		break;
	case 57600:
		dcb.BaudRate = CBR_57600;
		break;
	case 115200:
		dcb.BaudRate = CBR_115200;
		break;
	case 128000:
		dcb.BaudRate = CBR_128000;
		break;
	case 256000:
		dcb.BaudRate = CBR_256000;
		break;
	}
	dcb.fBinary = TRUE;
	if (flowCtrl)
	{
		dcb.fDtrControl = DTR_CONTROL_ENABLE;
		dcb.fRtsControl = RTS_CONTROL_ENABLE;
	}
	dcb.ByteSize = 8;
	switch (this->parity)
	{
	case IO::SerialPort::PARITY_ODD:
		dcb.fParity = TRUE;
		dcb.Parity = ODDPARITY;
		break;
	case IO::SerialPort::PARITY_EVEN:
		dcb.fParity = TRUE;
		dcb.Parity = EVENPARITY;
		break;
	case IO::SerialPort::PARITY_NONE:
	default:
		dcb.Parity = NOPARITY;
		break;
	}
	dcb.StopBits = ONESTOPBIT;
	SetCommState(hand, &dcb);
	SetCommMask(hand, EV_RXCHAR);
	SetupComm(hand, 1600, 1600);
	COMMTIMEOUTS to;
	to.ReadIntervalTimeout = 10;
	to.ReadTotalTimeoutConstant = 0;
	to.ReadTotalTimeoutMultiplier = 0;
	to.WriteTotalTimeoutConstant = 5000;
	to.WriteTotalTimeoutMultiplier = 0;
	SetCommTimeouts(hand, &to);
	this->handle = hand;
	return true;
}

Bool IO::SerialPort::GetAvailablePorts(NotNullPtr<Data::ArrayList<UOSInt>> ports, Data::ArrayList<SerialPortType> *portTypes)
{
	IO::Registry *reg;
	IO::Registry *comreg;
	Bool succ = false;
	WChar wbuff[512];
	WChar wbuff2[32];
	reg = IO::Registry::OpenLocalHardware();
	if (reg)
	{
		comreg = reg->OpenSubReg(L"DEVICEMAP\\SERIALCOMM");
		if (comreg)
		{
			UOSInt i;
			succ = true;
			i = 0;
			while (comreg->GetName(wbuff, i))
			{
				if (comreg->GetValueStr(wbuff, wbuff2))
				{
					if (Text::StrStartsWith(wbuff2, L"COM"))
					{
						ports->Add(Text::StrToUOSInt(&wbuff2[3]));
						if (portTypes)
						{
							if (Text::StrStartsWith(wbuff, L"\\Device\\com0com"))
							{
								portTypes->Add(SPT_COM0COM);
							}
							else if (Text::StrStartsWith(wbuff, L"\\Device\\Serial"))
							{
								portTypes->Add(SPT_SERIALPORT);
							}
							else if (Text::StrStartsWith(wbuff, L"\\Device\\DWSerial"))
							{
								portTypes->Add(SPT_DWSERIAL);
							}
							else if (Text::StrStartsWith(wbuff, L"\\Device\\BthModem"))
							{
								portTypes->Add(SPT_BLUETOOTH);
							}
							else if (Text::StrStartsWith(wbuff, L"\\Device\\USBSER"))
							{
								portTypes->Add(SPT_USBSERIAL);
							}
							else if (Text::StrStartsWith(wbuff, L"\\Device\\Silabser"))
							{
								portTypes->Add(SPT_USBSERIAL);
							}
							else if (Text::StrStartsWith(wbuff, L"\\Device\\QCUSB_COM"))
							{
								portTypes->Add(SPT_USBSERIAL);
							}
							else if (Text::StrStartsWith(wbuff, L"\\Device\\ProlificSerial"))
							{
								portTypes->Add(SPT_USBSERIAL);
							}
							else if (Text::StrStartsWith(wbuff, L"\\Device\\QUADPORTQuadSerial"))
							{
								portTypes->Add(SPT_USBSERIAL);
							}
							else
							{
								portTypes->Add(SPT_UNKNOWN);
							}
						}
					}
				}
				i++;
			}
			
			IO::Registry::CloseRegistry(comreg);
		}
		IO::Registry::CloseRegistry(reg);
	}
	return succ;
}

Text::CString IO::SerialPort::GetPortTypeName(SerialPortType portType)
{
	switch (portType)
	{
	case SPT_SERIALPORT:
		return CSTR("SerialPort");
	case SPT_BLUETOOTH:
		return CSTR("BT");
	case SPT_COM0COM:
		return CSTR("com0com");
	case SPT_DWSERIAL:
		return CSTR("DWSerialPort");
	case SPT_USBSERIAL:
		return CSTR("USBSerialPort");
	default:
	case SPT_UNKNOWN:
		return CSTR("Unknown");
	}
}

UOSInt IO::SerialPort::GetPortWithType(Text::CString portName)
{
	UOSInt port = 0;
	IO::Registry *reg;
	IO::Registry *comreg;
	WChar wbuff[512];
	WChar wbuff2[32];
	const WChar *wportName = Text::StrToWCharNew(portName.v);
	reg = IO::Registry::OpenLocalHardware();
	if (reg)
	{
		comreg = reg->OpenSubReg(L"DEVICEMAP\\SERIALCOMM");
		if (comreg)
		{
			UOSInt i;
			i = 0;
			while (comreg->GetName(wbuff, i))
			{
				if (comreg->GetValueStr(wbuff, wbuff2))
				{
					if (Text::StrIndexOf(wbuff, wportName) != INVALID_INDEX)
					{
						if (comreg->GetValueStr(wbuff, wbuff2))
						{
							if (Text::StrStartsWith(wbuff2, L"COM"))
							{
								port = Text::StrToUOSInt(&wbuff2[3]);
								break;
							}
						}
					}
				}
				i++;
			}
			
			IO::Registry::CloseRegistry(comreg);
		}
		IO::Registry::CloseRegistry(reg);
	}
	Text::StrDelNew(wportName);
	return port;
}

UOSInt IO::SerialPort::GetUSBPort()
{
	return GetPortWithType(CSTR("USBSER"));
}

UOSInt IO::SerialPort::GetBTPort()
{
	UOSInt port = 0;
	if (port == 0)
		port = GetPortWithType(CSTR("BthModem"));
	if (port == 0)
		port = GetPortWithType(CSTR("CsrBtSerial"));
	return port;
}

UTF8Char *IO::SerialPort::GetPortName(UTF8Char *buff, UOSInt portNum)
{
	return Text::StrUOSInt(Text::StrConcatC(buff, UTF8STRC("COM")), portNum);
}

Bool IO::SerialPort::ResetPort(UOSInt portNum)
{
	return false;
}

IO::SerialPort::SerialPort(UOSInt portNum, UInt32 baudRate, ParityType parity, Bool flowCtrl) : IO::Stream(CSTR("SerialPort"))
{
	this->handle = 0;
	this->reading = 0;
	this->portNum = portNum;
	this->baudRate = baudRate;
	this->parity = parity;
	this->flowCtrl = flowCtrl;
	this->InitStream();
}

IO::SerialPort::~SerialPort()
{
	ADDMESSAGE("Close\r\n");
	if (this->handle)
	{
		void *h = this->handle;
		this->handle = 0;
		CancelIo(h);
		CloseHandle(h);
	}
	
	ADDMESSAGE("Set event\r\n");
	this->rdEvt.Set();
	while (this->reading)
	{
		Sleep(10);
	}

}

Bool IO::SerialPort::IsDown() const
{
	if (this->handle == 0)
	{
		return true;
	}
	if (this->flowCtrl)
	{
		DWORD dwModemStatus;
		if (GetCommModemStatus(this->handle, &dwModemStatus))
		{
			return (MS_DSR_ON & dwModemStatus) == 0;
		}
	}
	return false;
}

UOSInt IO::SerialPort::Read(const Data::ByteArray &buff)
{
	UInt32 readCnt;
	BOOL ret;
	void *h;
	h = this->handle;
	if (h == 0)
		return 0;
	
	Sync::MutexUsage mutUsage(this->rdMut);
#ifdef _WIN32_WCE
	ret = ReadFile(h, buff, size, (DWORD*)&readCnt, 0);
#else
	OVERLAPPED ol;
	ol.hEvent = this->rdEvt.hand;
	ol.Internal = 0;
	ol.InternalHigh = 0;
	ol.Offset = 0;
	ol.OffsetHigh = 0;
	this->reading = true;
	ADDMESSAGE("Reading\r\n");
	ReadFile(h, buff.Ptr(), (DWORD)buff.GetSize(), (DWORD*)&readCnt, &ol);

	ADDMESSAGE("Waiting\r\n");
	this->rdEvt.Wait();
	if (this->handle == 0)
	{
		this->reading = false;
		mutUsage.EndUse();
		return 0;
	}

	ADDMESSAGE("Get Result\r\n");
	ret = GetOverlappedResult(h, &ol, (DWORD*)&readCnt, TRUE);
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

UOSInt IO::SerialPort::Write(const UInt8 *buff, UOSInt size)
{
	UOSInt writeCnt = 0;
	void *h = this->handle;
#ifdef _WIN32_WCE
	if (WriteFile(h, buff, size, (DWORD*)&writeCnt, 0))
	{
		return writeCnt;
	}
	else
	{
		return 0;
	}
#else
	if (h == 0)
		return 0;
	OVERLAPPED ol;
	ol.hEvent = CreateEvent(0, TRUE, FALSE, 0);
	ol.Internal = 0;
	ol.InternalHigh = 0;
	ol.Offset = 0;
	ol.OffsetHigh = 0;
	WriteFile(h, buff, (DWORD)size, (LPDWORD)&writeCnt, &ol);
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

void *IO::SerialPort::BeginRead(const Data::ByteArray &buff, Sync::Event *evt)
{
	void *h = this->handle;
	if (h == 0)
		return 0;

#ifdef _WIN32_WCE
	UInt32 readSize;
	if (ReadFile(h, buff.Ptr(), buff.GetSize(), (LPDWORD)&readSize, 0))
	{
		return (void*)(OSInt)readSize;
	}
	else
	{
		return 0;
	}
#else
	ReadEvent *re = MemAlloc(ReadEvent, 1);
	re->buff = buff.Ptr();
	re->evt = evt;
	re->size = buff.GetSize();
	re->ol.hEvent = evt->hand;
	re->ol.Internal = 0;
	re->ol.InternalHigh = 0;
	re->ol.Offset = 0;
	re->ol.OffsetHigh = 0;
	ReadFile(h, buff.Ptr(), (DWORD)buff.GetSize(), (DWORD*)&re->readSize, &re->ol);
	return re;
#endif
}

UOSInt IO::SerialPort::EndRead(void *reqData, Bool toWait, Bool *incomplete)
{
#ifdef _WIN32_WCE
	if (incomplete) *incomplete = false;
	return (UOSInt)reqData;
#else
	ReadEvent *re = (ReadEvent*)reqData;
	UInt32 retVal;
	Int32 result = GetOverlappedResult(this->handle, &re->ol, (DWORD*)&retVal, toWait?TRUE:FALSE);
	MemFree(re);
	if (incomplete) *incomplete = false;
	if (result)
		return retVal;
	return 0;
#endif
}

void IO::SerialPort::CancelRead(void *reqData)
{
#ifdef _WIN32_WCE
#else
//	ReadEvent *re = (ReadEvent*)reqData;
	CancelIo(this->handle);
	MemFree(reqData);
//	PurgeComm(this->handle, PURGE_RXABORT);
#endif
}

void *IO::SerialPort::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	evt->Set();
	if (handle == 0)
		return 0;
	return (void*)(OSInt)Write(buff, size);
}

UOSInt IO::SerialPort::EndWrite(void *reqData, Bool toWait)
{
	return (UOSInt)reqData;
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
	ADDMESSAGE("Close\r\n");
	if (this->handle)
	{
		void *h = this->handle;
		this->handle = 0;
		CancelIo(h);
		CloseHandle(h);
	}
	this->rdEvt.Set();
}

Bool IO::SerialPort::Recover()
{
	this->Close();
	this->InitStream();
	return true;
}

IO::StreamType IO::SerialPort::GetStreamType() const
{
	return IO::StreamType::SerialPort;
}

UOSInt IO::SerialPort::GetPortNum() const
{
	return this->portNum;
}

Bool IO::SerialPort::IsError() const
{
	return this->handle == 0;
}
