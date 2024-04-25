#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/I2C.h"
#include "IO/RS232GPIO.h"
#include "IO/SerialPort.h"
#include "Manage/HiResClock.h"
#include "Sync/ThreadUtil.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::SerialPort serial(1, 115200, IO::SerialPort::PARITY_NONE, false);
	IO::GPIOControl gpio;
	IO::RS232GPIO modem(&gpio, 0, 22, 19200);
	OSInt readSize;
	OSInt i;
	UInt8 buff[128];
	while (true)
	{
		if (serial.HasData())
		{
			readSize = serial.Read(buff, 128);
			modem.Write(buff, readSize);
		}
		if (modem.HasData())
		{
			readSize = modem.Read(buff, 128);
			serial.Write(buff, readSize);
		}
	}
	return 0;
}
