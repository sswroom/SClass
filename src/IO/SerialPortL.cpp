#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/Stream.h"
#include "IO/SerialPort.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/MyString.h"
#include "Text/UTF8Reader.h"
#include <errno.h>
#include <fcntl.h>
#if defined(__linux__) && (defined(__GLIBC__) || defined(__UCLIBC__))
#include <termio.h>
#else
#include <termios.h>
#include <sys/ioctl.h>
#endif
#include <unistd.h>
#include <sys/file.h>

#include <stdio.h>

#define ADDMESSAGE(msg)

Bool IO::SerialPort::InitStream()
{
	UTF8Char portName[16];
	if (portNum <= 0)
		return false;
	if (portNum <= 32)
	{
		Text::StrUOSInt(Text::StrConcatC(portName, UTF8STRC("/dev/ttyS")), portNum - 1);
	}
	else if (portNum <= 64)
	{
		Text::StrUOSInt(Text::StrConcatC(portName, UTF8STRC("/dev/ttyUSB")), portNum - 33);
	}
	else if (portNum <= 96)
	{
		Text::StrUOSInt(Text::StrConcatC(portName, UTF8STRC("/dev/ttyACM")), portNum - 65);
	}
	else if (portNum <= 128)
	{
		Text::StrUOSInt(Text::StrConcatC(portName, UTF8STRC("/dev/rfcomm")), portNum - 97);
	}
	else
	{
		return false;
	}

	Int32 h = open((Char*)portName, O_RDWR | O_NOCTTY | O_NDELAY);
	this->handle = (void*)(OSInt)h;
	if (h < 0)
	{
		this->handle = 0;
		return false;
	}

	tcflush(h, TCIOFLUSH);
#if defined(FNDELAY)
	fcntl(h, F_SETFL, FNDELAY);
#endif
	struct termios options;
	options.c_cflag &= (tcflag_t)~(PARENB | CSTOPB | CSIZE);
	options.c_cflag |= (tcflag_t)~CS8;
	if (tcgetattr(h, &options) == -1)
	{
		close(h);
		this->handle = 0;
		return false;
	}
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag &= (tcflag_t)~(ICANON | ECHO | ECHOE | ISIG);
	options.c_iflag &= (tcflag_t)~(IXON | IXOFF | IXANY | INLCR | ICRNL);
	options.c_oflag &= (tcflag_t)~OPOST;
	if (flowCtrl)
	{
		options.c_cflag |= CRTSCTS;
	}

	switch (parity)
	{
	case PARITY_ODD:
		options.c_cflag |= PARODD | PARENB;
		options.c_iflag |= INPCK;
		break;
	case PARITY_EVEN:
		options.c_cflag |= PARENB;
		options.c_cflag &= (tcflag_t)~PARODD;
		options.c_iflag |= INPCK;
		break;
	case PARITY_NONE:
		options.c_cflag &= (tcflag_t)~PARENB;
		options.c_iflag &= (tcflag_t)~INPCK;
		break;
	}

	UInt32 rateId;
	switch (baudRate)
	{
	case 50:
		rateId = B50;
		break;
	case 75:
		rateId = B75;
		break;
	case 110:
		rateId = B110;
		break;
	case 134:
		rateId = B134;
		break;
	case 150:
		rateId = B150;
		break;
	case 200:
		rateId = B200;
		break;
	case 300:
		rateId = B300;
		break;
	case 600:
		rateId = B600;
		break;
	case 1200:
		rateId = B1200;
		break;
	case 1800:
		rateId = B1800;
		break;
	case 2400:
		rateId = B2400;
		break;
	case 4800:
		rateId = B4800;
		break;
	case 9600:
		rateId = B9600;
		break;
	case 19200:
		rateId = B19200;
		break;
	case 38400:
		rateId = B38400;
		break;
	case 57600:
		rateId = B57600;
		break;
	case 115200:
		rateId = B115200;
		break;
	case 230400:
		rateId = B230400;
		break;
#ifdef B460800
	case 460800:
		rateId = B460800;
		break;
#endif
	default:
		rateId = B0;
		break;
	}
	cfsetispeed(&options, rateId);
	cfsetospeed(&options, rateId);
	if (tcsetattr(h, TCSANOW, &options) == -1)
	{
		close(h);
		this->handle = 0;
	}
	else
	{
		flock(h, LOCK_EX);
	}
	return true;
}

