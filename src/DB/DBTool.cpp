#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
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


DB::DBTool::DBTool(NN<DB::DBConn> conn, Bool needRelease, NN<IO::LogTool> log, Text::CString logPrefix) : DB::ReadingDBTool(conn, needRelease, log, logPrefix)
{
	this->ssh = nullptr;
	this->sshCli = nullptr;
	this->nqFail = 0;
	this->tran = nullptr;
}

DB::DBTool::~DBTool()
{
	this->sshCli.Delete();
	this->ssh.Delete();
}

void DB::DBTool::SetSSHTunnel(Optional<Net::SSHManager> ssh, Optional<Net::SSHClient> sshCli)
{
	this->ssh = ssh;
	this->sshCli = sshCli;
}

Optional<Net::SSHClient> DB::DBTool::GetSSHClient() const
{
	return this->sshCli;
}

OSInt DB::DBTool::ExecuteNonQuery(Text::CStringNN sqlCmd)
{
	{
		Text::StringBuilderUTF8 logMsg;
		logMsg.AppendC(UTF8STRC("ExecuteNonQuery: "));
		logMsg.Append(sqlCmd);
		AddLogMsgC(logMsg.ToCString(), IO::LogHandler::LogLevel::Raw);
	}

	Data::Timestamp t1 = Data::Timestamp::UtcNow();
	Data::Timestamp t2 = Data::Timestamp::UtcNow();
	OSInt i = this->db->ExecuteNonQuery(sqlCmd);
	if (i < -1 && !this->db->IsLastDataError())
	{
		this->db->Reconnect();
		i = this->db->ExecuteNonQuery(sqlCmd);
	}
	if (i >= -1)
	{
		Data::Timestamp t3 = Data::Timestamp::UtcNow();
		dataCnt += 1;
		if (t3.DiffMS(t2) >= 1000)
		{
			UTF8Char buff[256];
			UnsafeArray<UTF8Char> ptr;
			ptr = Text::StrConcatC(buff, UTF8STRC("SQL R t1 = "));
			ptr = Text::StrInt32(ptr, (Int32)t2.DiffMS(t1));
			ptr = Text::StrConcatC(ptr, UTF8STRC(", t2 = "));
			ptr = Text::StrInt32(ptr, (Int32)t3.DiffMS(t2));
			AddLogMsgC(CSTRP(buff, ptr), IO::LogHandler::LogLevel::Command);
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
			AddLogMsgC(logMsg.ToCString(), IO::LogHandler::LogLevel::Error);
		}

		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.AppendC(UTF8STRC("Exception detail: "));
			this->lastErrMsg.ClearStr();
			this->db->GetLastErrorMsg(this->lastErrMsg);
			logMsg.AppendSB(this->lastErrMsg);
			AddLogMsgC(logMsg.ToCString(), IO::LogHandler::LogLevel::ErrorDetail);
		}
		Bool isData = this->db->IsLastDataError();

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
	if (tran.IsNull())
		tran = this->db->BeginTransaction();
}

void DB::DBTool::EndTrans(Bool toCommit)
{
	NN<DB::DBTransaction> thisTran;
	if (!tran.SetTo(thisTran))
		return;

	if (toCommit)
		this->db->Commit(thisTran);
	else
		this->db->Rollback(thisTran);
	tran = nullptr;
}

