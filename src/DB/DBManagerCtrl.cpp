#include "Stdafx.h"
#include "DB/DBManager.h"
#include "DB/DBManagerCtrl.h"

DB::DBManagerCtrl::DBManagerCtrl(NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	this->log = log;
	this->sockf = sockf;
	this->parsers = parsers;
	this->connStr = 0;
	this->db = 0;
	this->status = ConnStatus::NotConnected;
}

DB::DBManagerCtrl::~DBManagerCtrl()
{
	this->Disconnect();
	OPTSTR_DEL(this->connStr);
}

Bool DB::DBManagerCtrl::Connect()
{
	NotNullPtr<Text::String> connStr;
	if (this->status == ConnStatus::Connected)
	{
		return true;
	}
	else if (this->connStr.SetTo(connStr))
	{
		this->db.Delete();
		this->db = DB::DBManager::OpenConn(connStr->ToCString(), this->log, this->sockf, this->parsers);
		if (this->db.NotNull())
		{
			this->status = ConnStatus::Connected;
			return true;
		}
		else
		{
			this->status = ConnStatus::Error;
			return false;
		}
	}
	this->status = ConnStatus::Error;
	return false;
}

void DB::DBManagerCtrl::Disconnect()
{
	if (this->status == ConnStatus::Error)
	{
		return;
	}
	this->db.Delete();
	this->status = ConnStatus::NotConnected;
}

DB::DBManagerCtrl::ConnStatus DB::DBManagerCtrl::GetStatus()
{
	return this->status;
}

Optional<Text::String> DB::DBManagerCtrl::GetConnStr()
{
	return this->connStr;
}

Optional<DB::ReadingDB> DB::DBManagerCtrl::GetDB()
{
	return this->db;
}

void DB::DBManagerCtrl::GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	NotNullPtr<Text::String> connStr;
	NotNullPtr<DB::ReadingDB> db;
	if (this->connStr.SetTo(connStr))
	{
		DB::DBManager::GetConnName(connStr->ToCString(), sb);		
	}
	else if (this->db.SetTo(db))
	{
		if (db->IsDBTool())
		{
			NotNullPtr<DB::ReadingDBTool>::ConvertFrom(db)->GetDBConn()->GetConnName(sb);
		}
		else if (db->IsFullConn())
		{
			NotNullPtr<DB::DBConn>::ConvertFrom(db)->GetConnName(sb);
		}
		else
		{
			sb->Append(db->GetSourceNameObj());
		}
	}
}

NotNullPtr<DB::DBManagerCtrl> DB::DBManagerCtrl::Create(Text::String *connStr, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	NotNullPtr<DB::DBManagerCtrl> ctrl;
	NEW_CLASSNN(ctrl, DB::DBManagerCtrl(log, sockf, parsers));
	ctrl->connStr = connStr->Clone();
	return ctrl;
}

NotNullPtr<DB::DBManagerCtrl> DB::DBManagerCtrl::Create(Text::CString connStr, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	NotNullPtr<DB::DBManagerCtrl> ctrl;
	NEW_CLASSNN(ctrl, DB::DBManagerCtrl(log, sockf, parsers));
	ctrl->connStr = Text::String::New(connStr);
	return ctrl;
}

NotNullPtr<DB::DBManagerCtrl> DB::DBManagerCtrl::Create(NotNullPtr<DB::DBTool> db, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	Text::StringBuilderUTF8 sb;
	NotNullPtr<DB::DBManagerCtrl> ctrl;
	NEW_CLASSNN(ctrl, DB::DBManagerCtrl(log, sockf, parsers));
	if (DB::DBManager::GetConnStr(db, sb))
	{
		ctrl->connStr = Text::String::New(sb.ToCString());
	}
	ctrl->db = db;
	ctrl->status = ConnStatus::Connected;
	return ctrl;
}

NotNullPtr<DB::DBManagerCtrl> DB::DBManagerCtrl::CreateFromFile(NotNullPtr<DB::ReadingDB> db, NotNullPtr<Text::String> filePath, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<Parser::ParserList> parsers)
{
	Text::StringBuilderUTF8 sb;
	NotNullPtr<DB::DBManagerCtrl> ctrl;
	NEW_CLASSNN(ctrl, DB::DBManagerCtrl(log, sockf, parsers));
	sb.AppendC(UTF8STRC("file:"));
	sb.Append(filePath);
	ctrl->connStr = Text::String::New(sb.ToCString());
	ctrl->db = db;
	ctrl->status = ConnStatus::Connected;
	return ctrl;
}
