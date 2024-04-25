#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Data/FastStringMap.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/PostgreSQLConn.h"
#include "DB/ReadingDBTool.h"
#include "DB/SQL/CreateTableCommand.h"
#include "DB/SQL/SQLCommand.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Math/Math.h"
#include "Math/Geometry/Point.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

#include <stdio.h>

void DB::ReadingDBTool::AddLogMsgC(const UTF8Char *msg, UOSInt msgLen, IO::LogHandler::LogLevel logLev)
{
	NN<Text::String> s;
	if (log->HasHandler())
	{
		if (logPrefix.SetTo(s))
		{
			Text::StringBuilderUTF8 str;
			str.Append(s);
			str.AppendC(msg, msgLen);
			log->LogMessage(str.ToCString(), logLev);
		}
		else
		{
			log->LogMessage({msg, msgLen}, logLev);
		}
	}
}

DB::ReadingDBTool::ReadingDBTool(NN<DB::DBConn> db, Bool needRelease, NN<IO::LogTool> log, Text::CString logPrefix) : ReadingDB(db->GetSourceNameObj())
{
	this->db = db;
	this->currDBName = 0;
	this->needRelease = needRelease;
	this->log = log;
	this->lastReader = 0;
	this->readerCnt = 0;
	this->readerFail = 0;
	this->openFail = 0;
	this->logPrefix = Text::String::NewOrNull(logPrefix);
	this->isWorking = false;
	this->workId = 0;
	this->trig = 0;
	this->dataCnt = 0;
	this->sqlType = db->GetSQLType();
	this->axisAware = db->IsAxisAware();
	switch (this->sqlType)
	{
	case DB::SQLType::Access:
		this->AddLogMsgC(UTF8STRC("Server type is Access"), IO::LogHandler::LogLevel::Command);
		break;
	case DB::SQLType::MSSQL:
		this->AddLogMsgC(UTF8STRC("Server type is MSSQL"), IO::LogHandler::LogLevel::Command);
		break;
	case DB::SQLType::MySQL:
		this->AddLogMsgC(UTF8STRC("Server type is MySQL"), IO::LogHandler::LogLevel::Command);
		if (this->axisAware)
		{
			this->AddLogMsgC(UTF8STRC("DB is Axis-Aware"), IO::LogHandler::LogLevel::Command);
		}
		else
		{
			this->AddLogMsgC(UTF8STRC("DB is not Axis-Aware"), IO::LogHandler::LogLevel::Command);
		}
		break;
	case DB::SQLType::Oracle:
		this->AddLogMsgC(UTF8STRC("Server type is Oracle"), IO::LogHandler::LogLevel::Command);
		break;
	case DB::SQLType::SQLite:
		this->AddLogMsgC(UTF8STRC("Server type is SQLite"), IO::LogHandler::LogLevel::Command);
		break;
	case DB::SQLType::WBEM:
		this->AddLogMsgC(UTF8STRC("Server type is WBEM"), IO::LogHandler::LogLevel::Command);
		break;
	case DB::SQLType::MDBTools:
		this->AddLogMsgC(UTF8STRC("Server type is MDBTools"), IO::LogHandler::LogLevel::Command);
		break;
	case DB::SQLType::PostgreSQL:
		this->AddLogMsgC(UTF8STRC("Server type is PostgreSQL"), IO::LogHandler::LogLevel::Command);
		break;
	case DB::SQLType::Unknown:
	default:
		this->AddLogMsgC(UTF8STRC("Server type is Unknown"), IO::LogHandler::LogLevel::Error);
		break;
	}
}

