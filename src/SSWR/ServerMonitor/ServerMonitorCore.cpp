#include "Stdafx.h"
#include "Crypto/Hash/Bcrypt.h"
#include "DB/SQLiteFile.h"
#include "DB/SQL/CreateTableCommand.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/ServerMonitor/ServerMonitorAlerter.h"
#include "SSWR/ServerMonitor/ServerMonitorCore.h"
#include "SSWR/ServerMonitor/ServerMonitorHandler.h"
#include "SSWR/ServerMonitor/URLMonitorClient.h"

void __stdcall SSWR::ServerMonitor::ServerMonitorCore::CheckThread(NN<Sync::Thread> thread)
{
	NN<ServerMonitorCore> me = thread->GetUserObj().GetNN<ServerMonitorCore>();
	UIntOS i;
	Int32 id;
	NN<ServerInfo> serverInfo;
	NN<ServerMonitorClient> client;
	Data::Timestamp currTime;
	Sync::MutexUsage mutUsage;
	while (!thread->IsStopping())
	{
		i = me->serverMap.GetCount();
		while (i-- > 0)
		{
			mutUsage.ReplaceMutex(me->mut);
			if (me->serverMap.GetItem(i).SetTo(serverInfo))
			{
				id = serverInfo->id;
				currTime = Data::Timestamp::Now();
				if (serverInfo->lastCheck.IsNull() || currTime.DiffMS(serverInfo->lastCheck) >= serverInfo->intervalMS)
				{
					if (serverInfo->client.SetTo(client))
					{
						mutUsage.EndUse();
						Bool thisSuccess = client->ServerValid();
						mutUsage.BeginUse();
						if (me->serverMap.Get(id).SetTo(serverInfo))
						{
							serverInfo->lastCheck = currTime;
							if (serverInfo->lastSuccess && !thisSuccess)
							{
								me->SendAlerts(serverInfo->name->ToCString());
							}
							serverInfo->lastSuccess = thisSuccess;
						}
					}
					else
					{
						serverInfo->lastCheck = currTime;
						serverInfo->lastSuccess = false;
					}
				}
			}
		}
		mutUsage.EndUse();
		thread->Wait(1000);
	}
}

void __stdcall SSWR::ServerMonitor::ServerMonitorCore::FreeUserInfo(NN<UserInfo> userInfo)
{
	userInfo->username->Release();
	userInfo->passwordHash->Release();
	userInfo.Delete();
}

void __stdcall SSWR::ServerMonitor::ServerMonitorCore::FreeServerInfo(NN<ServerInfo> serverInfo)
{
	serverInfo->name->Release();
	serverInfo->target->Release();
	serverInfo->client.Delete();
	serverInfo.Delete();
}

void __stdcall SSWR::ServerMonitor::ServerMonitorCore::FreeAlertInfo(NN<AlertInfo> alertInfo)
{
	alertInfo->settings->Release();
	alertInfo->targets->Release();
	alertInfo.Delete();
}

void SSWR::ServerMonitor::ServerMonitorCore::SendAlerts(Text::CStringNN serverName)
{

}

