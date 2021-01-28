#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/SerialPort.h"
#include "Sync/Thread.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::SerialPort serial(1, 9600, IO::SerialPort::PARITY_NONE, false);	
	UInt8 v = '0';
	Sync::Thread::Sleep(10000);
	while (v <= '9')
	{
		serial.Write(&v, 1);
		Sync::Thread::Sleep(1000);
		v++;
	}
	return 0;
}