UOSInt DB::ReadingDBTool::SplitMySQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr)
{
	UOSInt currCnt = 0;
	OSInt quoteType = 0;
	Bool quoted = false;
	Bool cmdStart = true;
	Bool lineStart = true;
	Bool comment = false;
	UTF8Char c;

	while ((c = *oriStr) != 0)
	{
		if (comment)
		{
			if (c == '\r' || c == '\n')
				comment = false;
		}
		else
		{
			if (c == ' ')
			{
			}
			else if (c == '\r' || c == '\n')
			{
				if (quoted)
				{
				}
				else
				{
					if (cmdStart)
					{
						lineStart = true;
					}
				}
			}
			else if (c == ';')
			{
				if (quoted)
				{
				}
				else
				{
					*oriStr = 0;
					cmdStart = true;
					lineStart = true;
				}
			}
			else if (c == '#' && lineStart)
			{
				comment = true;
			}
			else
			{
				if (cmdStart)
				{
					outStrs[currCnt++] = oriStr;
					if (currCnt >= maxCnt)
						break;
				}

				if (c == '\\')
				{
					if (quoted && quoteType != 0)
					{
						c = *++oriStr;
						if (c == 0)
							break;
					}
				}
				else if (c == '`')
				{
					if (!quoted)
					{
						quoted = true;
						quoteType = 0;
					}
					else if (quoteType == 0)
					{
						quoted = false;
					}
				}
				else if (c == '\'')
				{
					if (!quoted)
					{
						quoted = true;
						quoteType = 1;
					}
					else if (quoteType == 1)
					{
						quoted = false;
					}
				}
				else if (c == '"')
				{
					if (!quoted)
					{
						quoted = true;
						quoteType = 2;
					}
					else if (quoteType == 2)
					{
						quoted = false;
					}
				}

				cmdStart = false;
				lineStart = false;
			}
		}
		oriStr++;
	}
	return currCnt;
}

UOSInt DB::ReadingDBTool::SplitMSSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr)
{
	UOSInt currCnt = 0;
	OSInt quoteType = 0;
	Bool quoted = false;
	Bool cmdStart = true;
	Bool lineStart = true;
	Bool comment = false;
	UTF8Char c;

	while ((c = *oriStr) != 0)
	{
		if (comment)
		{
			if (c == '\r' || c == '\n')
				comment = false;
		}
		else
		{
			if (c == ' ')
			{
			}
			else if (c == '\r' || c == '\n')
			{
				if (quoted)
				{
				}
				else
				{
					if (cmdStart)
					{
						lineStart = true;
					}
				}
			}
			else if (c == ';')
			{
				if (quoted)
				{
				}
				else
				{
					*oriStr = 0;
					cmdStart = true;
					lineStart = true;
				}
			}
			else if (c == '#' && lineStart)
			{
				comment = true;
			}
			else
			{
				if (cmdStart)
				{
					outStrs[currCnt++] = oriStr;
					if (currCnt >= maxCnt)
						break;
				}

				if (c == '[' && !quoted)
				{
					quoted = true;
					quoteType = 0;
				}
				else if (c == ']' && quoted && quoteType == 0)
				{
					if (oriStr[1] == ']')
					{
						oriStr++;
					}
					else
					{
						quoted = false;
					}
				}
				else if (c == '\'')
				{
					if (!quoted)
					{
						quoted = true;
						quoteType = 1;
					}
					else if (quoteType == 1)
					{
						if (oriStr[1] == '\'')
						{
							oriStr++;
						}
						else
						{
							quoted = false;
						}
					}
				}
				else if (c == '"')
				{
					if (!quoted)
					{
						quoted = true;
						quoteType = 2;
					}
					else if (quoteType == 2)
					{
						if (oriStr[1] == '"')
						{
							oriStr++;
						}
						else
						{
							quoted = false;
						}
					}
				}

				cmdStart = false;
				lineStart = false;
			}
		}
		oriStr++;
	}
	return currCnt;
}

UOSInt DB::ReadingDBTool::SplitUnkSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr)
{
	return Text::StrSplit(outStrs, maxCnt, oriStr, ';');
}

DB::ReadingDBTool::~ReadingDBTool()
{
	OPTSTR_DEL(this->logPrefix);
	SDEL_STRING(this->currDBName);
	if (this->needRelease)
	{
		db.Delete();
	}
}

void DB::ReadingDBTool::SetFailTrigger(DB::ReadingDBTool::SQLFailedFunc trig)
{
	this->trig = trig;
}

