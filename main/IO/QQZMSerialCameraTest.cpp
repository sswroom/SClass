#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/MemoryStream.h"
#include "IO/SerialPort.h"
#include "IO/Device/QQZMSerialCamera.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter *console;
	UInt32 portNum = 1;
	UInt32 baudRate = 115200;

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUInt32(argv[1], portNum);
	}
	if (argc >= 3)
	{
		Text::StrToUInt32(argv[2], baudRate);
	}

	NEW_CLASS(console, IO::ConsoleWriter());
	IO::SerialPort *port;
	NEW_CLASS(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		console->WriteLine(CSTR("Error in opening the port"));
	}
	else
	{
		IO::Device::QQZMSerialCamera *camera;
		NEW_CLASS(camera, IO::Device::QQZMSerialCamera(port, 0, false));
		IO::MemoryStream mstm;
		if (camera->CapturePhoto(&mstm))
		{
			console->WriteLine(CSTR("Capture Image succeeded"));
		}
		else
		{
			console->WriteLine(CSTR("Error in capturing image"));
		}
		DEL_CLASS(camera);
	}
	DEL_CLASS(port);
	DEL_CLASS(console);
	return 0;
}
