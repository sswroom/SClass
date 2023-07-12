#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/SerialPort.h"
#include "Sync/ThreadUtil.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::SerialPort serial(1, 9600, IO::SerialPort::PARITY_NONE, false);	
	UInt8 v = '0';
	Sync::SimpleThread::Sleep(10000);
	while (v <= '9')
	{
		serial.Write(&v, 1);
		Sync::SimpleThread::Sleep(1000);
		v++;
	}
	return 0;
}
