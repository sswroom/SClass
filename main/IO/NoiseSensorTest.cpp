#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Core/ByteTool_C.h"
#include "IO/SerialPort.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include <stdio.h>

IO::SerialPort *port;
Bool threadRunning;
Bool threadToStop;

UInt32 __stdcall ReadThread(AnyType userObj)
{
	UInt8 readBuff[1024];
	UOSInt buffSize = 0;
	UOSInt readSize;
	UOSInt i;
	threadRunning = true;
	while (!threadToStop)
	{
		readSize = port->Read(Data::ByteArray(&readBuff[buffSize], 1024 - buffSize));
		if (readSize > 0)
		{
			buffSize += readSize;
			i = 0;
			while (i < buffSize - 5)
			{
				if (readBuff[i] == 0xBB && readBuff[i + 1] == 0xAA)
				{
					printf("Value1 = %d, Value2 = %d\r\n", ReadMUInt16(&readBuff[i + 2]), ReadMUInt16(&readBuff[i + 4]));
					i += 6;
				}
				else
				{
					i++;
				}
			}

			if (i >= buffSize)
			{
				buffSize = 0;
			}
			else if (i > 0)
			{
				MemCopyO(readBuff, &readBuff[i], buffSize - i);
				buffSize -= i;
			}
		}
	}
	threadRunning = false;
	return 0;
}

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	NEW_CLASS(port, IO::SerialPort(33, 115200, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		printf("Error in opeining serial port\r\n");
	}
	else
	{
		threadToStop = false;
		threadRunning = false;
		Sync::ThreadUtil::Create(ReadThread, 0);
		progCtrl->WaitForExit(progCtrl);
		threadToStop = true;
		port->Close();
		while (threadRunning)
		{
			Sync::SimpleThread::Sleep(10);
		}
	}
	DEL_CLASS(port);
	return 0;
}
