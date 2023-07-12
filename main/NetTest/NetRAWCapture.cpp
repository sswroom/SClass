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

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	Net::RAWAnalyzer *analyzer;
	Net::SocketFactory *sockf;
	IO::Writer *console;
	UInt16 portNum = 8089;
	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], &portNum);
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
		console->WriteLineC(UTF8STRC("NetRAWCapture Started"));
		progCtrl->WaitForExit(progCtrl);
	}
	else
	{
		console->WriteLineC(UTF8STRC("Error in listening to the port"));
	}
	
	DEL_CLASS(analyzer);
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
