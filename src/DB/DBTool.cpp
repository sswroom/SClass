#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListInt32.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "IO/Stream.h"
#include "Math/Point.h"
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"


DB::DBTool::DBTool(DB::DBConn *conn, Bool needRelease, IO::LogTool *log, Bool useMut, const UTF8Char *logPrefix) : DB::ReadingDBTool(conn, needRelease, log, useMut, logPrefix)
{
	this->nqFail = 0;
	this->tran = 0;
}

DB::DBTool::~DBTool()
{
}

OSInt DB::DBTool::ExecuteNonQuery(const UTF8Char *sqlCmd)
{
	{
		Text::StringBuilderUTF8 logMsg;
		logMsg.Append((const UTF8Char*)"ExecuteNonQuery: ");
		logMsg.Append(sqlCmd);
		AddLogMsg(logMsg.ToString(), IO::ILogHandler::LOG_LEVEL_RAW);
	}
	if (this->db == 0)
	{
		dataCnt += 1;
		return -1;
	}

	Bool mutWait = false;
	Data::DateTime t1;
	if (this->mut)
	{
		this->mut->Lock();
		mutWait = true;
	}
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
			ptr = Text::StrConcat(buff, (const UTF8Char*)"SQL R t1 = ");
			ptr = Text::StrInt32(ptr, (Int32)t2.DiffMS(&t1));
			ptr = Text::StrConcat(ptr, (const UTF8Char*)", t2 = ");
			ptr = Text::StrInt32(ptr, (Int32)t3.DiffMS(&t2));
			AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_COMMAND);
		}
		nqFail = 0;
		openFail = 0;
		if (mutWait)
		{
			this->mut->Unlock();
		}
		return i;
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
		Bool isData = this->db->IsLastDataError();
		if (!isData)
		{
			this->db->Reconnect();
		}
		if (mutWait)
		{
			this->mut->Unlock();
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
	if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL || this->svrType == DB::DBUtil::SVR_TYPE_MSSQL || this->svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		DB::DBReader *reader = this->ExecuteReader((const UTF8Char*)"select @@identity");
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
	if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL || this->svrType == DB::DBUtil::SVR_TYPE_MSSQL || this->svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		DB::DBReader *reader = this->ExecuteReader((const UTF8Char*)"select @@identity");
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

DB::DBConn *DB::DBTool::GetConn()
{
	return (DB::DBConn*)this->db;
}

Bool DB::DBTool::GenCreateTableCmd(DB::SQLBuilder *sql, const UTF8Char *tableName, DB::TableDef *tabDef)
{
	OSInt i;
	OSInt j;
	OSInt k;
	DB::ColDef *col;
	sql->AppendCmd((const UTF8Char*)"create table ");
	sql->AppendCol(tableName);
	sql->AppendCmd((const UTF8Char*)" (");
	if (this->svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		j = tabDef->GetColCnt();
		i = 0;
		while (i < j)
		{
			if (i > 0)
			{
				sql->AppendCmd((const UTF8Char*)", ");
			}
			col = tabDef->GetCol(i++);
			this->AppendColDef(this->svrType, sql, col);
		}
	}
	else if (this->svrType == DB::DBUtil::SVR_TYPE_SQLITE)
	{
		Bool hasAutoInc = false;
		j = tabDef->GetColCnt();
		i = 0;
		while (i < j)
		{
			col = tabDef->GetCol(i++);
			this->AppendColDef(this->svrType, sql, col);
			if (col->GetIsAutoInc())
			{
				hasAutoInc = true;
			}
			if (i < j)
			{
				sql->AppendCmd((const UTF8Char*)", ");
			}
		}
		if (!hasAutoInc)
		{
			i = 0;
			k = 0;
			sql->AppendCmd((const UTF8Char*)", ");
			sql->AppendCmd((const UTF8Char*)"PRIMARY KEY (");
			while (i < j)
			{
				col = tabDef->GetCol(i++);
				if (col->GetIsPK())
				{
					if (k > 0)
					{
						sql->AppendCmd((const UTF8Char*)", ");
					}
					sql->AppendCol(col->GetColName());
					k++;
				}
			}
			sql->AppendCmd((const UTF8Char*)")");
		}
	}
	else
	{
		j = tabDef->GetColCnt();
		i = 0;
		while (i < j)
		{
			col = tabDef->GetCol(i++);
			this->AppendColDef(this->svrType, sql, col);
			if (i < j)
			{
				sql->AppendCmd((const UTF8Char*)", ");
			}
		}
		i = 0;
		k = 0;
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendCmd((const UTF8Char*)"PRIMARY KEY (");
		while (i < j)
		{
			col = tabDef->GetCol(i++);
			if (col->GetIsPK())
			{
				if (k > 0)
				{
					sql->AppendCmd((const UTF8Char*)", ");
				}
				sql->AppendCol(col->GetColName());
				k++;
			}
		}
		sql->AppendCmd((const UTF8Char*)")");
	}
	sql->AppendCmd((const UTF8Char*)")");
	if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		if (tabDef->GetEngine())
		{
			sql->AppendCmd((const UTF8Char*)" ENGINE=");
			sql->AppendCmd(tabDef->GetEngine());
		}
		if (tabDef->GetCharset())
		{
			sql->AppendCmd((const UTF8Char*)" DEFAULT CHARSET=");
			sql->AppendCmd(tabDef->GetCharset());
		}
	}
	return true;
}

