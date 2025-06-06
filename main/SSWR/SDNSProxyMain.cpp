#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Manage/ExceptionRecorder.h"
#include "SSWR/SDNSProxy/SDNSProxyCore.h"

Int32 MyMain(NN<Core::ProgControl> progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	NN<IO::ConfigFile> cfg;
	IO::ConsoleWriter *console;
	SSWR::SDNSProxy::SDNSProxyCore *core;

//	MemSetBreakPoint(0x014746E8);
	MemSetLogFile(UTF8STRCPTR("Memory.log"));
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder(CSTR("SDNSProxy.log"), Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASS(console, IO::ConsoleWriter());
	if (IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		NEW_CLASS(core, SSWR::SDNSProxy::SDNSProxyCore(cfg, console));
		if (!core->IsError())
		{
			core->Run(progCtrl);
		}
		DEL_CLASS(core);
		cfg.Delete();
	}
	else
	{
		console->WriteLine(CSTR("Error in opening config file"));
	}

	DEL_CLASS(console);
	DEL_CLASS(exHdlr);
	return 0;
}
