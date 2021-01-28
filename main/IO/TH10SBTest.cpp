#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/MODBUSRTUMaster.h"
#include "IO/SerialPort.h"
#include "IO/Device/TH10SB.h"
#include "Sync/Thread.h"
#include <stdio.h>

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::MODBUSMaster *modbus;
	IO::SerialPort *port;
	IO::Device::TH10SB *th10sb;
	Int32 portNum = 33;
	Int32 baudRate = 9600;
	UInt8 addr = 1;
	OSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 3)
	{
		Text::StrToInt32(argv[1], &portNum);
		Text::StrToInt32(argv[2], &baudRate);
	}
	if (argc >= 4)
	{
		Text::StrToUInt8(argv[3], &addr);
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
		NEW_CLASS(modbus, IO::MODBUSRTUMaster(port));
		NEW_CLASS(th10sb, IO::Device::TH10SB(modbus, addr));

		if (th10sb->ReadTempRH(&temp, &rh))
		{
			printf("Temp = %lf, RH = %lf\r\n", temp, rh);
		}
		else
		{
			printf("Error in reading temp/rh\r\n");
		}

		if (th10sb->ReadBaudRate(&iVal))
		{
			printf("BaudRate = %d\r\n", iVal);
		}

		if (th10sb->ReadId(&iVal))
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
		DEL_CLASS(modbus);
	}
	DEL_CLASS(port);
	return 0;
}