Bool DB::DBTool::GenDropTableCmd(DB::SQLBuilder *sql, const UTF8Char *tableName)
{
	sql->AppendCmd((const UTF8Char*)"drop table ");
	sql->AppendCol(tableName);
	return true;
}

Bool DB::DBTool::GenDeleteTableCmd(DB::SQLBuilder *sql, const UTF8Char *tableName)
{
	sql->AppendCmd((const UTF8Char*)"delete from ");
	sql->AppendCol(tableName);
	return true;
}

Bool DB::DBTool::GenSelectCmd(DB::SQLBuilder *sql, DB::TableDef *tabDef)
{
	DB::ColDef *col;
	UOSInt i = 0;
	UOSInt j = tabDef->GetColCnt();
	sql->AppendCmd((const UTF8Char*)"select ");
	while (i < j)
	{
		col = tabDef->GetCol(i);
		if (i > 0)
		{
			sql->AppendCmd((const UTF8Char*)", ");
		}
		sql->AppendCol(col->GetColName());
		i++;
	}
	sql->AppendCmd((const UTF8Char*)" from ");
	sql->AppendTableName(tabDef);
}

Bool DB::DBTool::GenInsertCmd(DB::SQLBuilder *sql, const UTF8Char *tableName, DB::DBReader *r)
{
	UTF8Char tmpBuff[256];
	DB::DBUtil::ColType colType;
	Text::StringBuilderUTF8 *sb;
	Data::DateTime *dt;
	OSInt i;
	OSInt j;

	NEW_CLASS(sb, Text::StringBuilderUTF8());
	NEW_CLASS(dt, Data::DateTime());

	sql->AppendCmd((const UTF8Char*)"insert into ");
	sql->AppendCol(tableName);
	sql->AppendCmd((const UTF8Char*)" (");
	j = r->ColCount();
	i = 1;
	r->GetName(0, tmpBuff);
	sql->AppendCol(tmpBuff);
	while (i < j)
	{
		r->GetName(i, tmpBuff);
		sql->AppendCmd((const UTF8Char*)", ");
		sql->AppendCol(tmpBuff);
		i++;
	}
	sql->AppendCmd((const UTF8Char*)") values (");
	i = 0;
	while (i < j)
	{
		if (i > 0)
		{
			sql->AppendCmd((const UTF8Char*)", ");
		}
		if (r->IsNull(i))
		{
			sql->AppendCmd((const UTF8Char*)"NULL");
		}
		else
		{
			UOSInt colSize;
			colType = r->GetColType(i, &colSize);
			switch (colType)
			{
			case DB::DBUtil::CT_Bool:
				sql->AppendBool(r->GetBool(i));
				break;
			case DB::DBUtil::CT_DateTime:
				r->GetDate(i, dt);
				sql->AppendDate(dt);
				break;
			case DB::DBUtil::CT_Int64:
				sql->AppendInt64(r->GetInt64(i));
				break;
			case DB::DBUtil::CT_Int32:
				sql->AppendInt32(r->GetInt32(i));
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
				sql->AppendDbl(r->GetDbl(i));
				break;
			default:
				sb->ClearStr();
				r->GetStr(i, sb);
				sql->AppendStrUTF8(sb->ToString());
				break;
			}
		}
		i++;
	}
	sql->AppendCmd((const UTF8Char*)")");
	DEL_CLASS(dt);
	DEL_CLASS(sb);
	return true;
}

UTF8Char *DB::DBTool::GenInsertCmd(UTF8Char *sqlstr, const UTF8Char *tableName, DB::DBReader *r)
{
	UTF8Char *currPtr;
	UTF8Char tmpBuff[256];
	Int32 i;
	OSInt j;

	currPtr = Text::StrConcat(sqlstr, (const UTF8Char*)"insert into ");
	currPtr = DBColUTF8(currPtr, tableName);
	r->GetName(0, tmpBuff);
	currPtr = DBColUTF8(currPtr, tmpBuff);
	j = r->ColCount();
	i = 1;
	while (i < j)
	{
		currPtr = Text::StrConcat(currPtr, (const UTF8Char*)", ");
		r->GetName(i++, tmpBuff);
		currPtr = DBColUTF8(currPtr, tmpBuff);
	}
	currPtr = Text::StrConcat(currPtr, (const UTF8Char*)") values (");
	if (r->IsNull(0))
	{
		currPtr = Text::StrConcat(currPtr, (const UTF8Char*)"NULL");
	}
	else
	{
		r->GetStr(0, tmpBuff, sizeof(tmpBuff));
		currPtr = DBStrUTF8(currPtr, tmpBuff);
	}
	i = 1;
	while (i < j)
	{
		currPtr = Text::StrConcat(currPtr, (const UTF8Char*)", ");
		if (r->IsNull(i))
		{
			currPtr = Text::StrConcat(currPtr, (const UTF8Char*)"NULL");
		}
		else
		{
			r->GetStr(i, tmpBuff, sizeof(tmpBuff));
			currPtr = DBStrUTF8(currPtr, tmpBuff);
		}
		i++;
	}

	currPtr = Text::StrConcat(currPtr, (const UTF8Char*)")");
	return currPtr;
}