Bool IO::SerialPort::GetAvailablePorts(NN<Data::ArrayList<UOSInt>> ports, Data::ArrayList<SerialPortType> *portTypes)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	IO::Path::FindFileSession *sess;
	sess = IO::Path::FindFile(CSTR("/dev/tty*"));
	if (sess)
	{
		while ((sptr = IO::Path::FindNextFile(sbuff, sess, 0, 0, 0)) != 0)
		{
			if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ttyS")))
			{
				ports->Add(1 + Text::StrToUOSInt(&sbuff[4]));
				if (portTypes)
					portTypes->Add(SPT_SERIALPORT);
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ttyUSB")))
			{
				ports->Add(33 + Text::StrToUOSInt(&sbuff[6]));
				if (portTypes)
					portTypes->Add(SPT_USBSERIAL);
			}
			else if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ttyACM")))
			{
				ports->Add(65 + Text::StrToUOSInt(&sbuff[6]));
				if (portTypes)
					portTypes->Add(SPT_USBSERIAL);
			}
		}
		IO::Path::FindFileClose(sess);
	}
	sess = IO::Path::FindFile(CSTR("/dev/rfcomm*"));
	if (sess)
	{
		while ((sptr = IO::Path::FindNextFile(sbuff, sess, 0, 0, 0)) != 0)
		{
			ports->Add(97 + Text::StrToUOSInt(&sbuff[6]));
			if (portTypes)
				portTypes->Add(SPT_BLUETOOTH);
		}
		IO::Path::FindFileClose(sess);
	}
	return true;
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
	return 0;
}

UOSInt IO::SerialPort::GetUSBPort()
{
	if (IO::Path::GetPathType(CSTR("/dev/ttyUSB0")) == IO::Path::PathType::File)
		return 33;
	else if (IO::Path::GetPathType(CSTR("/dev/ttyACM0")) == IO::Path::PathType::File)
		return 65;
	else
		return 0;
}

UOSInt IO::SerialPort::GetBTPort()
{
	return 0;
}

UTF8Char *IO::SerialPort::GetPortName(UTF8Char *buff, UOSInt portNum)
{
	if (portNum <= 0)
		return 0;
	if (portNum <= 32)
	{
		return Text::StrUOSInt(Text::StrConcatC(buff, UTF8STRC("/dev/ttyS")), portNum - 1);
	}
	else if (portNum <= 64)
	{
		return Text::StrUOSInt(Text::StrConcatC(buff, UTF8STRC("/dev/ttyUSB")), portNum - 33);
	}
	else if (portNum <= 96)
	{
		return Text::StrUOSInt(Text::StrConcatC(buff, UTF8STRC("/dev/ttyACM")), portNum - 65);
	}
	else if (portNum <= 128)
	{
		return Text::StrUOSInt(Text::StrConcatC(buff, UTF8STRC("/dev/rfcomm")), portNum - 97);
	}
	else
	{
		return 0;
	}
}

Bool SerialPort_WriteInt32(Text::CStringNN path, Int32 num)
{
	UTF8Char sbuff[32];
	Bool ret = false;
	UTF8Char *sptr;
	IO::FileStream fs(path, IO::FileMode::CreateWrite, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		sptr = Text::StrInt32(sbuff, num);
		if (fs.Write(sbuff, (UOSInt)(sptr - sbuff)) == (UOSInt)(sptr - sbuff))
		{
			ret = true;
		}
	}
	return ret;
}

