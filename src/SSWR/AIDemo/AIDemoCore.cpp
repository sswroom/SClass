#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/Sort/ArtificialQuickSort_C.h"
#include "DB/DBConfig.h"
#include "DB/SharedReadingDB.h"
#include "IO/ConsoleLogHandler.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileUtil.h"
#include "IO/IniFile.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AIDemo/AIDemoCore.h"
#include "Sync/ThreadUtil.h"

SSWR::AIDemo::AIDemoCore::AIDemoCore(NN<Core::ProgControl> progCtrl) : sockf(true), clif(sockf)
{
	this->progCtrl = progCtrl;
	this->ssl = Net::SSLEngineFactory::Create(this->clif, false);
	NEW_CLASSNN(this->logHdlr, IO::ConsoleLogHandler(this->console));
	this->listener = nullptr;
	this->hdlr = nullptr;
	this->aiClient = nullptr;
	this->dbDDL = nullptr;
	this->dbRule = nullptr;
	this->db = nullptr;

	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	NN<Text::String> s2;
	NN<IO::ConfigFile> cfg;
	UInt16 port = 0;
	if (!IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		this->console.WriteLine(CSTR("Error in opening config file"));
		return;
	}
	if (cfg->GetValue(CSTR("NoConsole")).SetTo(s) && s->ToInt32() != 0)
	{
	}
	else
	{
		this->log.AddLogHandler(this->logHdlr, IO::LogHandler::LogLevel::Raw);
	}
	if (!cfg->GetValue(CSTR("Port")).SetTo(s))
	{
		this->log.LogMessage(CSTR("Config Port not found"), IO::LogHandler::LogLevel::Error);
	}
	else if (!s->ToUInt16(port))
	{
		this->log.LogMessage(CSTR("Config Port not valid"), IO::LogHandler::LogLevel::Error);
	}
	else if (port == 0)
	{
		this->log.LogMessage(CSTR("Config Port cannot be 0"), IO::LogHandler::LogLevel::Error);
	}
	if (cfg->GetValue(CSTR("OpenAIURL")).SetTo(s) && cfg->GetValue(CSTR("OpenAIKey")).SetTo(s2))
	{
		NN<Net::OpenAIClient> cli;
		NEW_CLASSNN(cli, Net::OpenAIClient(this->clif, this->ssl, s->ToCString(), s2->ToCString()));
		if (cli->IsError())
		{
			cli.Delete();
			this->log.LogMessage(CSTR("Error in creating OpenAI client"), IO::LogHandler::LogLevel::Error);
			port = 0;
		}
		else if ((this->db = DB::DBConfig::LoadFromConfig(this->sockf, cfg, nullptr, this->log, false)).IsNull())
		{
			this->log.LogMessage(CSTR("Error in creating DB connection"), IO::LogHandler::LogLevel::Error);
			port = 0;
			cli.Delete();
		}
		else
		{
			this->aiClient = cli;
			IO::MemoryStream mstm;
			if (!cfg->GetValue(CSTR("DBDDL")).SetTo(s))
			{
				this->log.LogMessage(CSTR("Config DBDDL not found"), IO::LogHandler::LogLevel::Error);
				port = 0;
				this->db.Delete();
				this->aiClient.Delete();
			}
			else
			{
				IO::FileStream fs(s->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				if (!fs.IsError() && fs.ReadToEnd(mstm, 8192) > 0)
				{
					this->dbDDL = Text::String::New(mstm.GetBuff(), (UIntOS)mstm.GetLength());
				}
				else
				{
					this->log.LogMessage(CSTR("Error in loading DBDDL file"), IO::LogHandler::LogLevel::Error);
					port = 0;
					this->db.Delete();
					this->aiClient.Delete();
				}
			}
			if (!cfg->GetValue(CSTR("DBRule")).SetTo(s))
			{
				this->log.LogMessage(CSTR("Config DBRule not found"), IO::LogHandler::LogLevel::Error);
				port = 0;
				this->db.Delete();
				this->aiClient.Delete();
			}
			else
			{
				mstm.Clear();
				IO::FileStream fs(s->ToCString(), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				if (!fs.IsError() && fs.ReadToEnd(mstm, 8192) > 0)
				{
					this->dbRule = Text::String::New(mstm.GetBuff(), (UIntOS)mstm.GetLength());
				}
				else
				{
					this->log.LogMessage(CSTR("Error in loading DBRule file"), IO::LogHandler::LogLevel::Error);
					port = 0;
					this->db.Delete();
					this->aiClient.Delete();
				}
			}
		}
	}
	else
	{
		this->log.LogMessage(CSTR("Config OpenAIURL or OpenAIKey not found"), IO::LogHandler::LogLevel::Error);
		port = 0;
	}
	if (port)
	{
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("files"));
		NN<SSWR::AIDemo::AIDemoHandler> hdlr;
		NEW_CLASSNN(hdlr, SSWR::AIDemo::AIDemoHandler(CSTRP(sbuff, sptr), log, *this));
		this->hdlr = hdlr;
		NN<Net::WebServer::WebListener> listener;
		NEW_CLASSNN(listener, Net::WebServer::WebListener(clif, nullptr, hdlr, port, 60, 1, Sync::ThreadUtil::GetThreadCnt(), CSTR("AIDemo/1.0"), false, Net::WebServer::KeepAlive::Default, false));
		if (listener->IsError())
		{
			listener.Delete();
			this->log.LogMessage(CSTR("Error in listening to port"), IO::LogHandler::LogLevel::Error);
		}
		else
		{
			this->listener = listener;
			listener->SetAccessLog(log, IO::LogHandler::LogLevel::Command);
		}
	}
	cfg.Delete();
}

SSWR::AIDemo::AIDemoCore::~AIDemoCore()
{
	this->listener.Delete();
	this->hdlr.Delete();

	this->log.ClearHandlers();
	this->logHdlr.Delete();
	OPTSTR_DEL(this->dbDDL);
	OPTSTR_DEL(this->dbRule);
	this->db.Delete();
	this->aiClient.Delete();
	this->ssl.Delete();
}

Bool SSWR::AIDemo::AIDemoCore::IsError() const
{
	return this->listener.IsNull() || this->aiClient.IsNull() || this->db.IsNull() || this->dbDDL.IsNull() || this->dbRule.IsNull();
}

void SSWR::AIDemo::AIDemoCore::Run()
{
	NN<Net::WebServer::WebListener> listener;
	if (this->listener.SetTo(listener))
	{
		if (listener->Start())
		{
			this->log.LogMessage(CSTR("Start listening"), IO::LogHandler::LogLevel::Action);
			this->progCtrl->WaitForExit(this->progCtrl);
		}
	}
}