Int32 DB::DBTool::GetLastIdentity32()
{
	if (this->sqlType == DB::SQLType::MySQL || this->sqlType == DB::SQLType::MSSQL || this->sqlType == DB::SQLType::Access || this->sqlType == DB::SQLType::MDBTools)
	{
		Int32 id = 0;
		NN<DB::DBReader> reader;
		if (this->ExecuteReader(CSTR("select @@identity")).SetTo(reader))
		{
			if (reader->ReadNext())
			{
				id = reader->GetInt32(0);
			}
			this->CloseReader(reader);
		}
		return id;
	}
	else if (this->sqlType == DB::SQLType::PostgreSQL)
	{
		Int32 id = 0;
		NN<DB::DBReader> reader;
		if (this->ExecuteReader(CSTR("select lastval()")).SetTo(reader))
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
	if (this->sqlType == DB::SQLType::MySQL || this->sqlType == DB::SQLType::MSSQL || this->sqlType == DB::SQLType::Access || this->sqlType == DB::SQLType::MDBTools)
	{
		Int64 id = 0;
		NN<DB::DBReader> reader;
		if (this->ExecuteReader(CSTR("select @@identity")).SetTo(reader))
		{
			if (reader->ReadNext())
			{
				id = reader->GetInt64(0);
			}
			this->CloseReader(reader);
		}
		return id;
	}
	else if (this->sqlType == DB::SQLType::PostgreSQL)
	{
		Int64 id = 0;
		NN<DB::DBReader> reader;
		if (this->ExecuteReader(CSTR("select lastval()")).SetTo(reader))
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

Bool DB::DBTool::CreateDatabase(Text::CStringNN databaseName, const Collation *collation)
{
	switch (this->sqlType)
	{
	case DB::SQLType::MySQL:
	case DB::SQLType::MSSQL:
	case DB::SQLType::Oracle:
	case DB::SQLType::Access:
	case DB::SQLType::SQLite:
	case DB::SQLType::MDBTools:
	case DB::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		DB::SQLGenerator::GenCreateDatabaseCmd(sql, databaseName, collation);
		return this->ExecuteNonQuery(sql.ToCString()) >= -1;
	}
	case DB::SQLType::WBEM:
	case DB::SQLType::Unknown:
	default:
		this->lastErrMsg.ClearStr();
		this->lastErrMsg.AppendC(UTF8STRC("Create database is not supported"));
		return false;
	}
}

Bool DB::DBTool::DeleteDatabase(Text::CStringNN databaseName)
{
	switch (this->sqlType)
	{
	case DB::SQLType::MSSQL:
	case DB::SQLType::MySQL:
	case DB::SQLType::Oracle:
	case DB::SQLType::Access:
	case DB::SQLType::SQLite:
	case DB::SQLType::MDBTools:
	case DB::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		DB::SQLGenerator::GenDeleteDatabaseCmd(sql, databaseName);
		return this->ExecuteNonQuery(sql.ToCString()) >= -1;
	}
	case DB::SQLType::WBEM:
	case DB::SQLType::Unknown:
	default:
		this->lastErrMsg.ClearStr();
		this->lastErrMsg.AppendC(UTF8STRC("Delete database is not supported"));
		return false;
	}
}

Bool DB::DBTool::CreateSchema(Text::CStringNN schemaName)
{
	switch (this->sqlType)
	{
	case DB::SQLType::MSSQL:
	case DB::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		DB::SQLGenerator::GenCreateSchemaCmd(sql, schemaName);
		return this->ExecuteNonQuery(sql.ToCString()) >= -1;
	}
	case DB::SQLType::MySQL:
	case DB::SQLType::Oracle:
	case DB::SQLType::SQLite:
	case DB::SQLType::MDBTools:
	case DB::SQLType::Access:
	case DB::SQLType::WBEM:
	case DB::SQLType::Unknown:
	default:
		this->lastErrMsg.ClearStr();
		this->lastErrMsg.AppendC(UTF8STRC("Delete schema is not supported"));
		return false;
	}
}

Bool DB::DBTool::DeleteSchema(Text::CStringNN schemaName)
{
	switch (this->sqlType)
	{
	case DB::SQLType::MSSQL:
	case DB::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		DB::SQLGenerator::GenDeleteSchemaCmd(sql, schemaName);
		return this->ExecuteNonQuery(sql.ToCString()) >= -1;
	}
	case DB::SQLType::MySQL:
	case DB::SQLType::Oracle:
	case DB::SQLType::SQLite:
	case DB::SQLType::MDBTools:
	case DB::SQLType::Access:
	case DB::SQLType::WBEM:
	case DB::SQLType::Unknown:
	default:
		this->lastErrMsg.ClearStr();
		this->lastErrMsg.AppendC(UTF8STRC("Delete schema is not supported"));
		return false;
	}
}

Bool DB::DBTool::DeleteTableData(Text::CString schemaName, Text::CStringNN tableName)
{
	DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
	DB::SQLGenerator::GenTruncateTableCmd(sql, schemaName, tableName);
	if (this->ExecuteNonQuery(sql.ToCString()) >= -1)
	{
		return true;
	}
	sql.Clear();
	DB::SQLGenerator::GenDeleteTableDataCmd(sql, schemaName, tableName);
	return this->ExecuteNonQuery(sql.ToCString()) >= -1;
}

Bool DB::DBTool::KillConnection(Int32 id)
{
	Bool ret = false;
	if (this->sqlType == DB::SQLType::MySQL)
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		sql.AppendCmdC(CSTR("kill "));
		sql.AppendInt32(id);
		ret = (this->ExecuteNonQuery(sql.ToCString()) >= -1);
	}
	else if (this->sqlType == DB::SQLType::MSSQL)
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		sql.AppendCmdC(CSTR("kill "));
		sql.AppendInt32(id);
		ret = (this->ExecuteNonQuery(sql.ToCString()) >= -1);
	}
	else if (this->sqlType == DB::SQLType::PostgreSQL)
	{
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		sql.AppendCmdC(CSTR("SELECT pg_terminate_backend("));
		sql.AppendInt32(id);
		sql.AppendCmdC(CSTR(")"));
		NN<DB::DBReader> r;
		if (this->ExecuteReader(sql.ToCString()).SetTo(r))
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