Optional<DB::DBReader> DB::ReadingDBTool::ExecuteReader(Text::CStringNN sqlCmd)
{
	if (this->log->HasHandler())
	{
		Text::StringBuilderUTF8 logMsg;
		logMsg.AppendC(UTF8STRC("ExecuteReader: "));
		logMsg.Append(sqlCmd);
		AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::LogHandler::LogLevel::Raw);
	}

	Data::Timestamp t1 = Data::Timestamp::UtcNow();
	Data::Timestamp t2 = Data::Timestamp::UtcNow();
	NN<DB::DBReader> r;
	if (this->db->ExecuteReader(sqlCmd).SetTo(r))
	{
		Data::Timestamp t3 = Data::Timestamp::UtcNow();
		dataCnt += 1;
		if (t3.DiffMS(t2) >= 1000)
		{
			UTF8Char buff[256];
			UTF8Char *ptr;
			ptr = Text::StrConcatC(buff, UTF8STRC("SQL R t1 = "));
			ptr = Text::StrInt32(ptr, (Int32)t2.DiffMS(t1));
			ptr = Text::StrConcatC(ptr, UTF8STRC(", t2 = "));
			ptr = Text::StrInt32(ptr, (Int32)t3.DiffMS(t2));
			AddLogMsgC(buff, (UOSInt)(ptr - buff), IO::LogHandler::LogLevel::Command);
		}
		readerCnt += 1;
		readerFail = 0;
		openFail = 0;
		lastReader = r;
		return r;
	}
	else
	{
		if (this->log->HasHandler())
		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.AppendC(UTF8STRC("Cannot execute the sql command: "));
			logMsg.Append(sqlCmd);
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::LogHandler::LogLevel::Error);

			logMsg.ClearStr();
			logMsg.AppendC(UTF8STRC("Exception detail: "));
			this->lastErrMsg.ClearStr();
			this->db->GetLastErrorMsg(this->lastErrMsg);
			logMsg.AppendSB(this->lastErrMsg);
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::LogHandler::LogLevel::ErrorDetail);
		}

		readerFail += 1;
		if (readerFail >= 2)
		{
			NN<DB::DBReader> r;
			if (lastReader.SetTo(r))
			{
				AddLogMsgC(UTF8STRC("Automatically closed last reader"), IO::LogHandler::LogLevel::Action);

				this->CloseReader(r);
			}
		}

		Bool isData = this->db->IsLastDataError();
		if (!isData)
		{
			this->db->Reconnect();
		}

		if (trig)
			trig(sqlCmd, DB::ReadingDBTool::ReaderTrigger);
		return 0;
	}
}

void DB::ReadingDBTool::CloseReader(NN<DB::DBReader> r)
{
	this->db->CloseReader(r);
	this->lastReader = 0;
	this->readerCnt = 0;
}

DB::SQLType DB::ReadingDBTool::GetSQLType() const
{
	return this->sqlType;
}

Bool DB::ReadingDBTool::IsAxisAware() const
{
	return this->axisAware;
}

Bool DB::ReadingDBTool::IsDataError(const UTF8Char *errCode)
{
	if (errCode == 0)
		return false;
	if (Text::StrEqualsC(errCode, 5, UTF8STRC("23000")))
		return true;
	if (Text::StrEqualsC(errCode, 5, UTF8STRC("42000")))
		return true;
	if (Text::StrEqualsC(errCode, 5, UTF8STRC("HY000")))
		return true;
	return false;
}

void DB::ReadingDBTool::GetLastErrorMsg(NN<Text::StringBuilderUTF8> sb)
{
	sb->Append(this->lastErrMsg);
}

NN<DB::DBConn> DB::ReadingDBTool::GetDBConn()
{
	return this->db;
}

Int8 DB::ReadingDBTool::GetTzQhr() const
{
	return this->db->GetTzQhr();
}

void DB::ReadingDBTool::Reconnect()
{
	this->db->Reconnect();
}

Bool DB::ReadingDBTool::IsDBTool() const
{
	return true;
}

