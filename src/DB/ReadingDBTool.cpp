#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/ReadingDBTool.h"
#include "DB/SQL/CreateTableCommand.h"
#include "DB/SQL/SQLCommand.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Math/Math.h"
#include "Math/Point.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"

void DB::ReadingDBTool::AddLogMsgC(const UTF8Char *msg, UOSInt msgLen, IO::ILogHandler::LogLevel logLev)
{
	if (log)
	{
		if (logPrefix)
		{
			Text::StringBuilderUTF8 str;
			str.Append(logPrefix);
			str.AppendC(msg, msgLen);
			log->LogMessage(str.ToCString(), logLev);
		}
		else
		{
			log->LogMessage({msg, msgLen}, logLev);
		}
	}
}

DB::ReadingDBTool::ReadingDBTool(DB::DBConn *db, Bool needRelease, IO::LogTool *log, Text::CString logPrefix)
{
	this->db = db;
	this->needRelease = needRelease;
	this->log = log;
	this->lastReader = 0;
	this->readerCnt = 0;
	this->readerFail = 0;
	this->openFail = 0;
	NEW_CLASS(this->lastErrMsg, Text::StringBuilderUTF8());
	this->logPrefix = Text::String::NewOrNull(logPrefix);
	this->isWorking = false;
	this->workId = 0;
	this->trig = 0;
	this->dataCnt = 0;
	this->svrType = db->GetSvrType();
	if (this->svrType == DB::DBUtil::ServerType::Access)
	{
		this->AddLogMsgC(UTF8STRC("Server type is Access"), IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::ServerType::MSSQL)
	{
		this->AddLogMsgC(UTF8STRC("Server type is MSSQL"), IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::ServerType::MySQL)
	{
		this->AddLogMsgC(UTF8STRC("Server type is MySQL"), IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::ServerType::Oracle)
	{
		this->AddLogMsgC(UTF8STRC("Server type is Oracle"), IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::ServerType::Text)
	{
		this->AddLogMsgC(UTF8STRC("Server type is Text"), IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::ServerType::SQLite)
	{
		this->AddLogMsgC(UTF8STRC("Server type is SQLite"), IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else
	{
		this->AddLogMsgC(UTF8STRC("Server type is Unknown"), IO::ILogHandler::LOG_LEVEL_ERROR);
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
	SDEL_STRING(this->logPrefix);
	if (this->db && this->needRelease)
	{
		DEL_CLASS(db);
		db = 0;
	}
	if (this->lastErrMsg)
	{
		DEL_CLASS(this->lastErrMsg);
		this->lastErrMsg = 0;
	}
}

void DB::ReadingDBTool::SetFailTrigger(DB::ReadingDBTool::SQLFailedFunc trig)
{
	this->trig = trig;
}

DB::DBReader *DB::ReadingDBTool::ExecuteReader(Text::CString sqlCmd)
{
	{
		Text::StringBuilderUTF8 logMsg;
		logMsg.AppendC(UTF8STRC("ExecuteReader: "));
		logMsg.Append(sqlCmd);
		AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
	if (this->db == 0)
	{
		readerCnt += 1;
		dataCnt += 1;
		return lastReader;
	}

	Data::DateTime t1;
	Data::DateTime t2;
	DB::DBReader *r = this->db->ExecuteReader(sqlCmd);
	if (r)
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
			AddLogMsgC(buff, (UOSInt)(ptr - buff), IO::ILogHandler::LOG_LEVEL_COMMAND);
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
			logMsg.AppendC(UTF8STRC("Cannot execute the sql command: "));
			logMsg.Append(sqlCmd);
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::ILogHandler::LOG_LEVEL_ERROR);
		}

		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.AppendC(UTF8STRC("Exception detail: "));
			this->lastErrMsg->ClearStr();
			this->db->GetErrorMsg(this->lastErrMsg);
			logMsg.AppendSB(this->lastErrMsg);
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
		}

		readerFail += 1;
		if (readerFail >= 2)
		{
			if (lastReader)
			{
				AddLogMsgC(UTF8STRC("Automatically closed last reader"), IO::ILogHandler::LOG_LEVEL_ACTION);

				this->CloseReader(lastReader);
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

void DB::ReadingDBTool::CloseReader(DB::DBReader *r)
{
	if (r)
	{
		this->db->CloseReader(r);
		this->lastReader = 0;
		this->readerCnt = 0;
	}
}

DB::DBUtil::ServerType DB::ReadingDBTool::GetSvrType()
{
	return this->svrType;
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

void DB::ReadingDBTool::GetLastErrorMsg(Text::StringBuilderUTF8 *sb)
{
	sb->Append(this->lastErrMsg);
}

DB::DBConn *DB::ReadingDBTool::GetDBConn()
{
	return this->db;
}

Int8 DB::ReadingDBTool::GetTzQhr()
{
	if (this->db)
	{
		return this->db->GetTzQhr();
	}
	else
	{
		return 0;
	}
}


UTF8Char *DB::ReadingDBTool::DBColUTF8(UTF8Char *sqlstr, const UTF8Char *colName)
{
	return DB::DBUtil::SDBColUTF8(sqlstr, colName, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBColW(UTF8Char *sqlstr, const WChar *colName)
{
	return DB::DBUtil::SDBColW(sqlstr, colName, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBTrim(UTF8Char *sqlstr, Text::CString val)
{
	return DB::DBUtil::SDBTrim(sqlstr, val, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBStrUTF8(UTF8Char *sqlStr, const UTF8Char *val)
{
	return DB::DBUtil::SDBStrUTF8(sqlStr, val, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBStrW(UTF8Char *sqlStr, const WChar *val)
{
	return DB::DBUtil::SDBStrW(sqlStr, val, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBDbl(UTF8Char *sqlStr, Double val)
{
	return DB::DBUtil::SDBDbl(sqlStr, val, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBSng(UTF8Char *sqlStr, Single val)
{
	return DB::DBUtil::SDBSng(sqlStr, val, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBInt32(UTF8Char *sqlStr, Int32 val)
{
	return DB::DBUtil::SDBInt32(sqlStr, val, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBInt64(UTF8Char *sqlStr, Int64 val)
{
	return DB::DBUtil::SDBInt64(sqlStr, val, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBBool(UTF8Char *sqlStr, Bool val)
{
	return DB::DBUtil::SDBBool(sqlStr, val, this->svrType);
}

UTF8Char *DB::ReadingDBTool::DBDate(UTF8Char *sqlStr, Data::DateTime *val)
{
	return DB::DBUtil::SDBDate(sqlStr, val, this->svrType, (Int8)this->GetTzQhr());
}

UInt32 DB::ReadingDBTool::GetDataCnt()
{
	return this->dataCnt;
}

DB::DBReader *DB::ReadingDBTool::QueryTableData(Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	{
		Text::StringBuilderUTF8 logMsg;
		logMsg.AppendC(UTF8STRC("GetTableData: "));
		logMsg.Append(tableName);
		AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
	if (this->db == 0)
	{
		readerCnt += 1;
		dataCnt += 1;
		return lastReader;
	}

	Data::DateTime t1;
	Data::DateTime t2;
	DB::DBReader *r = this->db->QueryTableData(tableName, columnNames, ofst, maxCnt, ordering, condition);
	if (r)
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
			AddLogMsgC(buff, (UOSInt)(ptr - buff), IO::ILogHandler::LOG_LEVEL_COMMAND);
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
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::ILogHandler::LOG_LEVEL_ERROR);
		}

		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.AppendC(UTF8STRC("Exception detail: "));
			this->lastErrMsg->ClearStr();
			this->db->GetErrorMsg(this->lastErrMsg);
			logMsg.AppendSB(this->lastErrMsg);
			AddLogMsgC(logMsg.ToString(), logMsg.GetLength(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
		}

		readerFail += 1;
		if (readerFail >= 2)
		{
			if (lastReader)
			{
				AddLogMsgC(UTF8STRC("Automatically closed last reader"), IO::ILogHandler::LOG_LEVEL_ACTION);

				this->CloseReader(lastReader);
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

UOSInt DB::ReadingDBTool::QueryTableNames(Data::ArrayList<Text::CString> *arr)
{
	if (this->svrType == DB::DBUtil::ServerType::MSSQL)
	{
		UOSInt ret = 0;
		DB::DBReader *r = this->ExecuteReader(CSTR("select TABLE_SCHEMA, TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_TYPE='BASE TABLE'"));
		if (r)
		{
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, &sb))
				{
					sb.AppendUTF8Char('.');
				}
				if (r->GetStr(1, &sb))
				{
					arr->Add(Text::CString(Text::StrCopyNewC(sb.ToString(), sb.GetLength()), sb.GetLength()));
					ret++;
				}
			}
			this->CloseReader(r);
		}
		return ret;
	}
	else if (this->svrType == DB::DBUtil::ServerType::MySQL)
	{
		DB::DBReader *r = this->ExecuteReader(CSTR("show tables"));
		if (r)
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, &sb))
				{
					arr->Add(Text::CString(Text::StrCopyNewC(sb.ToString(), sb.GetLength()), sb.GetLength()));
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
	else if (this->svrType == DB::DBUtil::ServerType::SQLite)
	{
		DB::DBReader *r = this->ExecuteReader(CSTR("select name from sqlite_master where type = 'table'"));
		if (r)
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, &sb))
				{
					arr->Add(Text::CString(Text::StrCopyNewC(sb.ToString(), sb.GetLength()), sb.GetLength()));
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
	else if (this->svrType == DB::DBUtil::ServerType::Access || this->svrType == DB::DBUtil::ServerType::MDBTools)
	{
		DB::DBReader *r = this->ExecuteReader(CSTR("select name, type from MSysObjects where type = 1"));
		if (r)
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				//Int32 type = r->GetInt32(1);
				if (r->GetStr(0, &sb))
				{
					arr->Add(Text::CString(Text::StrCopyNewC(sb.ToString(), sb.GetLength()), sb.GetLength()));
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
		Data::ArrayList<Text::CString> tables;
		Text::CString tableName;
		this->db->GetTableNames(&tables);
		UOSInt i = 0;
		UOSInt j = tables.GetCount();
		while (i < j)
		{
			tableName = tables.GetItem(i);
			arr->Add(Text::CString(Text::StrCopyNewC(tableName.v, tableName.leng), tableName.leng));
			i++;
		}
		return j;
	}
}

void DB::ReadingDBTool::ReleaseTableNames(Data::ArrayList<Text::CString> *arr)
{
	UOSInt i = arr->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(arr->RemoveAt(i).v);
	}
}

DB::TableDef *DB::ReadingDBTool::GetTableDef(Text::CString tableName)
{
	UTF8Char buff[256];
	UTF8Char *ptr;
	if (this->svrType == DB::DBUtil::ServerType::MySQL)
	{
		OSInt i = 4;
		DB::DBReader *r = 0;
		ptr = Text::StrConcatC(buff, UTF8STRC("show table status where Name = "));
		ptr = this->DBStrUTF8(ptr, tableName.v);

		while (i-- > 0 && r == 0)
		{
			r = this->ExecuteReader(CSTRP(buff, ptr));
		}
		if (r == 0)
			return 0;

		DB::TableDef *tab;

		if (r->ReadNext())
		{
			ptr = r->GetStr(0, buff, sizeof(buff));
			NEW_CLASS(tab, DB::TableDef(CSTRP(buff, ptr)));
			ptr = r->GetStr(1, buff, sizeof(buff));
			tab->SetEngine(CSTRP(buff, ptr));
			if (r->GetStr(17, buff, sizeof(buff)))
			{
				tab->SetComments(buff);
			}
			else
			{
				tab->SetComments(0);
			}
			if (r->GetStr(16, buff, sizeof(buff)))
			{
				tab->SetAttr(buff);
			}
			else
			{
				tab->SetAttr(0);
			}
			tab->SetSvrType(DB::DBUtil::ServerType::MySQL);
			tab->SetCharset(CSTR_NULL);
			this->CloseReader(r);
		}
		else
		{
			this->CloseReader(r);
			return 0;
		}
		DB::ColDef *col;
		ptr = Text::StrConcatC(buff, UTF8STRC("desc "));
		ptr = this->DBColUTF8(ptr, tableName.v);
		r = this->ExecuteReader(CSTRP(buff, ptr));
		if (r)
		{
			while (r->ReadNext())
			{
				ptr = r->GetStr(0, buff, sizeof(buff));
				NEW_CLASS(col, DB::ColDef(CSTRP(buff, ptr)));
				ptr = r->GetStr(2, buff, sizeof(buff));
				col->SetNotNull(Text::StrEqualsICaseC(buff, (UOSInt)(ptr - buff), UTF8STRC("NO")));
				ptr = r->GetStr(3, buff, sizeof(buff));
				col->SetPK(Text::StrEqualsICaseC(buff, (UOSInt)(ptr - buff), UTF8STRC("PRI")));
				if ((ptr = r->GetStr(4, buff, sizeof(buff))) != 0)
				{
					col->SetDefVal(CSTRP(buff, ptr));
				}
				else
				{
					col->SetDefVal(CSTR_NULL);
				}
				if ((ptr = r->GetStr(5, buff, sizeof(buff))) != 0)
				{
					if (Text::StrEqualsC(buff, (UOSInt)(ptr - buff), UTF8STRC("auto_increment")))
					{
						col->SetAutoInc(true);
						col->SetAttr(CSTR_NULL);
					}
					else
					{
						col->SetAttr(CSTRP(buff, ptr));
					}
				}
				else
				{
					col->SetAttr(CSTR_NULL);
				}
				r->GetStr(1, buff, sizeof(buff));
				UOSInt colSize;
				col->SetColType(DB::DBUtil::ParseColType(this->svrType, buff, &colSize));
				col->SetColSize(colSize);
				if (col->GetColType() == DB::DBUtil::CT_DateTime2)
				{
					if (col->IsNotNull())
					{
						col->SetNotNull(false);
					}
				}
				tab->AddCol(col);
			}
			this->CloseReader(r);
		}
		return tab;
	}
	else if (this->svrType == DB::DBUtil::ServerType::Oracle)
	{
		return 0;
	}
	else if (this->svrType == DB::DBUtil::ServerType::MSSQL)
	{
		Int32 i = 4;
		DB::DBReader *r = 0;
		UOSInt ind;
		ptr = Text::StrConcatC(buff, UTF8STRC("exec sp_columns "));
		ind = tableName.IndexOf('.');
		if (ind != INVALID_INDEX)
		{
			ptr = this->DBStrUTF8(ptr, &tableName.v[ind + 1]);
			ptr = Text::StrConcatC(ptr, UTF8STRC(", "));
			const UTF8Char *tmpPtr = Text::StrCopyNewC(tableName.v, (UOSInt)ind);
			ptr = this->DBStrUTF8(ptr, tmpPtr);
			Text::StrDelNew(tmpPtr);
		}
		else
		{
			ptr = this->DBStrUTF8(ptr, tableName.v);
		}
		while (i-- > 0 && r == 0)
		{
			r = this->ExecuteReader(CSTRP(buff, ptr));
		}
		if (r == 0)
			return 0;

		DB::TableDef *tab;
		NEW_CLASS(tab, DB::TableDef(tableName));
		tab->SetEngine(CSTR_NULL);
		tab->SetComments(0);
		tab->SetAttr(0);
		tab->SetCharset(CSTR_NULL);
		tab->SetSvrType(DB::DBUtil::ServerType::MSSQL);

		DB::ColDef *col;
		while (r->ReadNext())
		{
			ptr = r->GetStr(3, buff, sizeof(buff));
			NEW_CLASS(col, DB::ColDef(CSTRP(buff, ptr)));
			col->SetNotNull(!r->GetBool(10));
			col->SetPK(false);
			if ((ptr = r->GetStr(12, buff, sizeof(buff))) != 0)
			{
				if (*buff == '{')
				{
					ptr[-1] = 0;
					col->SetDefVal(CSTRP(&buff[1], ptr - 1));
				}
				else
				{
					col->SetDefVal(CSTRP(buff, ptr));
				}
			}
			col->SetColSize((UOSInt)r->GetInt32(6));
			r->GetStr(5, buff, sizeof(buff));
			if (Text::StrEndsWith(buff, (const UTF8Char*)" identity"))
			{
				col->SetAutoInc(true);
				buff[Text::StrCharCnt(buff) - 9] = 0;
			}
			col->SetColType(DB::DBUtil::ParseColType(this->svrType, buff, 0));
			tab->AddCol(col);
		}
		this->CloseReader(r);

		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("SELECT c.name AS column_name, i.name AS index_name, c.is_identity FROM sys.indexes i"));
		sb.AppendC(UTF8STRC(" inner join sys.index_columns ic  ON i.object_id = ic.object_id AND i.index_id = ic.index_id"));
		sb.AppendC(UTF8STRC(" inner join sys.columns c ON ic.object_id = c.object_id AND c.column_id = ic.column_id"));
		sb.AppendC(UTF8STRC(" WHERE i.is_primary_key = 1"));
		sb.AppendC(UTF8STRC(" and i.object_ID = OBJECT_ID('"));
		sb.Append(tableName);
		sb.AppendC(UTF8STRC("')"));
		r = 0;
		i = 4;
		while (i-- > 0 && r == 0)
		{
			r = this->ExecuteReader(sb.ToCString());
		}
		if (r == 0)
			return tab;

		UOSInt j;
		UOSInt k;
		while (r->ReadNext())
		{
			ptr = r->GetStr(0, buff, sizeof(buff));
			j = 0;
			k = tab->GetColCnt();
			while (j < k)
			{
				col = tab->GetCol(j);
				if (col->GetColName()->Equals(buff, (UOSInt)(ptr - buff)))
				{
					col->SetPK(true);
					break;
				}
				j++;
			}
		}
		this->CloseReader(r);

		return tab;
	}
	else if (this->svrType == DB::DBUtil::ServerType::Access)
	{
		return 0;
	}
	else if (this->svrType == DB::DBUtil::ServerType::SQLite)
	{
		DB::SQLBuilder sql(this->svrType, this->GetTzQhr());
		sql.AppendCmdC(CSTR("select sql from sqlite_master where type='table' and name="));
		sql.AppendStrC(tableName);
		DB::DBReader *r = this->db->ExecuteReader(sql.ToCString());
		if (r == 0)
		{
			return 0;
		}
		Text::StringBuilderUTF8 sb;
		if (r->ReadNext())
		{
			r->GetStr(0, &sb);
		}
		this->db->CloseReader(r);

		DB::TableDef *tab = 0;
		DB::SQL::SQLCommand *cmd = DB::SQL::SQLCommand::Parse(sb.ToString(), this->svrType);
		if (cmd)
		{
			if (cmd->GetCommandType() == DB::SQL::SQLCommand::CT_CREATE_TABLE)
			{
				tab = ((DB::SQL::CreateTableCommand*)cmd)->GetTableDef()->Clone();
			}
			DEL_CLASS(cmd);
		}
		return tab;
	}
	else
	{
		return 0;
	}
}

UOSInt DB::ReadingDBTool::GetDatabaseNames(Data::ArrayList<const UTF8Char*> *arr)
{
	if (this->svrType == DB::DBUtil::ServerType::MSSQL)
	{
		DB::DBReader *r = this->ExecuteReader(CSTR("select name from master.dbo.sysdatabases"));
		if (r)
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, &sb))
				{
					arr->Add(Text::StrCopyNew(sb.ToString()));
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
	else if (this->svrType == DB::DBUtil::ServerType::MySQL)
	{
		DB::DBReader *r = this->ExecuteReader(CSTR("show databases"));
		if (r)
		{
			UOSInt ret = 0;
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, &sb))
				{
					arr->Add(Text::StrCopyNew(sb.ToString()));
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
	else if (this->svrType == DB::DBUtil::ServerType::SQLite)
	{
		Text::StringBuilderUTF8 sb;
		Text::String *name = this->db->GetSourceNameObj();
		UOSInt i = name->LastIndexOf((UTF8Char)IO::Path::PATH_SEPERATOR);
		sb.AppendC(&name->v[i + 1], name->leng - i - 1);
		i = sb.IndexOf('.');
		if (i != INVALID_INDEX)
		{
			sb.RemoveChars(sb.GetLength() - (UOSInt)i);
		}
		arr->Add(Text::StrCopyNew(sb.ToString()));
		return 1;
	}
	else
	{
		return 0;
	}
}

void DB::ReadingDBTool::ReleaseDatabaseNames(Data::ArrayList<const UTF8Char*> *arr)
{
	UOSInt i = arr->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(arr->GetItem(i));
	}
	arr->Clear();
}

Bool DB::ReadingDBTool::ChangeDatabase(const UTF8Char *databaseName)
{
	UTF8Char sbuff[256];
	if (this->svrType == DB::DBUtil::ServerType::MSSQL)
	{
		UTF8Char *sptr = this->DBColUTF8(Text::StrConcatC(sbuff, UTF8STRC("use ")), databaseName);
		DB::DBReader *r = this->ExecuteReader(CSTRP(sbuff, sptr));
		if (r)
		{
			OSInt rowChg = r->GetRowChanged();
			this->CloseReader(r);
			return rowChg >= -1;
		}
		else
		{
			return false;
		}
	}
	else if (this->svrType == DB::DBUtil::ServerType::MySQL)
	{
		UTF8Char *sptr = this->DBColUTF8(Text::StrConcatC(sbuff, UTF8STRC("use ")), databaseName);
		DB::DBReader *r = this->ExecuteReader(CSTRP(sbuff, sptr));
		if (r)
		{
			OSInt rowChg = r->GetRowChanged();
			this->CloseReader(r);
			return rowChg >= -1;
		}
		else
		{
			return false;
		}
	}
	return false;
}

UOSInt DB::ReadingDBTool::SplitSQL(UTF8Char **outStrs, UOSInt maxCnt, UTF8Char *oriStr)
{
	if (this->svrType == DB::DBUtil::ServerType::MySQL)
	{
		return SplitMySQL(outStrs, maxCnt, oriStr);
	}
	else if (this->svrType == DB::DBUtil::ServerType::MSSQL)
	{
		return SplitMSSQL(outStrs, maxCnt, oriStr);
	}
	else
	{
		return SplitUnkSQL(outStrs, maxCnt, oriStr);
	}
}

void DB::ReadingDBTool::AppendColDef(DB::DBUtil::ServerType svrType, DB::SQLBuilder *sql, DB::ColDef *col)
{
	sql->AppendCol(col->GetColName()->v);
	sql->AppendCmdC(CSTR(" "));
	AppendColType(svrType, sql, col->GetColType(), col->GetColSize());
	if (col->IsNotNull())
	{
		sql->AppendCmdC(CSTR(" NOT NULL"));
	}
	if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		if (col->IsAutoInc())
		{
			sql->AppendCmdC(CSTR(" IDENTITY(1,1)"));
		}
	}
	else if (svrType == DB::DBUtil::ServerType::MySQL)
	{
		if (col->IsAutoInc())
		{
			sql->AppendCmdC(CSTR(" AUTO_INCREMENT"));
		}
	}
	else if (svrType == DB::DBUtil::ServerType::SQLite)
	{
		if (col->IsAutoInc() && col->IsPK())
		{
			sql->AppendCmdC(CSTR(" PRIMARY KEY"));
		}
		if (col->IsAutoInc() && (col->GetColType() == DB::DBUtil::CT_Int32 || col->GetColType() == DB::DBUtil::CT_UInt32))
		{
			sql->AppendCmdC(CSTR(" AUTOINCREMENT"));
		}
	}
	else if (svrType == DB::DBUtil::ServerType::Access)
	{
		if (col->IsPK())
		{
			sql->AppendCmdC(CSTR(" PRIMARY KEY"));
		}
	}

	if (col->GetDefVal())
	{
		sql->AppendCmdC(CSTR(" DEFAULT "));
		col->GetDefVal(sql);
		//sql->AppendStr(col->GetDefVal());
	}
}

void DB::ReadingDBTool::AppendColType(DB::DBUtil::ServerType svrType, DB::SQLBuilder *sql, DB::DBUtil::ColType colType, UOSInt colSize)
{
	if (svrType == DB::DBUtil::ServerType::MySQL)
	{
		if (colType == DB::DBUtil::CT_Bool)
		{
			sql->AppendCmdC(CSTR("TINYINT(1)"));
		}
		else if (colType == DB::DBUtil::CT_Byte)
		{
			sql->AppendCmdC(CSTR("TINYINT(3)"));
		}
		else if (colType == DB::DBUtil::CT_Char)
		{
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendInt32((Int32)colSize);
			sql->AppendCmdC(CSTR(")"));
		}
		else if (colType == DB::DBUtil::CT_DateTime)
		{
			sql->AppendCmdC(CSTR("DATETIME"));
		}
		else if (colType == DB::DBUtil::CT_Double)
		{
			sql->AppendCmdC(CSTR("DOUBLE"));
		}
		else if (colType == DB::DBUtil::CT_Float)
		{
			sql->AppendCmdC(CSTR("FLOAT"));
		}
		else if (colType == DB::DBUtil::CT_Int16)
		{
			sql->AppendCmdC(CSTR("SMALLINT"));
		}
		else if (colType == DB::DBUtil::CT_Int32)
		{
			sql->AppendCmdC(CSTR("INTEGER"));
		}
		else if (colType == DB::DBUtil::CT_Int64)
		{
			sql->AppendCmdC(CSTR("BIGINT"));
		}
		else if (colType == DB::DBUtil::CT_UInt32)
		{
			sql->AppendCmdC(CSTR("INTEGER UNSIGNED"));
		}
		else if (colType == DB::DBUtil::CT_VarChar)
		{
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("LONGTEXT"));
			}
			else if (colSize == 65535)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendInt32((Int32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
		}
		else
		{
			////////////////////////////////////////
		}
	}
	else if (svrType == DB::DBUtil::ServerType::MSSQL)
	{
		if (colType == DB::DBUtil::CT_Bool)
		{
			sql->AppendCmdC(CSTR("BOOL"));
		}
		else if (colType == DB::DBUtil::CT_Byte)
		{
			sql->AppendCmdC(CSTR("BYTE"));
		}
		else if (colType == DB::DBUtil::CT_Char)
		{
			sql->AppendCmdC(CSTR("NCHAR("));
			sql->AppendInt32((Int32)colSize);
			sql->AppendCmdC(CSTR(")"));
		}
		else if (colType == DB::DBUtil::CT_DateTime)
		{
			sql->AppendCmdC(CSTR("DATETIME"));
		}
		else if (colType == DB::DBUtil::CT_Double)
		{
			sql->AppendCmdC(CSTR("BINARY_DOUBLE"));
		}
		else if (colType == DB::DBUtil::CT_Float)
		{
			sql->AppendCmdC(CSTR("BINARY_FLOAT"));
		}
		else if (colType == DB::DBUtil::CT_Int16)
		{
			sql->AppendCmdC(CSTR("SMALLINT"));
		}
		else if (colType == DB::DBUtil::CT_Int32)
		{
			sql->AppendCmdC(CSTR("INT"));
		}
		else if (colType == DB::DBUtil::CT_Int64)
		{
			sql->AppendCmdC(CSTR("BIGINT"));
		}
		else if (colType == DB::DBUtil::CT_UInt32)
		{
			sql->AppendCmdC(CSTR("INT"));
		}
		else if (colType == DB::DBUtil::CT_VarChar)
		{
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("NVARCHAR(MAX)"));
			}
			else if (colSize > 4000)
			{
				sql->AppendCmdC(CSTR("NVARCHAR(MAX)"));
			}
			else
			{
				sql->AppendCmdC(CSTR("NVARCHAR("));
				sql->AppendInt32((Int32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
		}
		else
		{
			////////////////////////////////////////
		}
	}
	else if (svrType == DB::DBUtil::ServerType::Access)
	{
		if (colType == DB::DBUtil::CT_Bool)
		{
			sql->AppendCmdC(CSTR("BOOL"));
		}
		else if (colType == DB::DBUtil::CT_Byte)
		{
			sql->AppendCmdC(CSTR("BYTE"));
		}
		else if (colType == DB::DBUtil::CT_Char)
		{
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendInt32((Int32)colSize);
			sql->AppendCmdC(CSTR(")"));
		}
		else if (colType == DB::DBUtil::CT_DateTime)
		{
			sql->AppendCmdC(CSTR("DATETIME"));
		}
		else if (colType == DB::DBUtil::CT_Double)
		{
			sql->AppendCmdC(CSTR("DOUBLE"));
		}
		else if (colType == DB::DBUtil::CT_Float)
		{
			sql->AppendCmdC(CSTR("SINGLE"));
		}
		else if (colType == DB::DBUtil::CT_Int16)
		{
			sql->AppendCmdC(CSTR("SMALLINT"));
		}
		else if (colType == DB::DBUtil::CT_Int32)
		{
			sql->AppendCmdC(CSTR("INT"));
		}
		else if (colType == DB::DBUtil::CT_Int64)
		{
			sql->AppendCmdC(CSTR("BIGINT"));
		}
		else if (colType == DB::DBUtil::CT_UInt32)
		{
			sql->AppendCmdC(CSTR("INT"));
		}
		else if (colType == DB::DBUtil::CT_VarChar)
		{
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else if (colSize > 255)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendInt32((Int32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
		}
		else
		{
			////////////////////////////////////////
		}
	}
	if (svrType == DB::DBUtil::ServerType::SQLite)
	{
		if (colType == DB::DBUtil::CT_Bool)
		{
			sql->AppendCmdC(CSTR("BOOLEAN"));
		}
		else if (colType == DB::DBUtil::CT_Byte)
		{
			sql->AppendCmdC(CSTR("TINYINT"));
		}
		else if (colType == DB::DBUtil::CT_Char)
		{
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendInt32((Int32)colSize);
			sql->AppendCmdC(CSTR(")"));
		}
		else if (colType == DB::DBUtil::CT_DateTime)
		{
			sql->AppendCmdC(CSTR("DATETIME"));
		}
		else if (colType == DB::DBUtil::CT_Double)
		{
			sql->AppendCmdC(CSTR("DOUBLE"));
		}
		else if (colType == DB::DBUtil::CT_Float)
		{
			sql->AppendCmdC(CSTR("FLOAT"));
		}
		else if (colType == DB::DBUtil::CT_Int16)
		{
			sql->AppendCmdC(CSTR("SMALLINT"));
		}
		else if (colType == DB::DBUtil::CT_Int32)
		{
			sql->AppendCmdC(CSTR("INTEGER"));
		}
		else if (colType == DB::DBUtil::CT_Int64)
		{
			sql->AppendCmdC(CSTR("BIGINT"));
		}
		else if (colType == DB::DBUtil::CT_UInt32)
		{
			sql->AppendCmdC(CSTR("INTEGER"));
		}
		else if (colType == DB::DBUtil::CT_VarChar)
		{
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else if (colSize >= 65535)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendInt32((Int32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
		}
		else
		{
			////////////////////////////////////////
		}
	}
	else
	{
		//////////////////////
	}
}
