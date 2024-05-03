#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Sync/ThreadUtil.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Sync::SimpleThread::Sleep(10000);
	while (true)
	{
		console.WriteLine(CSTR("Console Test");
		Sync::SimpleThread::Sleep(1000);
	}
	return 0;
}

