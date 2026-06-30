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
	NN<Manage::ExceptionRecorder> exHdlr;
	NN<IO::ConfigFile> cfg;
	NN<IO::ConsoleWriter> console;
	NN<SSWR::SDNSProxy::SDNSProxyCore> core;

//	MemSetBreakPoint(0x014746E8);
	MemSetLogFile(UTF8STRCPTR("Memory.log"));
	NEW_CLASSNN(exHdlr, Manage::ExceptionRecorder(CSTR("SDNSProxy.log"), Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASSNN(console, IO::ConsoleWriter());
	if (IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		NEW_CLASSNN(core, SSWR::SDNSProxy::SDNSProxyCore(cfg, console));
		if (!core->IsError())
		{
			core->Run(progCtrl);
		}
		core.Delete();
		cfg.Delete();
	}
	else
	{
		console->WriteLine(CSTR("Error in opening config file"));
	}

	console.Delete();
	exHdlr.Delete();
	return 0;
}