SSWR::ServerMonitor::ServerMonitorCore::ServerMonitorCore() : checkThread(CheckThread, this, CSTR("CheckThread"))
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(false));
	NEW_CLASSNN(this->clif, Net::TCPClientFactory(this->sockf));
	this->ssl = Net::SSLEngineFactory::Create(this->clif, false);
	this->db = nullptr;
	this->listener = nullptr;
	this->webHdlr = nullptr;

	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Log"));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("SM"));
	this->log.AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, nullptr, false);

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
			NN<DB::DBTool> db;
			this->db = DB::SQLiteFile::CreateDBTool(CSTRP(sbuff, sptr), this->log, CSTR("DB: "));
			if (!this->db.SetTo(db))
			{
				this->console.WriteLine(CSTR("Error in opening database file"));
			}
			else
			{
				NN<UserInfo> userInfo;
				NN<DB::DBReader> r;
				if (db->ExecuteReader(CSTR("select username, password, role from webuser")).SetTo(r))
				{
					while (r->ReadNext())
					{
						NN<Text::String> username = r->GetNewStrNN(0);
						NN<Text::String> passwordHash = r->GetNewStrNN(1);
						UserRole role = (UserRole)r->GetInt32(2);
						NEW_CLASSNN(userInfo, UserInfo());
						userInfo->username = username;
						userInfo->passwordHash = passwordHash;
						userInfo->role = role;
						this->userMap.Put(username, userInfo);
					}
					db->CloseReader(r);
				}
				else
				{
					DB::TableDef tabDef(nullptr, CSTR("webuser"));
					tabDef.AddCol(DB::ColDef::Create(CSTR("username"))->ColType(DB::DBUtil::ColType::CT_VarUTF8Char)->ColSize(64)->NotNull(true)->PK(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("password"))->ColType(DB::DBUtil::ColType::CT_VarUTF8Char)->ColSize(512)->NotNull(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("role"))->ColType(DB::DBUtil::ColType::CT_Int32)->NotNull(true));
					DB::SQL::CreateTableCommand cmd(tabDef, false);
					DB::SQLBuilder sql(db->GetSQLType(), db->IsAxisAware(), db->GetTzQhr());
					sql.AppendSQLCommand(cmd);
					db->ExecuteNonQuery(sql.ToCString());
					sql.Clear();
					sql.AppendCmdC(CSTR("insert into webuser ("));
					sql.AppendCol(U8STR("username"));
					sql.AppendCmdC(CSTR(", "));
					sql.AppendCol(U8STR("password"));
					sql.AppendCmdC(CSTR(", "));
					sql.AppendCol(U8STR("role"));
					sql.AppendCmdC(CSTR(") values ("));
					sql.AppendStrC(CSTR("admin"));
					sql.AppendCmdC(CSTR(", "));
					Text::StringBuilderUTF8 sb;
					Crypto::Hash::Bcrypt bcrypt;
					bcrypt.GenHash(sb, 10, CSTR("admin"));
					NEW_CLASSNN(userInfo, UserInfo());
					userInfo->username = Text::String::New(CSTR("admin"));
					userInfo->passwordHash = Text::String::New(sb.ToCString());
					userInfo->role = UserRole::Admin;
					this->userMap.Put(userInfo->username, userInfo);
					sql.AppendStrC(sb.ToCString());
					sql.AppendCmdC(CSTR(", "));
					sql.AppendInt32((Int32)UserRole::Admin);
					sql.AppendCmdC(CSTR(")"));
					db->ExecuteNonQuery(sql.ToCString());
				}

				NN<ServerInfo> serverInfo;
				if (db->ExecuteReader(CSTR("select id, name, serverType, target, intervalMS, timeoutMS from servers")).SetTo(r))
				{
					while (r->ReadNext())
					{
						NEW_CLASSNN(serverInfo, ServerInfo());
						serverInfo->id = r->GetInt32(0);
						serverInfo->name = r->GetNewStrNN(1);
						serverInfo->serverType = (ServerType)r->GetInt32(2);
						serverInfo->target = r->GetNewStrNN(3);
						serverInfo->intervalMS = r->GetInt32(4);
						serverInfo->timeoutMS = r->GetInt32(5);
						serverInfo->lastSuccess = false;
						serverInfo->lastCheck = nullptr;
						serverInfo->client = ServerMonitorClient::CreateClient(serverInfo->serverType, this->clif, this->ssl, serverInfo->target->ToCString(), serverInfo->timeoutMS);
						this->serverMap.Put(serverInfo->id, serverInfo);
					}
					db->CloseReader(r);
				}
				else
				{
					DB::TableDef tabDef(nullptr, CSTR("servers"));
					tabDef.AddCol(DB::ColDef::Create(CSTR("id"))->ColType(DB::DBUtil::ColType::CT_Int32)->NotNull(true)->PK(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("name"))->ColType(DB::DBUtil::ColType::CT_VarUTF8Char)->ColSize(512)->NotNull(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("serverType"))->ColType(DB::DBUtil::ColType::CT_Int32)->NotNull(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("target"))->ColType(DB::DBUtil::ColType::CT_VarUTF8Char)->ColSize(1024)->NotNull(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("intervalMS"))->ColType(DB::DBUtil::ColType::CT_Int32)->NotNull(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("timeoutMS"))->ColType(DB::DBUtil::ColType::CT_Int32)->NotNull(true));
					DB::SQL::CreateTableCommand cmd(tabDef, false);
					DB::SQLBuilder sql(db->GetSQLType(), db->IsAxisAware(), db->GetTzQhr());
					sql.AppendSQLCommand(cmd);
					db->ExecuteNonQuery(sql.ToCString());
				}

				NN<AlertInfo> alertInfo;
				if (db->ExecuteReader(CSTR("select id, type, settings, targets from alerts")).SetTo(r))
				{
					while (r->ReadNext())
					{
						NEW_CLASSNN(alertInfo, AlertInfo());
						alertInfo->id = r->GetInt32(0);
						alertInfo->type = (AlertType)r->GetInt32(1);
						alertInfo->settings = r->GetNewStrNN(2);
						alertInfo->targets = r->GetNewStrNN(3);
						alertInfo->alerter = ServerMonitorAlerter::CreateAlerter(alertInfo->type, alertInfo->settings->ToCString(), alertInfo->targets->ToCString(), this->clif, this->ssl, this->log);
						this->alertMap.Put(alertInfo->id, alertInfo);
					}
					db->CloseReader(r);
				}
				else
				{
					DB::TableDef tabDef(nullptr, CSTR("alerts"));
					tabDef.AddCol(DB::ColDef::Create(CSTR("id"))->ColType(DB::DBUtil::ColType::CT_Int32)->NotNull(true)->PK(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("type"))->ColType(DB::DBUtil::ColType::CT_Int32)->NotNull(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("settings"))->ColType(DB::DBUtil::ColType::CT_VarUTF8Char)->ColSize(2048)->NotNull(true));
					tabDef.AddCol(DB::ColDef::Create(CSTR("targets"))->ColType(DB::DBUtil::ColType::CT_VarUTF8Char)->ColSize(2048)->NotNull(true));
					DB::SQL::CreateTableCommand cmd(tabDef, false);
					DB::SQLBuilder sql(db->GetSQLType(), db->IsAxisAware(), db->GetTzQhr());
					sql.AppendSQLCommand(cmd);
					db->ExecuteNonQuery(sql.ToCString());
				}

				NN<SSWR::ServerMonitor::ServerMonitorHandler> webHdlr;
				if (cfg->GetValue(CSTR("WebPath")).SetTo(s))
				{
					sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
					sptr = IO::Path::AppendPath(sbuff, sptr, s->ToCString());
				}
				else
				{
					sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
					sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("web"));
				}
				NEW_CLASSNN(webHdlr, SSWR::ServerMonitor::ServerMonitorHandler(*this, CSTRP(sbuff, sptr)));
				this->webHdlr = webHdlr;

				if (cfg->GetValue(CSTR("Port")).SetTo(s) && s->ToUInt16(port))
				{
					NN<Net::WebServer::WebListener> listener;
					NEW_CLASSNN(listener, Net::WebServer::WebListener(this->clif, nullptr, webHdlr, port, 60, 1, 8, nullptr, false, Net::WebServer::KeepAlive::Default, false));
					if (listener->IsError())
					{
						this->console.WriteLine(CSTR("Error in starting web server"));
						listener.Delete();
					}
					else
					{
						this->listener = listener;
					}
				}
				else
				{
					this->console.WriteLine(CSTR("Error in reading port number"));
				}
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
	this->checkThread.BeginStop();
	this->listener.Delete();
	this->webHdlr.Delete();
	this->checkThread.Stop();
	this->db.Delete();
	this->ssl.Delete();
	this->clif.Delete();
	this->sockf.Delete();
	this->userMap.FreeAll(FreeUserInfo);
}