UTF8Char *DB::ReadingDBTool::DBColUTF8(UTF8Char *sqlstr, const UTF8Char *colName)
{
	return DB::DBUtil::SDBColUTF8(sqlstr, colName, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBColW(UTF8Char *sqlstr, const WChar *colName)
{
	return DB::DBUtil::SDBColW(sqlstr, colName, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBTrim(UTF8Char *sqlstr, Text::CString val)
{
	return DB::DBUtil::SDBTrim(sqlstr, val, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBStrUTF8(UTF8Char *sqlStr, const UTF8Char *val)
{
	return DB::DBUtil::SDBStrUTF8(sqlStr, val, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBStrW(UTF8Char *sqlStr, const WChar *val)
{
	return DB::DBUtil::SDBStrW(sqlStr, val, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBDbl(UTF8Char *sqlStr, Double val)
{
	return DB::DBUtil::SDBDbl(sqlStr, val, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBSng(UTF8Char *sqlStr, Single val)
{
	return DB::DBUtil::SDBSng(sqlStr, val, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBInt32(UTF8Char *sqlStr, Int32 val)
{
	return DB::DBUtil::SDBInt32(sqlStr, val, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBInt64(UTF8Char *sqlStr, Int64 val)
{
	return DB::DBUtil::SDBInt64(sqlStr, val, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBBool(UTF8Char *sqlStr, Bool val)
{
	return DB::DBUtil::SDBBool(sqlStr, val, this->sqlType);
}

UTF8Char *DB::ReadingDBTool::DBDateTime(UTF8Char *sqlStr, Data::DateTime *val)
{
	return DB::DBUtil::SDBDateTime(sqlStr, val, this->sqlType, (Int8)this->GetTzQhr());
}

UInt32 DB::ReadingDBTool::GetDataCnt()
{
	return this->dataCnt;
}

Optional<DB::DBReader> DB::ReadingDBTool::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	{
		Text::StringBuilderUTF8 logMsg;
		logMsg.AppendC(UTF8STRC("QueryTableData: "));
		if (schemaName.leng > 0)
		{
			logMsg.Append(schemaName);
			logMsg.AppendUTF8Char('.');
		}
		logMsg.Append(tableName);
		AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::LogHandler::LogLevel::Raw);
	}

	Data::Timestamp t1 = Data::Timestamp::UtcNow();
	Data::Timestamp t2 = Data::Timestamp::UtcNow();
	NN<DB::DBReader> r;
	if (this->db->QueryTableData(schemaName, tableName, columnNames, ofst, maxCnt, ordering, condition).SetTo(r))
	{
		Data::Timestamp t3 = Data::Timestamp::UtcNow();
		dataCnt += 1;
		if (t3.DiffMS(t2) >= 1000)
		{
			UTF8Char buff[256];
			UTF8Char *ptr;
			ptr = Text::StrConcatC(buff, UTF8STRC("SQL R t1 = "));
			ptr = Text::StrInt32(ptr, (Int32)t2.DiffMS(t1));
			ptr = Text::StrConcatC(ptr, UTF8STRC(", t2 = "));
			ptr = Text::StrInt32(ptr, (Int32)t3.DiffMS(t2));
			AddLogMsgC(buff, (UOSInt)(ptr - buff), IO::LogHandler::LogLevel::Command);
		}
		readerCnt += 1;
		readerFail = 0;
		openFail = 0;
		lastReader = r;
		return r;
	}
	else
	{
		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.AppendC(UTF8STRC("Cannot get table data: "));
			logMsg.Append(tableName);
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::LogHandler::LogLevel::Error);
		}

		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.AppendC(UTF8STRC("Exception detail: "));
			this->lastErrMsg.ClearStr();
			this->db->GetLastErrorMsg(this->lastErrMsg);
			logMsg.AppendSB(this->lastErrMsg);
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::LogHandler::LogLevel::ErrorDetail);
		}

		readerFail += 1;
		if (readerFail >= 2)
		{
			NN<DB::DBReader> r;
			if (lastReader.SetTo(r))
			{
				AddLogMsgC(UTF8STRC("Automatically closed last reader"), IO::LogHandler::LogLevel::Action);

				this->CloseReader(r);
			}
		}

		Bool isData = this->db->IsLastDataError();
		if (!isData)
		{
			this->db->Reconnect();
		}
		return 0;
	}
}

UOSInt DB::ReadingDBTool::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> arr)
{
	if (this->sqlType == DB::SQLType::MSSQL)
	{
		UOSInt ret = 0;
		DB::SQLBuilder sql(DB::SQLType::MSSQL, this->axisAware, this->GetTzQhr());
		sql.AppendCmdC(CSTR("select TABLE_SCHEMA, TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_TYPE='BASE TABLE' and TABLE_SCHEMA="));
		if (schemaName.leng == 0)
		{
			sql.AppendStrC(CSTR("dbo"));
		}
		else
		{
			sql.AppendStrC(schemaName);
		}
		NN<DB::DBReader> r;
		if (this->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(1, sb))
				{
					arr->Add(Text::String::New(sb.ToCString()));
					ret++;
				}
			}
			this->CloseReader(r);
		}
		return ret;
	}
	else if (this->sqlType == DB::SQLType::MySQL)
	{
		if (schemaName.leng != 0)
			return 0;
		NN<DB::DBReader> r;
		if (this->ExecuteReader(CSTR("show tables")).SetTo(r))
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, sb))
				{
					arr->Add(Text::String::New(sb.ToCString()));
					ret++;
				}
			}
			this->CloseReader(r);
			return ret;
		}
		else
		{
			return 0;
		}
	}
	else if (this->sqlType == DB::SQLType::SQLite)
	{
		if (schemaName.leng != 0)
			return 0;
		NN<DB::DBReader> r;
		if (this->ExecuteReader(CSTR("select name from sqlite_master where type = 'table'")).SetTo(r))
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, sb))
				{
					arr->Add(Text::String::New(sb.ToCString()));
					ret++;
				}
			}
			this->CloseReader(r);
			return ret;
		}
		else
		{
			return 0;
		}
	}
	else if (this->sqlType == DB::SQLType::Access || this->sqlType == DB::SQLType::MDBTools)
	{
		if (schemaName.leng != 0)
			return 0;
		NN<DB::DBReader> r;
		if (this->ExecuteReader(CSTR("select name, type from MSysObjects where type = 1")).SetTo(r))
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				//Int32 type = r->GetInt32(1);
				if (r->GetStr(0, sb))
				{
					arr->Add(Text::String::New(sb.ToCString()));
					ret++;
				}
			}
			this->CloseReader(r);
			return ret;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return this->db->QueryTableNames(schemaName, arr);
	}
}

