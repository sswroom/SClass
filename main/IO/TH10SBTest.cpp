#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/MODBUSRTUMaster.h"
#include "IO/SerialPort.h"
#include "IO/Device/TH10SB.h"
#include "Sync/ThreadUtil.h"
#include <stdio.h>

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::SerialPort *port;
	IO::Device::TH10SB *th10sb;
	UInt32 portNum = 33;
	UInt32 baudRate = 9600;
	UInt8 addr = 1;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);
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
		Int32 iVal;
		IO::MODBUSRTUMaster modbus(port);
		NEW_CLASS(th10sb, IO::Device::TH10SB(modbus, addr));

		if (th10sb->ReadTempRH(temp, rh))
		{
			printf("Temp = %lf, RH = %lf\r\n", temp, rh);
		}
		else
		{
			printf("Error in reading temp/rh\r\n");
		}

		if (th10sb->ReadBaudRate(iVal))
		{
			printf("BaudRate = %d\r\n", iVal);
		}

		if (th10sb->ReadId(iVal))
		{
			printf("Id = %d\r\n", iVal);
		}

/*		if (sht20->SetId(2))
		{
			printf("Id Set to 2\r\n");
		}
		else
		{
			printf("Error in setting Id\r\n");
		}*/

		DEL_CLASS(th10sb);
	}
	DEL_CLASS(port);
	return 0;
}
