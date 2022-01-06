#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Sync/Thread.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	Sync::Thread::Sleep(10000);
	while (true)
	{
		console.WriteLineC(UTF8STRC("Console Test");
		Sync::Thread::Sleep(1000);
	}
	return 0;
}