UOSInt DB::ReadingDBTool::QuerySchemaNames(NN<Data::ArrayListStringNN> arr)
{
	if (this->sqlType == DB::SQLType::PostgreSQL)
	{
		NN<DB::DBReader> r;
		if (this->ExecuteReader(CSTR("SELECT nspname FROM pg_catalog.pg_namespace")).SetTo(r))
		{
			UOSInt ret = 0;
			NN<Text::String> s;
			while (r->ReadNext())
			{
				if (r->GetNewStr(0).SetTo(s))
				{
					arr->Add(s);
					ret++;
				}
			}
			this->CloseReader(r);
			return ret;
		}
		else
		{
			return 0;
		}
	}
	else if (this->sqlType == DB::SQLType::MSSQL)
	{
		NN<DB::DBReader> r;
		if (this->ExecuteReader(CSTR("select s.name from sys.schemas s")).SetTo(r))
		{
			UOSInt ret = 0;
			NN<Text::String> s;
			while (r->ReadNext())
			{
				if (r->GetNewStr(0).SetTo(s))
				{
					arr->Add(s);
					ret++;
				}
			}
			this->CloseReader(r);
			return ret;
		}
		else
		{
			return 0;
		}
		return 0;
	}
	else
	{
		return 0;
	}
}

DB::TableDef *DB::ReadingDBTool::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	return this->db->GetTableDef(schemaName, tableName);
}

