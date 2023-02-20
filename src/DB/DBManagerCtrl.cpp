#include "Stdafx.h"
#include "DB/DBManager.h"
#include "DB/DBManagerCtrl.h"

DB::DBManagerCtrl::DBManagerCtrl(IO::LogTool *log, Net::SocketFactory *sockf)
{
	this->log = log;
	this->sockf = sockf;
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
		this->db = DB::DBManager::OpenConn(this->connStr->ToCString(), this->log, this->sockf);
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

DB::DBTool *DB::DBManagerCtrl::GetDB()
{
	return this->db;
}

void DB::DBManagerCtrl::GetConnName(Text::StringBuilderUTF8 *sb)
{
	if (this->db)
	{
		this->db->GetConn()->GetConnName(sb);
	}
	else if (this->connStr)
	{
		DB::DBManager::GetConnName(this->connStr->ToCString(), sb);		
	}
}

DB::DBManagerCtrl *DB::DBManagerCtrl::Create(Text::String *connStr, IO::LogTool *log, Net::SocketFactory *sockf)
{
	DB::DBManagerCtrl *ctrl;
	NEW_CLASS(ctrl, DB::DBManagerCtrl(log, sockf));
	ctrl->connStr = connStr->Clone();
	return ctrl;
}

DB::DBManagerCtrl *DB::DBManagerCtrl::Create(Text::CString connStr, IO::LogTool *log, Net::SocketFactory *sockf)
{
	DB::DBManagerCtrl *ctrl;
	NEW_CLASS(ctrl, DB::DBManagerCtrl(log, sockf));
	ctrl->connStr = Text::String::New(connStr);
	return ctrl;
}

DB::DBManagerCtrl *DB::DBManagerCtrl::Create(DB::DBTool *db, IO::LogTool *log, Net::SocketFactory *sockf)
{
	Text::StringBuilderUTF8 sb;
	DB::DBManagerCtrl *ctrl;
	NEW_CLASS(ctrl, DB::DBManagerCtrl(log, sockf));
	if (DB::DBManager::GetConnStr(db, &sb))
	{
		ctrl->connStr = Text::String::New(sb.ToCString());
	}
	ctrl->db = db;
	ctrl->status = ConnStatus::Connected;
	return ctrl;
}