Bool IO::SerialPort::ResetPort(UOSInt portNum)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UOSInt i;
	OSInt si;
	UTF8Char *sptr;
	if (portNum <= 32)
	{
		return false;
	}
	else if (portNum <= 64)
	{
		sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("/sys/bus/usb-serial/devices/ttyUSB")), portNum - 33);
		if ((si = readlink((const Char*)sbuff, (Char*)sbuff2, 511)) <= 0)
		{
			return false;
		}
		sbuff2[si] = 0;
		sptr = IO::Path::AppendPath(sbuff, sptr, Text::CStringNN(sbuff2, (UOSInt)si));
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '/');
		if (i == INVALID_INDEX)
			return false;
		sbuff[i] = 0;
		i = Text::StrLastIndexOfCharC(sbuff, i, '/');
		if (i == INVALID_INDEX)
			return false;
		sptr = &sbuff[i + 1];
		sptr = Text::StrConcatC(sptr, UTF8STRC("authorized"));
		if (SerialPort_WriteInt32(CSTRP(sbuff, sptr), 0) && SerialPort_WriteInt32(CSTRP(sbuff, sptr), 1))
		{
			return true;
		}
		return false;
	}
	else
	{
		return false;
	}
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
		OSInt h = (OSInt)this->handle;
		this->handle = 0;
		flock((int)h, LOCK_UN);
		close((int)h);
	}

	ADDMESSAGE("Set event\r\n");
	this->rdEvt.Set();
	while (this->reading)
	{
		Sync::SimpleThread::Sleep(10);
	}
}

Bool IO::SerialPort::IsDown() const
{
	if (this->IsError())
	{
		return true;
	}
	if (this->flowCtrl)
	{
		int status;
		if (ioctl((int)(OSInt)this->handle, TIOCMGET, &status) == 0)
		{
			return (status & TIOCM_DSR) == 0;
		}
	}
	return false;
}

UOSInt IO::SerialPort::Read(const Data::ByteArray &buff)
{
	OSInt readCnt;
	OSInt h;
	h = (OSInt)this->handle;
	if (h == 0)
	{
//		wprintf(L"h == 0\n");
		return 0;
	}

//	wprintf(L"Begin read\n");
	Sync::MutexUsage mutUsage(this->rdMut);
	this->reading = true;
	readCnt = 0;
	while (true)
	{
		if (this->handle == 0)
			break;
		readCnt = read((int)h, buff.GetPtr(), buff.GetSize());
		if (readCnt == -1)
		{
			if (errno == EAGAIN)
			{
				readCnt = 0;
			}
			else
			{
//				wprintf(L"Read error: %d\n", errno);
				readCnt = 0;
				break;
			}
		}
		if (readCnt > 0)
			break;
		this->rdEvt.Wait(10);
	}
	this->reading = false;
	mutUsage.EndUse();
//	wprintf(L"End read, cnt = %d\n", readCnt);
	return (UOSInt)readCnt;
}

UOSInt IO::SerialPort::Write(const UInt8 *buff, UOSInt size)
{
	OSInt writeCnt;
	OSInt h = (OSInt)this->handle;
	writeCnt = write((int)h, buff, size);
	fsync((int)h);
	return (UOSInt)writeCnt;
}

void *IO::SerialPort::BeginRead(const Data::ByteArray &buff, Sync::Event *evt)
{
	return 0;
}

UOSInt IO::SerialPort::EndRead(void *reqData, Bool toWait, OutParam<Bool> incomplete)
{
	incomplete.Set(false);
	return 0;
}

void IO::SerialPort::CancelRead(void *reqData)
{
}

void *IO::SerialPort::BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt)
{
	evt->Set();
	if (this->handle == 0)
		return 0;
	return (void*)Write(buff, size);
}

UOSInt IO::SerialPort::EndWrite(void *reqData, Bool toWait)
{
	OSInt h = (OSInt)this->handle;
	fsync((int)h);
	return (UOSInt)reqData;
}

void IO::SerialPort::CancelWrite(void *reqData)
{
}

Int32 IO::SerialPort::Flush()
{
	tcflush((int)(OSInt)this->handle, TCIOFLUSH);
	return 0;
}

void IO::SerialPort::Close()
{
	ADDMESSAGE("Close\r\n");
	if (this->handle)
	{
		OSInt h = (OSInt)this->handle;
		this->handle = 0;
		flock((int)h, LOCK_UN);
		close((int)h);
	}
	this->rdEvt.Set();
}

Bool IO::SerialPort::Recover()
{
	this->Close();
	IO::SerialPort::ResetPort(this->portNum);
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