UOSInt DB::ReadingDBTool::GetDatabaseNames(NN<Data::ArrayListStringNN> arr)
{
	NN<DB::DBReader> r;
	switch (this->sqlType)
	{
	case DB::SQLType::MSSQL:
		if (this->ExecuteReader(CSTR("select name from master.dbo.sysdatabases")).SetTo(r))
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, sb))
				{
					arr->Add(Text::String::New(sb.ToCString()));
					ret++;
				}
			}
			this->CloseReader(r);
			return ret;
		}
		else
		{
			return 0;
		}
	case DB::SQLType::MySQL:
		if (this->ExecuteReader(CSTR("show databases")).SetTo(r))
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, sb))
				{
					arr->Add(Text::String::New(sb.ToCString()));
					ret++;
				}
			}
			this->CloseReader(r);
			return ret;
		}
		else
		{
			return 0;
		}
	case DB::SQLType::SQLite:
	{
		Text::StringBuilderUTF8 sb;
		NN<Text::String> name = this->db->GetSourceNameObj();
		UOSInt i = name->LastIndexOf((UTF8Char)IO::Path::PATH_SEPERATOR);
		sb.AppendC(&name->v[i + 1], name->leng - i - 1);
		i = sb.IndexOf('.');
		if (i != INVALID_INDEX)
		{
			sb.RemoveChars(sb.GetLength() - (UOSInt)i);
		}
		arr->Add(Text::String::New(sb.ToCString()));
		return 1;
	}
	case DB::SQLType::PostgreSQL:
		if (this->ExecuteReader(CSTR("SELECT datname FROM pg_database")).SetTo(r))
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, sb))
				{
					arr->Add(Text::String::New(sb.ToCString()));
					ret++;
				}
			}
			this->CloseReader(r);
			return ret;
		}
		else
		{
			return 0;
		}
	case DB::SQLType::Oracle:
	case DB::SQLType::Unknown:
	case DB::SQLType::Access:
	case DB::SQLType::WBEM:
	case DB::SQLType::MDBTools:
	default:
		return 0;
	}
}

void DB::ReadingDBTool::ReleaseDatabaseNames(NN<Data::ArrayListStringNN> arr)
{
	arr->FreeAll();
}

