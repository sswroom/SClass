#include "Stdafx.h"
#include "Core/Core.h"
#include "Core/DefaultDrawEngine.h"
#include "DB/DBTool.h"
#include "DB/MDBFile.h"
#include "DB/ODBCConn.h"
#include "IO/ConsoleWriter.h"
#include "IO/IniFile.h"
#include "Manage/ExceptionRecorder.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "SSWR/OrganMgr/OrganWebHandler.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Manage::ExceptionRecorder *exHdlr;
	DB::DBTool *db;
	IO::ConsoleWriter *console;
	Int32 scnSize = 0;
	Int32 unorganizedGroupId = 0;
	SSWR::OrganMgr::OrganWebHandler *dataHdlr;
	Net::SocketFactory *sockf;
	IO::LogTool *log;
	const UTF8Char *csptr;
	
//	MemSetBreakPoint(0x4ab9e78);
	NEW_CLASS(exHdlr, Manage::ExceptionRecorder((const UTF8Char*)"Error.txt", Manage::ExceptionRecorder::EA_RESTART));
	NEW_CLASS(console, IO::ConsoleWriter());
	dataHdlr = 0;
	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	NEW_CLASS(log, IO::LogTool());

	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg == 0)
	{
		console->WriteLine((const UTF8Char*)"Error in loading config file");
	}
	else
	{
		if ((csptr = cfg->GetValue((const UTF8Char*)"ScreenSize")) != 0)
		{
			scnSize = Text::StrToInt32(csptr);
		}
		if (scnSize <= 0)
		{
			scnSize = 1800;
		}
		if ((csptr = cfg->GetValue((const UTF8Char *)"Unorganized")) != 0)
		{
			Text::StrToInt32(csptr, &unorganizedGroupId);
		}
		if (cfg->GetValue((const UTF8Char*)"MDBFile"))
		{
			db = DB::MDBFile::CreateDBTool(cfg->GetValue((const UTF8Char*)"MDBFile"), log, false, (const UTF8Char*)"DB: ");
		}
		else if (cfg->GetValue((const UTF8Char*)"MySQLServer"))
		{
			db = Net::MySQLTCPClient::CreateDBTool(sockf, cfg->GetValue((const UTF8Char*)"MySQLServer"), cfg->GetValue((const UTF8Char*)"MySQLDB"), cfg->GetValue((const UTF8Char*)"MySQLUID"), cfg->GetValue((const UTF8Char*)"MySQLPwd"), log, false, (const UTF8Char*)"DB: ");
		}
		else
		{
			db = DB::ODBCConn::CreateDBTool(cfg->GetValue((const UTF8Char*)"DBDSN"), cfg->GetValue((const UTF8Char*)"DBUID"), cfg->GetValue((const UTF8Char*)"DBPwd"), cfg->GetValue((const UTF8Char*)"DBSchema"), log, false, (const UTF8Char*)"DB: ");
		}
		NEW_CLASS(dataHdlr, SSWR::OrganMgr::OrganWebHandler(sockf, log, db, cfg->GetValue((const UTF8Char*)"ImageDir"), Text::StrToInt32(cfg->GetValue((const UTF8Char*)"SvrPort")), cfg->GetValue((const UTF8Char*)"CacheDir"), cfg->GetValue((const UTF8Char*)"DataDir"), scnSize, cfg->GetValue((const UTF8Char*)"ReloadPwd"), unorganizedGroupId, Core::DefaultDrawEngine::CreateDrawEngine()));
		DEL_CLASS(cfg);

		if (dataHdlr->IsError())
		{
			console->WriteLine((const UTF8Char*)"Error in starting server");
		}
		else
		{
			console->WriteLine((const UTF8Char*)"SOrganWeb started");
			progCtrl->WaitForExit(progCtrl);
		}

		DEL_CLASS(dataHdlr);
	}

	DEL_CLASS(sockf);
	DEL_CLASS(log);

	DEL_CLASS(console);
	DEL_CLASS(exHdlr);
	return 0;
}
