#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GSMModemController.h"
#include "IO/SerialPort.h"
#include "Test/TestModem.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	UIntOS portNum = Test::TestModem::ListPorts(console);
	UInt32 baudRate = 115200;

	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUIntOS(argv[1], portNum);
	}
	if (argc >= 3)
	{
		Text::StrToUInt32(argv[2], baudRate);
	}
	console.WriteLine();
	sb.ClearStr();
	sb.AppendC(UTF8STRC("Trying Port "));
	sb.AppendUIntOS(portNum);
	console.WriteLine(sb.ToCString());

	IO::SerialPort port(portNum, baudRate, IO::SerialPort::PARITY_NONE, true);
	if (port.IsError())
	{
		console.WriteLine(CSTR("Error in opening the port"));
	}
	else
	{
		IO::ATCommandChannel channel(port, false);
		IO::GSMModemController modem(channel, false);

		Test::TestModem::GSMModemTest(console, modem, false);		
	}
	return 0;
}