Bool DB::ReadingDBTool::ChangeDatabase(Text::CString databaseName)
{
	UTF8Char sbuff[256];
	if (this->sqlType == DB::SQLType::MSSQL)
	{
		UTF8Char *sptr = this->DBColUTF8(Text::StrConcatC(sbuff, UTF8STRC("use ")), databaseName.v);
		NN<DB::DBReader> r;
		if (this->ExecuteReader(CSTRP(sbuff, sptr)).SetTo(r))
		{
			OSInt rowChg = r->GetRowChanged();
			this->CloseReader(r);
			if (rowChg >= -1)
			{
				SDEL_STRING(this->currDBName);
				this->currDBName = Text::String::New(databaseName).Ptr();
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	else if (this->sqlType == DB::SQLType::MySQL)
	{
		UTF8Char *sptr = this->DBColUTF8(Text::StrConcatC(sbuff, UTF8STRC("use ")), databaseName.v);
		NN<DB::DBReader> r;
		if (this->ExecuteReader(CSTRP(sbuff, sptr)).SetTo(r))
		{
			OSInt rowChg = r->GetRowChanged();
			this->CloseReader(r);
			if (rowChg >= -1)
			{
				SDEL_STRING(this->currDBName);
				this->currDBName = Text::String::New(databaseName).Ptr();
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	else if (this->sqlType == DB::SQLType::PostgreSQL)
	{
		if (this->db->GetConnType() == DB::DBConn::CT_POSTGRESQL)
		{
			if (NN<DB::PostgreSQLConn>::ConvertFrom(this->db)->ChangeDatabase(databaseName))
			{
				SDEL_STRING(this->currDBName);
				this->currDBName = Text::String::New(databaseName).Ptr();
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
		UTF8Char *sptr = this->DBStrUTF8(Text::StrConcatC(sbuff, UTF8STRC("SET search_path = ")), databaseName.v);
		NN<DB::DBReader> r;
		if (this->ExecuteReader(CSTRP(sbuff, sptr)).SetTo(r))
		{
			OSInt rowChg = r->GetRowChanged();
			this->CloseReader(r);
			if (rowChg >= -1)
			{
				SDEL_STRING(this->currDBName);
				this->currDBName = Text::String::New(databaseName).Ptr();
				return true;
			}
			return false;
		}
		else
		{
			return false;
		}
	}
	return false;
}

Text::String *DB::ReadingDBTool::GetCurrDBName()
{
	return this->currDBName;
}

Bool DB::ReadingDBTool::GetDBCollation(Text::CString databaseName, Collation *collation)
{
	NN<DB::DBReader> r;
	if (this->sqlType == DB::SQLType::MySQL)
	{
		Bool succ = false;
		DB::SQLBuilder sql(this->sqlType, this->axisAware, this->GetTzQhr());
		sql.AppendCmdC(CSTR("select DEFAULT_COLLATION_NAME from information_schema.SCHEMATA where SCHEMA_NAME = "));
		sql.AppendStrC(databaseName);
		if (this->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			if (r->ReadNext())
			{
				NN<Text::String> s;
				if (r->GetNewStr(0).SetTo(s))
				{
					succ = DB::DBUtil::CollationParseMySQL(s->ToCString(), collation);
					s->Release();
				}
			}
			this->CloseReader(r);
			return succ;
		}
	}
	return false;
}

UOSInt DB::ReadingDBTool::GetVariables(NN<Data::ArrayList<Data::TwinItem<Optional<Text::String>, Optional<Text::String>>>> vars)
{
	UOSInt ret = 0;
	NN<DB::DBReader> r;
	if (this->sqlType == DB::SQLType::MySQL)
	{
		if (this->ExecuteReader(CSTR("show variables")).SetTo(r))
		{
			NN<Text::String> name;
			while (r->ReadNext())
			{
				if (r->GetNewStr(0).SetTo(name))
					vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(name, r->GetNewStr(1)));
				ret++;
			}
			this->CloseReader(r);
		}
	}
	else if (this->sqlType == DB::SQLType::MSSQL)
	{
		if (this->ExecuteReader(CSTR("select @@CONNECTIONS, @@CPU_BUSY, @@IDLE, @@IO_BUSY, @@PACKET_ERRORS, @@PACK_RECEIVED, @@PACK_SENT, @@TIMETICKS, @@TOTAL_ERRORS, @@TOTAL_READ, @@TOTAL_WRITE, @@DATEFIRST, @@DBTS, @@LANGID, @@LANGUAGE, @@LOCK_TIMEOUT, @@MAX_CONNECTIONS, @@MAX_PRECISION, @@NESTLEVEL, @@OPTIONS, @@REMSERVER, @@SERVERNAME, @@SERVICENAME, @@SPID, @@TEXTSIZE, @@VERSION")).SetTo(r))
		{
			if (r->ReadNext())
			{
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("CONNECTIONS")), r->GetNewStr(0)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("CPU_BUSY")), r->GetNewStr(1)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("IDLE")), r->GetNewStr(2)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("IO_BUSY")), r->GetNewStr(3)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("PACKET_ERRORS")), r->GetNewStr(4)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("PACK_RECEIVED")), r->GetNewStr(5)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("PACK_SENT")), r->GetNewStr(6)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("TIMETICKS")), r->GetNewStr(7)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("TOTAL_ERRORS")), r->GetNewStr(8)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("TOTAL_READ")), r->GetNewStr(9)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("TOTAL_WRITE")), r->GetNewStr(10)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("DATEFIRST")), r->GetNewStr(11)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("DBTS")), r->GetNewStr(12)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("LANGID")), r->GetNewStr(13)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("LANGUAGE")), r->GetNewStr(14)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("LOCK_TIMEOUT")), r->GetNewStr(15)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("MAX_CONNECTIONS")), r->GetNewStr(16)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("MAX_PRECISION")), r->GetNewStr(17)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("NESTLEVEL")), r->GetNewStr(18)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("OPTIONS")), r->GetNewStr(19)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("REMSERVER")), r->GetNewStr(20)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("SERVERNAME")), r->GetNewStr(21)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("SERVICENAME")), r->GetNewStr(22)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("SPID")), r->GetNewStr(23)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("TEXTSIZE")), r->GetNewStr(24)));
				vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(Text::String::New(CSTR("VERSION")), r->GetNewStr(25)));
				ret = 26;
			}
			this->CloseReader(r);
		}
	}
	else if (this->sqlType == DB::SQLType::PostgreSQL)
	{
		if (this->ExecuteReader(CSTR("select name, setting from pg_Settings")).SetTo(r))
		{
			NN<Text::String> name;
			while (r->ReadNext())
			{
				if (r->GetNewStr(0).SetTo(name))
					vars->Add(Data::TwinItem<Optional<Text::String>, Optional<Text::String>>(name, r->GetNewStr(1)));
				ret++;
			}
			this->CloseReader(r);
		}
	}
	return ret;
}

