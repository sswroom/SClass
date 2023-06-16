#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/SerialPort.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::SerialPort serial(1, 115200, IO::SerialPort::PARITY_NONE, false);	
	UInt8 buff[64];
	UTF8Char *sptr;
	OSInt readSize;
	UInt16 tmpVal = (UInt16)(((F_CPU / 8 / 115200) - 1) >> 1);
	sptr = Text::StrConcatC(buff, UTF8STRC("UBBR0 = "));
	sptr = Text::StrUInt16(sptr, tmpVal);
	*sptr++ = 13;
	*sptr++ = 10;
	Sync::SimpleThread::Sleep(10000);
	serial.Write(buff, sptr - buff);

	while (true)
	{
		readSize = serial.Read(buff, 64);
		serial.Write(buff, readSize);
	}
	return 0;
}
