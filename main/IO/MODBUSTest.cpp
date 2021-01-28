#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Data/ByteTool.h"
#include "IO/MODBUSRTUMaster.h"
#include "IO/SerialPort.h"
#include "Sync/Thread.h"
#include <stdio.h>

void __stdcall ReadResult(void *userObj, UInt8 funcCode, const UInt8 *result, OSInt resultSize)
{
	if (resultSize == 4)
	{
		printf("Result = %f\r\n", ReadMFloat(result));
	}
}

void __stdcall SetResult(void *userObj, UInt8 funcCode, UInt16 startAddr, UInt16 count)
{
}

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::MODBUSMaster *modbus;
/*	NEW_CLASS(modbus, IO::MODBUSMaster(0));
	modbus->ReadHoldingRegisters(0x11, 0x6B, 3);
	printf("Correct: 11 03 00 6B 00 03 76 87\r\n");

	modbus->ReadInputs(0x1, 0, 1);
	printf("Correct: 01 02 00 00 00 01 B9 CA\r\n");*/

	IO::SerialPort *port;
	NEW_CLASS(port, IO::SerialPort(33, 2400, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		printf("Error in opeining serial port\r\n");
	}
	else
	{
		NEW_CLASS(modbus, IO::MODBUSRTUMaster(port));
		modbus->HandleReadResult(1, ReadResult, SetResult, 0);

		printf("Reading 0 register\r\n");
		modbus->ReadInputRegisters(1, 0, 2);
		Sync::Thread::Sleep(1000);
		printf("Reading 6 register\r\n");
		modbus->ReadInputRegisters(1, 6, 2);
		Sync::Thread::Sleep(1000);

		DEL_CLASS(modbus);
	}
	DEL_CLASS(port);
	return 0;
}
