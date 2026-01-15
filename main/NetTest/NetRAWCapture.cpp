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

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Net::RAWAnalyzer *analyzer;
	IO::Writer *console;
	UInt16 portNum = 8089;
	UIntOS argc;
	UnsafeArray<UnsafeArray<UTF8Char>> argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		Text::StrToUInt16(argv[1], portNum);
	}

#if defined(DEBUGCON)
	NEW_CLASS(console, IO::DebugWriter());
#else
	NEW_CLASS(console, IO::ConsoleWriter());
#endif
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	NEW_CLASS(analyzer, Net::RAWAnalyzer(clif, portNum, console, Net::EthernetAnalyzer::AT_ALL));
	if (!analyzer->IsError())
	{
		console->WriteLine(CSTR("NetRAWCapture Started"));
		progCtrl->WaitForExit(progCtrl);
	}
	else
	{
		console->WriteLine(CSTR("Error in listening to the port"));
	}
	
	DEL_CLASS(analyzer);
	DEL_CLASS(console);
	return 0;
}
