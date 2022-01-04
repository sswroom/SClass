#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Manage/ExceptionRecorder.h"
#include "Media/DrawEngineFactory.h"
#include "SSWR/SMonitor/SMonitorSvrCore.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	SSWR::SMonitor::SMonitorSvrCore *core;
	Manage::ExceptionRecorder *exHdlr;
	UTF8Char sbuff[512];
	IO::ConsoleWriter *console;

	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"SMonitorSvrMem.log");
	MemSetLogFile(sbuff);
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, (const UTF8Char*)"SMonitorSvr.err");
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(sbuff, Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(core, SSWR::SMonitor::SMonitorSvrCore(console, Media::DrawEngineFactory::CreateDrawEngine()));
	if (!core->IsError())
	{
		console->WriteLine((const UTF8Char*)"SMonitorSvr running");
		progCtrl->WaitForExit(progCtrl);
	}
	DEL_CLASS(core);
	DEL_CLASS(console);
	DEL_CLASS(exHdlr);
	return 0;
}
