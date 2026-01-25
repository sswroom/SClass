#include "Stdafx.h"
#include "DB/SQLiteFile.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/ServerMonitor/ServerMonitorCore.h"

SSWR::ServerMonitor::ServerMonitorCore::ServerMonitorCore()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(false));
	NEW_CLASSNN(this->clif, Net::TCPClientFactory(this->sockf));
	this->ssl = Net::SSLEngineFactory::Create(this->clif, false);
	this->db = nullptr;

	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::ReplaceExt(sbuff, UTF8STRC("cfg"));
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::File)
	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		fs.Write(CSTR("Port=8080\r\n").ToByteArray());
	}
	if (this->ssl.IsNull())
	{
		this->console.WriteLine(CSTR("SSL engine creation failed"));
	}
	else
	{
		NN<IO::ConfigFile> cfg;
		if (IO::IniFile::Parse(CSTRP(sbuff, sptr), 0).SetTo(cfg))
		{
			UInt16 port;
			NN<Text::String> s;
			sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
			if (cfg->GetValue(CSTR("DBFile")).SetTo(s))
			{
				sptr = IO::Path::AppendPath(sbuff, sptr, s->ToCString());
			}
			else
			{
				sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("ServerMonitor.sqlite"));
			}
			NN<DB::SQLiteFile> db;
			NEW_CLASSNN(db, DB::SQLiteFile(CSTRP(sbuff, sptr)));
			this->db = db;
			if (db->IsError())
			{
				this->console.WriteLine(CSTR("Error in opening database file"));
			}

			if (cfg->GetValue(CSTR("Port")).SetTo(s) && s->ToUInt16(port))
			{
			}
			cfg.Delete();
		}
		else
		{
			this->console.WriteLine(CSTR("Error in reading config file"));
		}
	}
}

SSWR::ServerMonitor::ServerMonitorCore::~ServerMonitorCore()
{
	this->ssl.Delete();
	this->clif.Delete();
	this->sockf.Delete();
	this->db.Delete();
}

Bool SSWR::ServerMonitor::ServerMonitorCore::IsError() const
{
	return this->ssl.IsNull() || this->db.IsNull();
}

Bool SSWR::ServerMonitor::ServerMonitorCore::Run()
{
	return true;
}