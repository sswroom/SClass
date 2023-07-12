#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Sync/ThreadUtil.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	Sync::SimpleThread::Sleep(10000);
	while (true)
	{
		console.WriteLineC(UTF8STRC("Console Test");
		Sync::SimpleThread::Sleep(1000);
	}
	return 0;
}

