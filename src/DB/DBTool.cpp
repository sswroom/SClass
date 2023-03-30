#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "DB/SQLGenerator.h"
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Math/Geometry/Point.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"


DB::DBTool::DBTool(DB::DBConn *conn, Bool needRelease, IO::LogTool *log, Text::CString logPrefix) : DB::ReadingDBTool(conn, needRelease, log, logPrefix)
{
	this->nqFail = 0;
	this->tran = 0;
}

DB::DBTool::~DBTool()
{
}

OSInt DB::DBTool::ExecuteNonQuery(Text::CString sqlCmd)
{
	{
		Text::StringBuilderUTF8 logMsg;
		logMsg.AppendC(UTF8STRC("ExecuteNonQuery: "));
		logMsg.Append(sqlCmd);
		AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::ILogHandler::LogLevel::Raw);
	}
	if (this->db == 0)
	{
		dataCnt += 1;
		return -1;
	}

	Data::DateTime t1;
	Data::DateTime t2;
	OSInt i = ((DB::DBConn*)this->db)->ExecuteNonQuery(sqlCmd);
	if (i >= -1)
	{
		Data::DateTime t3;
		dataCnt += 1;
		if (t3.DiffMS(&t2) >= 1000)
		{
			UTF8Char buff[256];
			UTF8Char *ptr;
			ptr = Text::StrConcatC(buff, UTF8STRC("SQL R t1 = "));
			ptr = Text::StrInt32(ptr, (Int32)t2.DiffMS(&t1));
			ptr = Text::StrConcatC(ptr, UTF8STRC(", t2 = "));
			ptr = Text::StrInt32(ptr, (Int32)t3.DiffMS(&t2));
			AddLogMsgC(buff, (UOSInt)(ptr - buff), IO::ILogHandler::LogLevel::Command);
		}
		nqFail = 0;
		openFail = 0;
		return i;
	}
	else
	{
		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.AppendC(UTF8STRC("Cannot execute the sql command: "));
			logMsg.Append(sqlCmd);
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::ILogHandler::LogLevel::Error);
		}

		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.AppendC(UTF8STRC("Exception detail: "));
			this->lastErrMsg.ClearStr();
			this->db->GetLastErrorMsg(&this->lastErrMsg);
			logMsg.AppendSB(&this->lastErrMsg);
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::ILogHandler::LogLevel::ErrorDetail);
		}
		Bool isData = this->db->IsLastDataError();
		if (!isData)
		{
			this->db->Reconnect();
		}

		if (trig)
			trig(sqlCmd, DB::ReadingDBTool::NonQueryTrigger);
		if (isData)
			return -3;
		else
			return -2;
	}
}

void DB::DBTool::BeginTrans()
{
	if (tran == 0 && this->db)
		tran = ((DB::DBConn*)this->db)->BeginTransaction();
}

void DB::DBTool::EndTrans(Bool toCommit)
{
	if (tran == 0)
		return;

	if (toCommit)
		((DB::DBConn*)this->db)->Commit(tran);
	else
		((DB::DBConn*)this->db)->Rollback(tran);
	tran = 0;
}

Int32 DB::DBTool::GetLastIdentity32()
{
	if (this->sqlType == DB::DBUtil::SQLType::MySQL || this->sqlType == DB::DBUtil::SQLType::MSSQL || this->sqlType == DB::DBUtil::SQLType::Access || this->sqlType == DB::DBUtil::SQLType::MDBTools)
	{
		DB::DBReader *reader = this->ExecuteReader(CSTR("select @@identity"));
		Int32 id = 0;
		if (reader)
		{
			if (reader->ReadNext())
			{
				id = reader->GetInt32(0);
			}
			this->CloseReader(reader);
		}
		return id;
	}
	else
	{
		return 0;
	}
}

Int64 DB::DBTool::GetLastIdentity64()
{
	if (this->sqlType == DB::DBUtil::SQLType::MySQL || this->sqlType == DB::DBUtil::SQLType::MSSQL || this->sqlType == DB::DBUtil::SQLType::Access || this->sqlType == DB::DBUtil::SQLType::MDBTools)
	{
		DB::DBReader *reader = this->ExecuteReader(CSTR("select @@identity"));
		Int64 id = 0;
		if (reader)
		{
			if (reader->ReadNext())
			{
				id = reader->GetInt64(0);
			}
			this->CloseReader(reader);
		}
		return id;
	}
	else
	{
		return 0;
	}
}