void DB::ReadingDBTool::FreeVariables(NN<Data::ArrayList<Data::TwinItem<Optional<Text::String>, Optional<Text::String>>>> vars)
{
	Data::TwinItem<Optional<Text::String>, Optional<Text::String>> item = vars->GetItem(0);
	UOSInt i = vars->GetCount();
	while (i-- > 0)
	{
		item = vars->GetItem(i);
		OPTSTR_DEL(item.key);
		OPTSTR_DEL(item.value);
	}
	vars->Clear();
}

UOSInt DB::ReadingDBTool::GetConnectionInfo(NN<Data::ArrayListNN<ConnectionInfo>> conns)
{
	UOSInt ret = 0;
	NN<ConnectionInfo> conn;
	NN<DB::DBReader> r;
	if (this->sqlType == DB::SQLType::MySQL)
	{
		if (this->ExecuteReader(CSTR("show processlist")).SetTo(r))
		{
			while (r->ReadNext())
			{
				conn = MemAllocNN(ConnectionInfo);
				conn->id = r->GetInt32(0);
				conn->user = r->GetNewStr(1);
				conn->clientHostName = r->GetNewStr(2);
				conn->dbName = r->GetNewStr(3);
				conn->cmd = r->GetNewStr(4);
				conn->timeUsed = r->GetInt32(5);
				conn->status = r->GetNewStr(6);
				conn->sql = r->GetNewStr(7);
				conns->Add(conn);
				ret++;
			}
			this->CloseReader(r);
		}
	}
	else if (this->sqlType == DB::SQLType::MSSQL)
	{
		if (this->ExecuteReader(CSTR("exec sp_who")).SetTo(r))
		{
			while (r->ReadNext())
			{
				conn = MemAllocNN(ConnectionInfo);
				conn->id = r->GetInt32(0);
				conn->status = r->GetNewStr(2);
				conn->user = r->GetNewStr(3);
				conn->clientHostName = r->GetNewStr(4);
				conn->dbName = r->GetNewStr(6);
				conn->cmd = r->GetNewStr(7);
				conn->timeUsed = 0;
				conn->sql = 0;
				conns->Add(conn);
				ret++;
			}
			this->CloseReader(r);
		}
	}
	else if (this->sqlType == DB::SQLType::PostgreSQL)
	{
		if (this->ExecuteReader(CSTR("select pid, state, usename, client_addr, datname, wait_event, query from pg_stat_activity")).SetTo(r))
		{
			while (r->ReadNext())
			{
				conn = MemAllocNN(ConnectionInfo);
				conn->id = r->GetInt32(0);
				conn->status = r->GetNewStr(1);
				conn->user = r->GetNewStr(2);
				conn->clientHostName = r->GetNewStr(3);
				conn->dbName = r->GetNewStr(4);
				conn->cmd = r->GetNewStr(5);
				conn->timeUsed = 0;
				conn->sql = r->GetNewStr(6);
				conns->Add(conn);
				ret++;
			}
			this->CloseReader(r);
		}
	}
	return ret;	
}

void DB::ReadingDBTool::FreeConnectionInfo(NN<Data::ArrayListNN<ConnectionInfo>> conns)
{
	NN<ConnectionInfo> conn;
	UOSInt i = conns->GetCount();
	while (i-- > 0)
	{
		conn = conns->GetItemNoCheck(i);
		OPTSTR_DEL(conn->status);
		OPTSTR_DEL(conn->user);
		OPTSTR_DEL(conn->clientHostName);
		OPTSTR_DEL(conn->dbName);
		OPTSTR_DEL(conn->cmd);
		OPTSTR_DEL(conn->sql);
		MemFreeNN(conn);
	}
	conns->Clear();
}

UOSInt DB::ReadingDBTool::SplitSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr)
{
	if (this->sqlType == DB::SQLType::MySQL)
	{
		return SplitMySQL(outStrs, maxCnt, oriStr);
	}
	else if (this->sqlType == DB::SQLType::MSSQL)
	{
		return SplitMSSQL(outStrs, maxCnt, oriStr);
	}
	else
	{
		return SplitUnkSQL(outStrs, maxCnt, oriStr);
	}
}

Bool DB::ReadingDBTool::CanModify()
{
	return false;
}
