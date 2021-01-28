#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/MemoryStream.h"
#include "IO/SerialPort.h"
#include "IO/Device/QQZMSerialCamera.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter *console;
	Int32 portNum = 1;
	Int32 baudRate = 115200;

	OSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToInt32(argv[1], &portNum);
	}
	if (argc >= 3)
	{
		Text::StrToInt32(argv[2], &baudRate);
	}

	NEW_CLASS(console, IO::ConsoleWriter());
	IO::SerialPort *port;
	NEW_CLASS(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, false));
	if (port->IsError())
	{
		console->WriteLine((const UTF8Char*)"Error in opening the port");
	}
	else
	{
		IO::MemoryStream *mstm;
		IO::Device::QQZMSerialCamera *camera;
		NEW_CLASS(camera, IO::Device::QQZMSerialCamera(port, 0, false));
		NEW_CLASS(mstm, IO::MemoryStream((const UTF8Char*)"QQZMSerialCameraTest.mstm"));
		if (camera->CapturePhoto(mstm))
		{
			console->WriteLine((const UTF8Char*)"Capture Image succeeded");
		}
		else
		{
			console->WriteLine((const UTF8Char*)"Error in capturing image");
		}
		DEL_CLASS(mstm);
		DEL_CLASS(camera);
	}
	DEL_CLASS(port);
	DEL_CLASS(console);
	return 0;
}
