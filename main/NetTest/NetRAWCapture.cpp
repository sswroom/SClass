#include "Stdafx.h"
#include "Core/Core.h"
#if defined(DEBUGCON)
#include "IO/DebugWriter.h"
#else
#include "IO/ConsoleWriter.h"
#endif
#include "IO/MemoryStream.h"
#include "Net/OSSocketFactory.h"
#include "Net/RAWAnalyzer.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Net::RAWAnalyzer *analyzer;
	Net::SocketFactory *sockf;
	IO::IWriter *console;
	Int32 portNum = 8089;
	OSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToInt32(argv[1], &portNum);
	}

#if defined(DEBUGCON)
	NEW_CLASS(console, IO::DebugWriter());
#else
	NEW_CLASS(console, IO::ConsoleWriter());
#endif
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(analyzer, Net::RAWAnalyzer(sockf, portNum, console, Net::EthernetAnalyzer::AT_ALL));
	if (!analyzer->IsError())
	{
		console->WriteLine((const UTF8Char*)"NetRAWCapture Started");
		progCtrl->WaitForExit(progCtrl);
	}
	else
	{
		console->WriteLine((const UTF8Char*)"Error in listening to the port");
	}
	
	DEL_CLASS(analyzer);
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
