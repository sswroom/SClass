#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/GSMModemController.h"
#include "IO/SerialPort.h"
#include "Test/TestModem.h"
#include "Text/StringBuilderUTF8.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Text::StringBuilderUTF8 sb;
	Int32 portNum = Test::TestModem::ListPorts(&console);
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
	console.WriteLine();
	sb.ClearStr();
	sb.Append((const UTF8Char*)"Trying Port ");
	sb.AppendI32(portNum);
	console.WriteLine(sb.ToString());

	IO::SerialPort *port;
	NEW_CLASS(port, IO::SerialPort(portNum, baudRate, IO::SerialPort::PARITY_NONE, true));
	if (port->IsError())
	{
		console.WriteLine((const UTF8Char*)"Error in opening the port");
	}
	else
	{
		IO::ATCommandChannel *channel;
		IO::GSMModemController *modem;
		NEW_CLASS(channel, IO::ATCommandChannel(port, false));
		NEW_CLASS(modem, IO::GSMModemController(channel, false));

		Test::TestModem::GSMModemTest(&console, modem, false);		

		DEL_CLASS(modem);
		DEL_CLASS(channel);
	}
	DEL_CLASS(port);
	return 0;
}
