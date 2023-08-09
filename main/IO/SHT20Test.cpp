#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/MODBUSRTUMaster.h"
#include "IO/SerialPort.h"
#include "IO/Device/SHT20.h"
#include "Sync/ThreadUtil.h"
#include <stdio.h>

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::MODBUSMaster *modbus;
	IO::SerialPort *port;
	IO::Device::SHT20 *sht20;
	UInt32 portNum = 33;
	UInt32 baudRate = 9600;
	UInt8 addr = 1;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 3)
	{
		Text::StrToUInt32(argv[1], portNum);
		Text::StrToUInt32(argv[2], baudRate);
	}
	if (argc >= 4)
	{
		Text::StrToUInt8(argv[3], addr);
	}

	NEW_CLASS(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		printf("Error in opeining serial port\r\n");
	}
	else
	{
		Double temp;
		Double rh;
		NEW_CLASS(modbus, IO::MODBUSRTUMaster(port));
		NEW_CLASS(sht20, IO::Device::SHT20(modbus, addr));

		if (sht20->ReadTempRH(&temp, &rh))
		{
			printf("Temp = %lf, RH = %lf\r\n", temp, rh);
		}
		else
		{
			printf("Error in reading temp/rh\r\n");
		}

/*		if (sht20->ReadBaudRate(&iVal))
		{
			printf("BaudRate = %d\r\n", iVal);
		}

		if (sht20->ReadParity(&iVal))
		{
			printf("Parity = %d\r\n", iVal);
		}

		if (sht20->ReadId(&iVal))
		{
			printf("Id = %d\r\n", iVal);
		}

		if (sht20->ReadIdValid(&iVal))
		{
			printf("IdValid = %d\r\n", iVal);
		}

		if (sht20->SetId(2))
		{
			printf("Id Set to 2\r\n");
		}
		else
		{
			printf("Error in setting Id\r\n");
		}*/

		DEL_CLASS(sht20);
		DEL_CLASS(modbus);
	}
	DEL_CLASS(port);
	return 0;
}