Bool DB::DBTool::CreateDatabase(Text::CString databaseName)
{
	switch (this->sqlType)
	{
	case DB::DBUtil::SQLType::MSSQL:
	case DB::DBUtil::SQLType::MySQL:
	case DB::DBUtil::SQLType::Oracle:
	case DB::DBUtil::SQLType::Access:
	case DB::DBUtil::SQLType::SQLite:
	case DB::DBUtil::SQLType::MDBTools:
	case DB::DBUtil::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		DB::SQLGenerator::GenCreateDatabaseCmd(&sql, databaseName);
		return this->ExecuteNonQuery(sql.ToCString()) >= -1;
	}
	case DB::DBUtil::SQLType::WBEM:
	case DB::DBUtil::SQLType::Unknown:
	default:
		this->lastErrMsg.ClearStr();
		this->lastErrMsg.AppendC(UTF8STRC("Create database is not supported"));
		return false;
	}
}

Bool DB::DBTool::DeleteDatabase(Text::CString databaseName)
{
	switch (this->sqlType)
	{
	case DB::DBUtil::SQLType::MSSQL:
	case DB::DBUtil::SQLType::MySQL:
	case DB::DBUtil::SQLType::Oracle:
	case DB::DBUtil::SQLType::Access:
	case DB::DBUtil::SQLType::SQLite:
	case DB::DBUtil::SQLType::MDBTools:
	case DB::DBUtil::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		DB::SQLGenerator::GenDeleteDatabaseCmd(&sql, databaseName);
		return this->ExecuteNonQuery(sql.ToCString()) >= -1;
	}
	case DB::DBUtil::SQLType::WBEM:
	case DB::DBUtil::SQLType::Unknown:
	default:
		this->lastErrMsg.ClearStr();
		this->lastErrMsg.AppendC(UTF8STRC("Delete database is not supported"));
		return false;
	}
}

Bool DB::DBTool::CreateSchema(Text::CString schemaName)
{
	switch (this->sqlType)
	{
	case DB::DBUtil::SQLType::MSSQL:
	case DB::DBUtil::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		DB::SQLGenerator::GenCreateSchemaCmd(&sql, schemaName);
		return this->ExecuteNonQuery(sql.ToCString()) >= -1;
	}
	case DB::DBUtil::SQLType::MySQL:
	case DB::DBUtil::SQLType::Oracle:
	case DB::DBUtil::SQLType::SQLite:
	case DB::DBUtil::SQLType::MDBTools:
	case DB::DBUtil::SQLType::Access:
	case DB::DBUtil::SQLType::WBEM:
	case DB::DBUtil::SQLType::Unknown:
	default:
		this->lastErrMsg.ClearStr();
		this->lastErrMsg.AppendC(UTF8STRC("Delete schema is not supported"));
		return false;
	}
}

Bool DB::DBTool::DeleteSchema(Text::CString schemaName)
{
	switch (this->sqlType)
	{
	case DB::DBUtil::SQLType::MSSQL:
	case DB::DBUtil::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		DB::SQLGenerator::GenDeleteSchemaCmd(&sql, schemaName);
		return this->ExecuteNonQuery(sql.ToCString()) >= -1;
	}
	case DB::DBUtil::SQLType::MySQL:
	case DB::DBUtil::SQLType::Oracle:
	case DB::DBUtil::SQLType::SQLite:
	case DB::DBUtil::SQLType::MDBTools:
	case DB::DBUtil::SQLType::Access:
	case DB::DBUtil::SQLType::WBEM:
	case DB::DBUtil::SQLType::Unknown:
	default:
		this->lastErrMsg.ClearStr();
		this->lastErrMsg.AppendC(UTF8STRC("Delete schema is not supported"));
		return false;
	}
}

Bool DB::DBTool::DeleteTableData(Text::CString schemaName, Text::CString tableName)
{
	DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
	DB::SQLGenerator::GenTruncateTableCmd(&sql, schemaName, tableName);
	if (this->ExecuteNonQuery(sql.ToCString()) >= -1)
	{
		return true;
	}
	sql.Clear();
	DB::SQLGenerator::GenDeleteTableDataCmd(&sql, schemaName, tableName);
	return this->ExecuteNonQuery(sql.ToCString()) >= -1;
}

Bool DB::DBTool::KillConnection(Int32 id)
{
	Bool ret = false;
	if (this->sqlType == DB::DBUtil::SQLType::MySQL)
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		sql.AppendCmdC(CSTR("kill "));
		sql.AppendInt32(id);
		ret = (this->ExecuteNonQuery(sql.ToCString()) >= -1);
	}
	else if (this->sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		sql.AppendCmdC(CSTR("kill "));
		sql.AppendInt32(id);
		ret = (this->ExecuteNonQuery(sql.ToCString()) >= -1);
	}
	else if (this->sqlType == DB::DBUtil::SQLType::PostgreSQL)
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		sql.AppendCmdC(CSTR("SELECT pg_terminate_backend("));
		sql.AppendInt32(id);
		sql.AppendCmdC(CSTR(")"));
		DB::DBReader *r = this->ExecuteReader(sql.ToCString());
		if (r)
		{
			if (r->ReadNext())
			{
				ret = r->GetBool(0);
			}
			this->CloseReader(r);
		}
	}
	return ret;	
}

Bool DB::DBTool::CanModify()
{
	return true;
}
