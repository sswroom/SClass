#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Manage/ExceptionRecorder.h"
#include "Media/DrawEngineFactory.h"
#include "SSWR/SMonitor/SMonitorSvrCore.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("SMonitorSvrMem.log"));
	MemSetLogFile(sbuff, (UOSInt)(sptr - sbuff));
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("SMonitorSvr.err"));
	Manage::ExceptionRecorder exHdlr(CSTRP(sbuff, sptr), Manage::ExceptionRecorder::EA_RESTART);
	IO::ConsoleWriter console;
	{
		SSWR::SMonitor::SMonitorSvrCore core(console, Media::DrawEngineFactory::CreateDrawEngine());
		if (!core.IsError())
		{
			console.WriteLineC(UTF8STRC("SMonitorSvr running"));
			progCtrl->WaitForExit(progCtrl);
		}
	}
	return 0;
}