Bool SSWR::ServerMonitor::ServerMonitorCore::IsError() const
{
	return this->ssl.IsNull() || this->db.IsNull();
}

Bool SSWR::ServerMonitor::ServerMonitorCore::Run()
{
	NN<Net::WebServer::WebListener> listener;
	if (this->ssl.NotNull() && this->db.NotNull() && this->listener.SetTo(listener) && listener->Start() && this->checkThread.Start())
	{
		this->console.WriteLine(CSTR("ServerMonitor is running"));
		return true;
	}
	return false;
}

SSWR::ServerMonitor::UserRole SSWR::ServerMonitor::ServerMonitorCore::Login(Text::CStringNN username, Text::CStringNN password)
{
	NN<UserInfo> userInfo;
	if (this->userMap.GetC(username).SetTo(userInfo))
	{
		Crypto::Hash::Bcrypt bcrypt;
		if (bcrypt.Matches(userInfo->passwordHash->ToCString(), password))
		{
			return userInfo->role;
		}
	}
	return UserRole::NotLogged;
}

void SSWR::ServerMonitor::ServerMonitorCore::GetServerList(NN<Data::ArrayListNN<ServerInfo>> serverList, NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
	serverList->AddAll(this->serverMap);
}

void SSWR::ServerMonitor::ServerMonitorCore::GetAlertList(NN<Data::ArrayListNN<AlertInfo>> alertList, NN<Sync::MutexUsage> mutUsage)
{
	mutUsage->ReplaceMutex(this->mut);
	alertList->AddAll(this->alertMap);
}

