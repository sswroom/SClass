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

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg == 0)
	{
		console.WriteLine(CSTR("Config file not found"));
		return 1;
	}
	UInt16 modbusPort = 0;
	UInt16 ctrlPort = 0;
	UInt8 devAddr = 0;
	NN<Text::String> s;
	if (!cfg->GetValue(CSTR("MODBUSPort")).SetTo(s) || !s->ToUInt16(modbusPort))
	{
		console.WriteLine(CSTR("Config MODBUSPort not valid"));
		DEL_CLASS(cfg);
		return 1;
	}
	if (!cfg->GetValue(CSTR("CtrlPort")).SetTo(s) || !s->ToUInt16(ctrlPort))
	{
		console.WriteLine(CSTR("Config CtrlPort not valid"));
		DEL_CLASS(cfg);
		return 1;
	}
	if (!cfg->GetValue(CSTR("DevAddr")).SetTo(s) || !s->ToUInt8(devAddr))
	{
		console.WriteLine(CSTR("Config DevAddr not valid"));
		DEL_CLASS(cfg);
		return 1;
	}
	if (!cfg->GetValue(CSTR("DevType")).SetTo(s))
	{
		console.WriteLine(CSTR("Config DevType not found"));
		DEL_CLASS(cfg);
		return 1;
	}
	NN<IO::MODBUSDevSim> dev;
	if (s->Equals(UTF8STRC("ED516")))
	{
		NEW_CLASSNN(dev, IO::ED516Sim());
	}
	else if (s->Equals(UTF8STRC("ED527")))
	{
		NEW_CLASSNN(dev, IO::ED527Sim());
	}
	else if (s->Equals(UTF8STRC("ED538")))
	{
		NEW_CLASSNN(dev, IO::ED538Sim());
	}
	else if (s->Equals(UTF8STRC("ED588")))
	{
		NEW_CLASSNN(dev, IO::ED588Sim());
	}
	else
	{
		console.WriteLine(CSTR("Unknown DevType in config file"));
		DEL_CLASS(cfg);
		return 1;
	}
	DEL_CLASS(cfg);

	Int32 ret = 0;
	Net::OSSocketFactory sockf(false);
	IO::LogTool log;
	Net::MODBUSTCPListener modbusListener(sockf, modbusPort, log, false);
	if (modbusListener.IsError())
	{
		console.WriteLine(CSTR("Error in listening to MODBUSPort"));
		dev.Delete();
		return 1;
	}
	else
	{
		modbusListener.AddDevice(devAddr, dev);
		Net::WebServer::MODBUSDevSimHandler devHdlr(modbusListener, dev);
		Net::WebServer::WebListener webListener(sockf, 0, devHdlr, ctrlPort, 120, 1, 2, CSTR("MODBUSSim/1.0"), false, Net::WebServer::KeepAlive::Default, false);
		if (webListener.IsError())
		{
			console.WriteLine(CSTR("Error in listening to CtrlPort"));
			ret = 1;
		}
		else if (!modbusListener.Start())
		{
			console.WriteLine(CSTR("Error in starting MODBUSPort"));
		}
		else if (!webListener.Start())
		{
			console.WriteLine(CSTR("Error in starting CtrlPort"));
		}
		else
		{
			console.WriteLine(CSTR("MODBUS Device Simulator started"));
			progCtrl->WaitForExit(progCtrl);
		}
	}
	return ret;
}