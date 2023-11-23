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
	SDEL_STRING(this->connStr);
}

Bool DB::DBManagerCtrl::Connect()
{
	if (this->status == ConnStatus::Connected)
	{
		return true;
	}
	else
	{
		SDEL_CLASS(this->db);
		this->db = DB::DBManager::OpenConn(this->connStr->ToCString(), this->log, this->sockf, this->parsers);
		if (this->db)
		{
			this->status = ConnStatus::Connected;
			return this->db;
		}
		else
		{
			this->status = ConnStatus::Error;
			return this->db;
		}
	}
}

void DB::DBManagerCtrl::Disconnect()
{
	if (this->status == ConnStatus::Error)
	{
		return;
	}
	SDEL_CLASS(this->db);
	this->status = ConnStatus::NotConnected;
}

DB::DBManagerCtrl::ConnStatus DB::DBManagerCtrl::GetStatus()
{
	return this->status;
}

Text::String *DB::DBManagerCtrl::GetConnStr()
{
	return this->connStr;
}

DB::ReadingDB *DB::DBManagerCtrl::GetDB()
{
	return this->db;
}

void DB::DBManagerCtrl::GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (this->connStr)
	{
		DB::DBManager::GetConnName(this->connStr->ToCString(), sb);		
	}
	else if (this->db)
	{
		if (this->db->IsDBTool())
		{
			((DB::ReadingDBTool*)this->db)->GetDBConn()->GetConnName(sb);
		}
		else if (this->db->IsFullConn())
		{
			((DB::DBConn*)this->db)->GetConnName(sb);
		}
		else
		{
			sb->Append(this->db->GetSourceNameObj());
		}
	}
}

DB::DBManagerCtrl *DB::DBManagerCtrl::Create(Text::String *connStr, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	DB::DBManagerCtrl *ctrl;
	NEW_CLASS(ctrl, DB::DBManagerCtrl(log, sockf, parsers));
	ctrl->connStr = connStr->Clone().Ptr();
	return ctrl;
}

DB::DBManagerCtrl *DB::DBManagerCtrl::Create(Text::CString connStr, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	DB::DBManagerCtrl *ctrl;
	NEW_CLASS(ctrl, DB::DBManagerCtrl(log, sockf, parsers));
	ctrl->connStr = Text::String::New(connStr).Ptr();
	return ctrl;
}

DB::DBManagerCtrl *DB::DBManagerCtrl::Create(NotNullPtr<DB::DBTool> db, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	Text::StringBuilderUTF8 sb;
	DB::DBManagerCtrl *ctrl;
	NEW_CLASS(ctrl, DB::DBManagerCtrl(log, sockf, parsers));
	if (DB::DBManager::GetConnStr(db, sb))
	{
		ctrl->connStr = Text::String::New(sb.ToCString()).Ptr();
	}
	ctrl->db = db.Ptr();
	ctrl->status = ConnStatus::Connected;
	return ctrl;
}

DB::DBManagerCtrl *DB::DBManagerCtrl::CreateFromFile(NotNullPtr<DB::ReadingDB> db, NotNullPtr<Text::String> filePath, NotNullPtr<IO::LogTool> log, NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<Parser::ParserList> parsers)
{
	Text::StringBuilderUTF8 sb;
	DB::DBManagerCtrl *ctrl;
	NEW_CLASS(ctrl, DB::DBManagerCtrl(log, sockf, parsers));
	sb.AppendC(UTF8STRC("file:"));
	sb.Append(filePath);
	ctrl->connStr = Text::String::New(sb.ToCString()).Ptr();
	ctrl->db = db.Ptr();
	ctrl->status = ConnStatus::Connected;
	return ctrl;
}