Optional<SSWR::ServerMonitor::ServerInfo> SSWR::ServerMonitor::ServerMonitorCore::AddServerURL(Text::CStringNN name, Text::CStringNN url, Text::CString containsText, Int32 timeoutMS)
{
	NN<DB::DBTool> db;
	if (!this->db.SetTo(db))
	{
		return nullptr;
	}

	if (containsText.leng == 0)
	{
		containsText = nullptr;
	}
	NN<URLMonitorClient> client;
	NEW_CLASSNN(client, URLMonitorClient(this->clif, this->ssl, url, containsText, timeoutMS));
	if (client->HasError())
	{
		return nullptr;
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		NN<ServerInfo> serverInfo;
		NEW_CLASSNN(serverInfo, ServerInfo());
		{
			Sync::MutexUsage mutUsage(this->mut);
			serverInfo->id = this->serverMap.GetKey(this->serverMap.GetCount() - 1) + 1;
			serverInfo->name = Text::String::New(name);
			serverInfo->serverType = ServerType::URL;
			client->BuildTarget(sb);
			serverInfo->target = Text::String::New(sb.ToCString());
			serverInfo->intervalMS = 60000;
			serverInfo->timeoutMS = timeoutMS;
			serverInfo->lastSuccess = false;
			serverInfo->lastCheck = nullptr;
			serverInfo->client = client;
			this->serverMap.Put(serverInfo->id, serverInfo);
		}
		DB::SQLBuilder sql(db->GetSQLType(), db->IsAxisAware(), db->GetTzQhr());
		sql.AppendCmdC(CSTR("insert into servers ("));
		sql.AppendCol(U8STR("id"));;
		sql.AppendCmdC(CSTR(", "));
		sql.AppendCol(U8STR("name"));;
		sql.AppendCmdC(CSTR(", "));
		sql.AppendCol(U8STR("serverType"));;
		sql.AppendCmdC(CSTR(", "));
		sql.AppendCol(U8STR("target"));;
		sql.AppendCmdC(CSTR(", "));
		sql.AppendCol(U8STR("intervalMS"));;
		sql.AppendCmdC(CSTR(", "));
		sql.AppendCol(U8STR("timeoutMS"));;
		sql.AppendCmdC(CSTR(") values ("));
		sql.AppendInt32(serverInfo->id);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendStr(serverInfo->name);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendInt32((Int32)serverInfo->serverType);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendStr(serverInfo->target);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendInt32(serverInfo->intervalMS);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendInt32(serverInfo->timeoutMS);
		sql.AppendCmdC(CSTR(")"));
		if (db->ExecuteNonQuery(sql.ToCString()) < 0)
		{
			Sync::MutexUsage mutUsage(this->mut);
			this->serverMap.Remove(serverInfo->id);
			FreeServerInfo(serverInfo);
			return nullptr;
		}
		return serverInfo;
	}
}

Optional<SSWR::ServerMonitor::AlertInfo> SSWR::ServerMonitor::ServerMonitorCore::AddAlertSMTP(Text::CStringNN host, UInt16 port, Net::Email::SMTPConn::ConnType connType, Text::CStringNN smtpUser, Text::CStringNN smtpPassword, Text::CStringNN fromEmail, Text::CStringNN toEmails)
{
	return nullptr;
}
