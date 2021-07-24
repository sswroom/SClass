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

void DB::ReadingDBTool::AddLogMsg(const UTF8Char *msg, IO::ILogHandler::LogLevel logLev)
{
	if (log)
	{
		if (logPrefix)
		{
			Text::StringBuilderUTF8 str;
			str.Append(logPrefix);
			str.Append(msg);
			log->LogMessage(str.ToString(), logLev);
		}
		else
		{
			log->LogMessage(msg, logLev);
		}
	}
}

DB::ReadingDBTool::ReadingDBTool(DB::DBConn *db, Bool needRelease, IO::LogTool *log, const UTF8Char *logPrefix)
{
	this->db = db;
	this->needRelease = needRelease;
	this->log = log;
	this->lastReader = 0;
	this->readerCnt = 0;
	this->readerFail = 0;
	this->openFail = 0;
	NEW_CLASS(this->lastErrMsg, Text::StringBuilderUTF8());

	if (logPrefix)
	{
		this->logPrefix = Text::StrCopyNew(logPrefix);
	}
	else
	{
		this->logPrefix = 0;
	}
	this->isWorking = false;
	this->workId = 0;
	this->trig = 0;
	this->dataCnt = 0;
	this->svrType = db->GetSvrType();
	if (this->svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		this->AddLogMsg((const UTF8Char*)"Server type is Access", IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		this->AddLogMsg((const UTF8Char*)"Server type is MSSQL", IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		this->AddLogMsg((const UTF8Char*)"Server type is MySQL", IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_ORACLE)
	{
		this->AddLogMsg((const UTF8Char*)"Server type is Oracle", IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_TEXT)
	{
		this->AddLogMsg((const UTF8Char*)"Server type is Text", IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		this->AddLogMsg((const UTF8Char*)"Server type is SQLite", IO::ILogHandler::LOG_LEVEL_COMMAND);
	}
	else
	{
		this->AddLogMsg((const UTF8Char*)"Server type is Unknown", IO::ILogHandler::LOG_LEVEL_ERROR);
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
	if (this->logPrefix)
	{
		Text::StrDelNew(this->logPrefix);
		this->logPrefix = 0;
	}
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

DB::DBReader *DB::ReadingDBTool::ExecuteReader(const UTF8Char *sqlCmd)
{
	{
		Text::StringBuilderUTF8 logMsg;
		logMsg.Append((const UTF8Char*)"ExecuteReader: ");
		logMsg.Append(sqlCmd);
		AddLogMsg(logMsg.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);
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
			ptr = Text::StrConcat(buff, (const UTF8Char*)"SQL R t1 = ");
			ptr = Text::StrInt32(ptr, (Int32)t2.DiffMS(&t1));
			ptr = Text::StrConcat(ptr, (const UTF8Char*)", t2 = ");
			ptr = Text::StrInt32(ptr, (Int32)t3.DiffMS(&t2));
			AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_COMMAND);
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
			logMsg.Append((const UTF8Char*)"Cannot execute the sql command: ");
			logMsg.Append(sqlCmd);
			AddLogMsg(logMsg.ToString(), IO::ILogHandler::LOG_LEVEL_ERROR);
		}

		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.Append((const UTF8Char*)"Exception detail: ");
			this->lastErrMsg->ClearStr();
			this->db->GetErrorMsg(this->lastErrMsg);
			logMsg.AppendSB(this->lastErrMsg);
			AddLogMsg(logMsg.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
		}

		readerFail += 1;
		if (readerFail >= 2)
		{
			if (lastReader)
			{
				AddLogMsg((const UTF8Char*)"Automatically closed last reader", IO::ILogHandler::LOG_LEVEL_ACTION);

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

Bool DB::ReadingDBTool::IsDataError(UTF8Char *errCode)
{
	if (errCode == 0)
		return false;
	if (Text::StrCompare(errCode, (const UTF8Char*)"23000") == 0)
		return true;
	if (Text::StrCompare(errCode, (const UTF8Char*)"42000") == 0)
		return true;
	if (Text::StrCompare(errCode, (const UTF8Char*)"HY000") == 0)
		return true;
	return false;
}

void DB::ReadingDBTool::GetLastErrorMsg(Text::StringBuilderUTF *sb)
{
	sb->Append(this->lastErrMsg->ToString());
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

UTF8Char *DB::ReadingDBTool::DBTrim(UTF8Char *sqlstr, const UTF8Char *val)
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

DB::DBReader *DB::ReadingDBTool::GetTableData(const UTF8Char *tableName, UOSInt maxCnt, void *ordering, void *condition)
{
	{
		Text::StringBuilderUTF8 logMsg;
		logMsg.Append((const UTF8Char*)"GetTableData: ");
		logMsg.Append(tableName);
		AddLogMsg(logMsg.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
	if (this->db == 0)
	{
		readerCnt += 1;
		dataCnt += 1;
		return lastReader;
	}

	Data::DateTime t1;
	Data::DateTime t2;
	DB::DBReader *r = this->db->GetTableData(tableName, maxCnt, ordering, condition);
	if (r)
	{
		Data::DateTime t3;
		dataCnt += 1;
		if (t3.DiffMS(&t2) >= 1000)
		{
			UTF8Char buff[256];
			UTF8Char *ptr;
			ptr = Text::StrConcat(buff, (const UTF8Char*)"SQL R t1 = ");
			ptr = Text::StrInt32(ptr, (Int32)t2.DiffMS(&t1));
			ptr = Text::StrConcat(ptr, (const UTF8Char*)", t2 = ");
			ptr = Text::StrInt32(ptr, (Int32)t3.DiffMS(&t2));
			AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_COMMAND);
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
			logMsg.Append((const UTF8Char*)"Cannot get table data: ");
			logMsg.Append(tableName);
			AddLogMsg(logMsg.ToString(), IO::ILogHandler::LOG_LEVEL_ERROR);
		}

		{
			Text::StringBuilderUTF8 logMsg;
			logMsg.Append((const UTF8Char*)"Exception detail: ");
			this->lastErrMsg->ClearStr();
			this->db->GetErrorMsg(this->lastErrMsg);
			logMsg.AppendSB(this->lastErrMsg);
			AddLogMsg(logMsg.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
		}

		readerFail += 1;
		if (readerFail >= 2)
		{
			if (lastReader)
			{
				AddLogMsg((const UTF8Char*)"Automatically closed last reader", IO::ILogHandler::LOG_LEVEL_ACTION);

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

UOSInt DB::ReadingDBTool::GetTableNames(Data::ArrayList<const UTF8Char*> *arr)
{
	if (this->svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		UOSInt ret = 0;
		DB::DBReader *r = this->ExecuteReader((const UTF8Char*)"select TABLE_SCHEMA, TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_TYPE='BASE TABLE'");
		if (r)
		{
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(0, &sb))
				{
					sb.AppendChar('.', 1);
				}
				if (r->GetStr(1, &sb))
				{
					arr->Add(Text::StrCopyNew(sb.ToString()));
					ret++;
				}
			}
			this->CloseReader(r);
		}
		return ret;
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		DB::DBReader *r = this->ExecuteReader((const UTF8Char*)"show tables");
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
	else if (this->svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		DB::DBReader *r = this->ExecuteReader((const UTF8Char*)"select name from sqlite_master where type = 'table'");
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
	else if (this->svrType == DB::DBUtil::SVR_TYPE_ACCESS || this->svrType == DB::DBUtil::SVR_TYPE_MDBTOOLS)
	{
		DB::DBReader *r = this->ExecuteReader((const UTF8Char*)"select name, type from MSysObjects where type = 1");
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
	else
	{
		return 0;
	}
}

void DB::ReadingDBTool::ReleaseTableNames(Data::ArrayList<const UTF8Char*> *arr)
{
	UOSInt i = arr->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(arr->RemoveAt(i));
	}
}

DB::TableDef *DB::ReadingDBTool::GetTableDef(const UTF8Char *tableName)
{
	UTF8Char buff[256];
	UTF8Char *ptr;
	UTF8Char *u8ptr;
	if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		OSInt i = 4;
		DB::DBReader *r = 0;
		ptr = Text::StrConcat(buff, (const UTF8Char*)"show table status where Name = ");
		ptr = this->DBStrUTF8(ptr, tableName);

		while (i-- > 0 && r == 0)
		{
			r = this->ExecuteReader(buff);
		}
		if (r == 0)
			return 0;

		DB::TableDef *tab;

		if (r->ReadNext())
		{
			r->GetStr(0, buff, sizeof(buff));
			NEW_CLASS(tab, DB::TableDef(buff));
			r->GetStr(1, buff, sizeof(buff));
			tab->SetEngine(buff);
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
			tab->SetSvrType(DB::DBUtil::SVR_TYPE_MYSQL);
			tab->SetCharset(0);
			this->CloseReader(r);
		}
		else
		{
			this->CloseReader(r);
			return 0;
		}
		DB::ColDef *col;
		ptr = Text::StrConcat(buff, (const UTF8Char*)"desc ");
		ptr = this->DBColUTF8(ptr, tableName);
		r = this->ExecuteReader(buff);
		if (r)
		{
			while (r->ReadNext())
			{
				r->GetStr(0, buff, sizeof(buff));
				NEW_CLASS(col, DB::ColDef(buff));
				r->GetStr(2, buff, sizeof(buff));
				col->SetNotNull(Text::StrEqualsICase(buff, (const UTF8Char*)"NO"));
				r->GetStr(3, buff, sizeof(buff));
				col->SetPK(Text::StrEqualsICase(buff, (const UTF8Char*)"PRI"));
				if (r->GetStr(4, buff, sizeof(buff)))
				{
					col->SetDefVal(buff);
				}
				else
				{
					col->SetDefVal(0);
				}
				if (r->GetStr(5, buff, sizeof(buff)))
				{
					if (Text::StrEquals(buff, (const UTF8Char*)"auto_increment"))
					{
						col->SetAutoInc(true);
						col->SetAttr(0);
					}
					else
					{
						col->SetAttr(buff);
					}
				}
				else
				{
					col->SetAttr(0);
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
	else if (this->svrType == DB::DBUtil::SVR_TYPE_ORACLE)
	{
		return 0;
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		Int32 i = 4;
		DB::DBReader *r = 0;
		OSInt ind;
		ptr = Text::StrConcat(buff, (const UTF8Char*)"exec sp_columns ");
		ind = Text::StrIndexOf(tableName, '.');
		if (ind > 0)
		{
			ptr = this->DBStrUTF8(ptr, &tableName[ind + 1]);
			ptr = Text::StrConcat(ptr, (const UTF8Char*)", ");
			const UTF8Char *tmpPtr = Text::StrCopyNewC(tableName, (UOSInt)ind);
			ptr = this->DBStrUTF8(ptr, tmpPtr);
			Text::StrDelNew(tmpPtr);
		}
		else
		{
			ptr = this->DBStrUTF8(ptr, tableName);
		}
		while (i-- > 0 && r == 0)
		{
			r = this->ExecuteReader(buff);
		}
		if (r == 0)
			return 0;

		DB::TableDef *tab;
		NEW_CLASS(tab, DB::TableDef(tableName));
		tab->SetEngine(0);
		tab->SetComments(0);
		tab->SetAttr(0);
		tab->SetCharset(0);
		tab->SetSvrType(DB::DBUtil::SVR_TYPE_MSSQL);

		DB::ColDef *col;
		while (r->ReadNext())
		{
			r->GetStr(3, buff, sizeof(buff));
			NEW_CLASS(col, DB::ColDef(buff));
			col->SetNotNull(!r->GetBool(10));
			col->SetPK(false);
			if ((u8ptr = r->GetStr(12, buff, sizeof(buff))) != 0)
			{
				if (*buff == '{')
				{
					u8ptr[-1] = 0;
					col->SetDefVal(&buff[1]);
				}
				else
				{
					col->SetDefVal(buff);
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
		sb.Append((const UTF8Char*)"SELECT c.name AS column_name, i.name AS index_name, c.is_identity FROM sys.indexes i");
		sb.Append((const UTF8Char*)" inner join sys.index_columns ic  ON i.object_id = ic.object_id AND i.index_id = ic.index_id");
		sb.Append((const UTF8Char*)" inner join sys.columns c ON ic.object_id = c.object_id AND c.column_id = ic.column_id");
		sb.Append((const UTF8Char*)" WHERE i.is_primary_key = 1");
		sb.Append((const UTF8Char*)" and i.object_ID = OBJECT_ID('");
		sb.Append(tableName);
		sb.Append((const UTF8Char*)"')");
		r = 0;
		i = 4;
		while (i-- > 0 && r == 0)
		{
			r = this->ExecuteReader(sb.ToString());
		}
		if (r == 0)
			return tab;

		UOSInt j;
		UOSInt k;
		while (r->ReadNext())
		{
			r->GetStr(0, buff, sizeof(buff));
			j = 0;
			k = tab->GetColCnt();
			while (j < k)
			{
				col = tab->GetCol(j);
				if (Text::StrEquals(col->GetColName(), buff))
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
	else if (this->svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		return 0;
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		DB::SQLBuilder sql(this->svrType, this->GetTzQhr());
		sql.AppendCmd((const UTF8Char*)"select sql from sqlite_master where type='table' and name=");
		sql.AppendStrUTF8(tableName);
		DB::DBReader *r = this->db->ExecuteReader(sql.ToString());
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
	if (this->svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		DB::DBReader *r = this->ExecuteReader((const UTF8Char*)"select name from master.dbo.sysdatabases");
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
	else if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		DB::DBReader *r = this->ExecuteReader((const UTF8Char*)"show databases");
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
	else if (this->svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		Text::StringBuilderUTF8 sb;
		const UTF8Char *name = this->db->GetSourceNameObj();
		UOSInt i = Text::StrLastIndexOf(name, (UTF8Char)IO::Path::PATH_SEPERATOR);
		sb.Append(&name[i + 1]);
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
	if (this->svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		this->DBColUTF8(Text::StrConcat(sbuff, (const UTF8Char*)"use "), databaseName);
		DB::DBReader *r = this->ExecuteReader(sbuff);
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
	else if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		this->DBColUTF8(Text::StrConcat(sbuff, (const UTF8Char*)"use "), databaseName);
		DB::DBReader *r = this->ExecuteReader(sbuff);
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
	if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		return SplitMySQL(outStrs, maxCnt, oriStr);
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_MSSQL)
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
	sql->AppendCol(col->GetColName());
	sql->AppendCmd((const UTF8Char*)" ");
	AppendColType(svrType, sql, col->GetColType(), col->GetColSize());
	if (col->IsNotNull())
	{
		sql->AppendCmd((const UTF8Char*)" NOT NULL");
	}
	if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		if (col->IsAutoInc())
		{
			sql->AppendCmd((const UTF8Char*)" IDENTITY(1,1)");
		}
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		if (col->IsAutoInc())
		{
			sql->AppendCmd((const UTF8Char*)" AUTO_INCREMENT");
		}
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		if (col->IsAutoInc() && col->IsPK())
		{
			sql->AppendCmd((const UTF8Char*)" PRIMARY KEY");
		}
		if (col->IsAutoInc() && (col->GetColType() == DB::DBUtil::CT_Int32 || col->GetColType() == DB::DBUtil::CT_UInt32))
		{
			sql->AppendCmd((const UTF8Char*)" AUTOINCREMENT");
		}
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		if (col->IsPK())
		{
			sql->AppendCmd((const UTF8Char*)" PRIMARY KEY");
		}
	}

	if (col->GetDefVal())
	{
		sql->AppendCmd((const UTF8Char*)" DEFAULT ");
		col->GetDefVal(sql);
		//sql->AppendStr(col->GetDefVal());
	}
}

void DB::ReadingDBTool::AppendColType(DB::DBUtil::ServerType svrType, DB::SQLBuilder *sql, DB::DBUtil::ColType colType, UOSInt colSize)
{
	if (svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		if (colType == DB::DBUtil::CT_Bool)
		{
			sql->AppendCmd((const UTF8Char*)"TINYINT(1)");
		}
		else if (colType == DB::DBUtil::CT_Byte)
		{
			sql->AppendCmd((const UTF8Char*)"TINYINT(3)");
		}
		else if (colType == DB::DBUtil::CT_Char)
		{
			sql->AppendCmd((const UTF8Char*)"CHAR(");
			sql->AppendInt32((Int32)colSize);
			sql->AppendCmd((const UTF8Char*)")");
		}
		else if (colType == DB::DBUtil::CT_DateTime)
		{
			sql->AppendCmd((const UTF8Char*)"DATETIME");
		}
		else if (colType == DB::DBUtil::CT_Double)
		{
			sql->AppendCmd((const UTF8Char*)"DOUBLE");
		}
		else if (colType == DB::DBUtil::CT_Float)
		{
			sql->AppendCmd((const UTF8Char*)"FLOAT");
		}
		else if (colType == DB::DBUtil::CT_Int16)
		{
			sql->AppendCmd((const UTF8Char*)"SMALLINT");
		}
		else if (colType == DB::DBUtil::CT_Int32)
		{
			sql->AppendCmd((const UTF8Char*)"INTEGER");
		}
		else if (colType == DB::DBUtil::CT_Int64)
		{
			sql->AppendCmd((const UTF8Char*)"BIGINT");
		}
		else if (colType == DB::DBUtil::CT_UInt32)
		{
			sql->AppendCmd((const UTF8Char*)"INTEGER UNSIGNED");
		}
		else if (colType == DB::DBUtil::CT_VarChar)
		{
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmd((const UTF8Char*)"LONGTEXT");
			}
			else if (colSize == 65535)
			{
				sql->AppendCmd((const UTF8Char*)"TEXT");
			}
			else
			{
				sql->AppendCmd((const UTF8Char*)"VARCHAR(");
				sql->AppendInt32((Int32)colSize);
				sql->AppendCmd((const UTF8Char*)")");
			}
		}
		else
		{
			////////////////////////////////////////
		}
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		if (colType == DB::DBUtil::CT_Bool)
		{
			sql->AppendCmd((const UTF8Char*)"BOOL");
		}
		else if (colType == DB::DBUtil::CT_Byte)
		{
			sql->AppendCmd((const UTF8Char*)"BYTE");
		}
		else if (colType == DB::DBUtil::CT_Char)
		{
			sql->AppendCmd((const UTF8Char*)"NCHAR(");
			sql->AppendInt32((Int32)colSize);
			sql->AppendCmd((const UTF8Char*)")");
		}
		else if (colType == DB::DBUtil::CT_DateTime)
		{
			sql->AppendCmd((const UTF8Char*)"DATETIME");
		}
		else if (colType == DB::DBUtil::CT_Double)
		{
			sql->AppendCmd((const UTF8Char*)"BINARY_DOUBLE");
		}
		else if (colType == DB::DBUtil::CT_Float)
		{
			sql->AppendCmd((const UTF8Char*)"BINARY_FLOAT");
		}
		else if (colType == DB::DBUtil::CT_Int16)
		{
			sql->AppendCmd((const UTF8Char*)"SMALLINT");
		}
		else if (colType == DB::DBUtil::CT_Int32)
		{
			sql->AppendCmd((const UTF8Char*)"INT");
		}
		else if (colType == DB::DBUtil::CT_Int64)
		{
			sql->AppendCmd((const UTF8Char*)"BIGINT");
		}
		else if (colType == DB::DBUtil::CT_UInt32)
		{
			sql->AppendCmd((const UTF8Char*)"INT");
		}
		else if (colType == DB::DBUtil::CT_VarChar)
		{
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmd((const UTF8Char*)"NVARCHAR(MAX)");
			}
			else if (colSize > 4000)
			{
				sql->AppendCmd((const UTF8Char*)"NVARCHAR(MAX)");
			}
			else
			{
				sql->AppendCmd((const UTF8Char*)"NVARCHAR(");
				sql->AppendInt32((Int32)colSize);
				sql->AppendCmd((const UTF8Char*)")");
			}
		}
		else
		{
			////////////////////////////////////////
		}
	}
	else if (svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		if (colType == DB::DBUtil::CT_Bool)
		{
			sql->AppendCmd((const UTF8Char*)"BOOL");
		}
		else if (colType == DB::DBUtil::CT_Byte)
		{
			sql->AppendCmd((const UTF8Char*)"BYTE");
		}
		else if (colType == DB::DBUtil::CT_Char)
		{
			sql->AppendCmd((const UTF8Char*)"CHAR(");
			sql->AppendInt32((Int32)colSize);
			sql->AppendCmd((const UTF8Char*)")");
		}
		else if (colType == DB::DBUtil::CT_DateTime)
		{
			sql->AppendCmd((const UTF8Char*)"DATETIME");
		}
		else if (colType == DB::DBUtil::CT_Double)
		{
			sql->AppendCmd((const UTF8Char*)"DOUBLE");
		}
		else if (colType == DB::DBUtil::CT_Float)
		{
			sql->AppendCmd((const UTF8Char*)"SINGLE");
		}
		else if (colType == DB::DBUtil::CT_Int16)
		{
			sql->AppendCmd((const UTF8Char*)"SMALLINT");
		}
		else if (colType == DB::DBUtil::CT_Int32)
		{
			sql->AppendCmd((const UTF8Char*)"INT");
		}
		else if (colType == DB::DBUtil::CT_Int64)
		{
			sql->AppendCmd((const UTF8Char*)"BIGINT");
		}
		else if (colType == DB::DBUtil::CT_UInt32)
		{
			sql->AppendCmd((const UTF8Char*)"INT");
		}
		else if (colType == DB::DBUtil::CT_VarChar)
		{
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmd((const UTF8Char*)"TEXT");
			}
			else if (colSize > 255)
			{
				sql->AppendCmd((const UTF8Char*)"TEXT");
			}
			else
			{
				sql->AppendCmd((const UTF8Char*)"VARCHAR(");
				sql->AppendInt32((Int32)colSize);
				sql->AppendCmd((const UTF8Char*)")");
			}
		}
		else
		{
			////////////////////////////////////////
		}
	}
	if (svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		if (colType == DB::DBUtil::CT_Bool)
		{
			sql->AppendCmd((const UTF8Char*)"BOOLEAN");
		}
		else if (colType == DB::DBUtil::CT_Byte)
		{
			sql->AppendCmd((const UTF8Char*)"TINYINT");
		}
		else if (colType == DB::DBUtil::CT_Char)
		{
			sql->AppendCmd((const UTF8Char*)"CHAR(");
			sql->AppendInt32((Int32)colSize);
			sql->AppendCmd((const UTF8Char*)")");
		}
		else if (colType == DB::DBUtil::CT_DateTime)
		{
			sql->AppendCmd((const UTF8Char*)"DATETIME");
		}
		else if (colType == DB::DBUtil::CT_Double)
		{
			sql->AppendCmd((const UTF8Char*)"DOUBLE");
		}
		else if (colType == DB::DBUtil::CT_Float)
		{
			sql->AppendCmd((const UTF8Char*)"FLOAT");
		}
		else if (colType == DB::DBUtil::CT_Int16)
		{
			sql->AppendCmd((const UTF8Char*)"SMALLINT");
		}
		else if (colType == DB::DBUtil::CT_Int32)
		{
			sql->AppendCmd((const UTF8Char*)"INTEGER");
		}
		else if (colType == DB::DBUtil::CT_Int64)
		{
			sql->AppendCmd((const UTF8Char*)"BIGINT");
		}
		else if (colType == DB::DBUtil::CT_UInt32)
		{
			sql->AppendCmd((const UTF8Char*)"INTEGER");
		}
		else if (colType == DB::DBUtil::CT_VarChar)
		{
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmd((const UTF8Char*)"TEXT");
			}
			else if (colSize >= 65535)
			{
				sql->AppendCmd((const UTF8Char*)"TEXT");
			}
			else
			{
				sql->AppendCmd((const UTF8Char*)"VARCHAR(");
				sql->AppendInt32((Int32)colSize);
				sql->AppendCmd((const UTF8Char*)")");
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
