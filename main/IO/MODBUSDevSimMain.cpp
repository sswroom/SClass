#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/ED516Sim.h"
#include "IO/ED527Sim.h"
#include "IO/ED538Sim.h"
#include "IO/ED588Sim.h"
#include "IO/IniFile.h"
#include "Net/MODBUSTCPListener.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/MODBUSDevSimHandler.h"
#include "Net/WebServer/WebListener.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg == 0)
	{
		console.WriteLineC(UTF8STRC("Config file not found"));
		return 1;
	}
	UInt16 modbusPort = 0;
	UInt16 ctrlPort = 0;
	UInt8 devAddr = 0;
	Text::String *s;
	if ((s = cfg->GetValue(CSTR("MODBUSPort"))) == 0 || !s->ToUInt16(&modbusPort))
	{
		console.WriteLineC(UTF8STRC("Config MODBUSPort not valid"));
		DEL_CLASS(cfg);
		return 1;
	}
	if ((s = cfg->GetValue(CSTR("CtrlPort"))) == 0 || !s->ToUInt16(&ctrlPort))
	{
		console.WriteLineC(UTF8STRC("Config CtrlPort not valid"));
		DEL_CLASS(cfg);
		return 1;
	}
	if ((s = cfg->GetValue(CSTR("DevAddr"))) == 0 || !s->ToUInt8(&devAddr))
	{
		console.WriteLineC(UTF8STRC("Config DevAddr not valid"));
		DEL_CLASS(cfg);
		return 1;
	}
	if ((s = cfg->GetValue(CSTR("DevType"))) == 0)
	{
		console.WriteLineC(UTF8STRC("Config DevType not found"));
		DEL_CLASS(cfg);
		return 1;
	}
	IO::MODBUSDevSim *dev = 0;
	if (s->Equals(UTF8STRC("ED516")))
	{
		NEW_CLASS(dev, IO::ED516Sim());
	}
	else if (s->Equals(UTF8STRC("ED527")))
	{
		NEW_CLASS(dev, IO::ED527Sim());
	}
	else if (s->Equals(UTF8STRC("ED538")))
	{
		NEW_CLASS(dev, IO::ED538Sim());
	}
	else if (s->Equals(UTF8STRC("ED588")))
	{
		NEW_CLASS(dev, IO::ED588Sim());
	}
	else
	{
		console.WriteLineC(UTF8STRC("Unknown DevType in config file"));
		DEL_CLASS(cfg);
		return 1;
	}
	DEL_CLASS(cfg);

	Int32 ret = 0;
	Net::OSSocketFactory sockf(false);
	IO::LogTool log;
	Net::MODBUSTCPListener modbusListener(&sockf, modbusPort, &log);
	if (modbusListener.IsError())
	{
		console.WriteLineC(UTF8STRC("Error in listening to MODBUSPort"));
		DEL_CLASS(dev);
		return 1;
	}
	else
	{
		modbusListener.AddDevice(devAddr, dev);
		Net::WebServer::MODBUSDevSimHandler devHdlr(dev);
		Net::WebServer::WebListener webListener(&sockf, 0, &devHdlr, ctrlPort, 120, 2, CSTR("MODBUSSim/1.0"), false, true);
		if (webListener.IsError())
		{
			console.WriteLineC(UTF8STRC("Error in listening to CtrlPort"));
			ret = 1;
		}
		else
		{
			console.WriteLineC(UTF8STRC("MODBUS Device Simulator started"));
			progCtrl->WaitForExit(progCtrl);
		}
	}
	return ret;
}