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
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Writer.h"


DB::DBTool::DBTool(DB::DBConn *conn, Bool needRelease, IO::LogTool *log, const UTF8Char *logPrefix) : DB::ReadingDBTool(conn, needRelease, log, logPrefix)
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
			ptr = Text::StrConcat(buff, (const UTF8Char*)"SQL R t1 = ");
			ptr = Text::StrInt32(ptr, (Int32)t2.DiffMS(&t1));
			ptr = Text::StrConcat(ptr, (const UTF8Char*)", t2 = ");
			ptr = Text::StrInt32(ptr, (Int32)t3.DiffMS(&t2));
			AddLogMsg(buff, IO::ILogHandler::LOG_LEVEL_COMMAND);
		}
		nqFail = 0;
		openFail = 0;
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
	if (this->svrType == DB::DBUtil::ServerType::MySQL || this->svrType == DB::DBUtil::ServerType::MSSQL || this->svrType == DB::DBUtil::ServerType::Access || this->svrType == DB::DBUtil::ServerType::MDBTools)
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
	if (this->svrType == DB::DBUtil::ServerType::MySQL || this->svrType == DB::DBUtil::ServerType::MSSQL || this->svrType == DB::DBUtil::ServerType::Access || this->svrType == DB::DBUtil::ServerType::MDBTools)
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
	UOSInt i;
	UOSInt j;
	UOSInt k;
	DB::ColDef *col;
	sql->AppendCmd((const UTF8Char*)"create table ");
	sql->AppendCol(tableName);
	sql->AppendCmd((const UTF8Char*)" (");
	if (this->svrType == DB::DBUtil::ServerType::Access || this->svrType == DB::DBUtil::ServerType::MDBTools)
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
	else if (this->svrType == DB::DBUtil::ServerType::SQLite)
	{
		Bool hasAutoInc = false;
		j = tabDef->GetColCnt();
		i = 0;
		while (i < j)
		{
			col = tabDef->GetCol(i++);
			this->AppendColDef(this->svrType, sql, col);
			if (col->IsAutoInc())
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
				if (col->IsPK())
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
			if (col->IsPK())
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
	if (this->svrType == DB::DBUtil::ServerType::MySQL)
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

DB::DBTool::PageStatus DB::DBTool::GenSelectCmdPage(DB::SQLBuilder *sql, DB::TableDef *tabDef, DB::PageRequest *page)
{
	DB::DBTool::PageStatus status;
	if (page)
	{
		status = PS_NO_PAGE;
	}
	else
	{
		status = PS_SUCC;
	}
	DB::ColDef *col;
	UOSInt i = 0;
	UOSInt j = tabDef->GetColCnt();
	sql->AppendCmd((const UTF8Char*)"select ");
	if (page && (this->svrType == DB::DBUtil::ServerType::Access))
	{
		sql->AppendCmd((const UTF8Char*)"TOP ");
		sql->AppendInt32((Int32)((page->GetPageNum() + 1) * page->GetPageSize()));
		status = PS_NO_OFFSET;
	}
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
	if (page)
	{
		Bool hasOrder = false;
		i = 1;
		j = page->GetSortingCount();
		if (j > 0)
		{
			hasOrder = true;
			sql->AppendCmd((const UTF8Char*)" order by ");
			sql->AppendCol(page->GetSortColumn(0));
			if (page->IsSortDesc(0))
			{
				sql->AppendCmd((const UTF8Char*)" desc");
			}
			while (i < j)
			{
				sql->AppendCmd((const UTF8Char*)", ");
				sql->AppendCol(page->GetSortColumn(i));
				if (page->IsSortDesc(i))
				{
					sql->AppendCmd((const UTF8Char*)" desc");
				}
				i++;
			}
		}

		if (this->svrType == DB::DBUtil::ServerType::MySQL)
		{
			sql->AppendCmd((const UTF8Char*)" LIMIT ");
			sql->AppendInt32((Int32)(page->GetPageNum() * page->GetPageSize()));
			sql->AppendCmd((const UTF8Char*)", ");
			sql->AppendInt32((Int32)page->GetPageSize());
			status = PS_SUCC;
		}
		else if (this->svrType == DB::DBUtil::ServerType::MSSQL)
		{
			if (!hasOrder)
			{
				i = 0;
				j = tabDef->GetColCnt();
				while (i < j)
				{
					col = tabDef->GetCol(i);
					if (col->IsPK())
					{
						if (hasOrder)
						{
							sql->AppendCmd((const UTF8Char*)", ");
						}
						else
						{
							hasOrder = true;
							sql->AppendCmd((const UTF8Char*)" order by ");
						}
						sql->AppendCol(col->GetColName());
					}
					i++;
				}
			}
			if (hasOrder)
			{
				status = PS_SUCC;
				sql->AppendCmd((const UTF8Char*)" offset ");
				sql->AppendInt32((Int32)(page->GetPageNum() * page->GetPageSize()));
				sql->AppendCmd((const UTF8Char*)" row fetch next ");
				sql->AppendInt32((Int32)page->GetPageSize());
				sql->AppendCmd((const UTF8Char*)" row only");
			}
			else
			{
				status = PS_NO_PAGE;
			}
		}
	}
	return status;
}

Bool DB::DBTool::GenInsertCmd(DB::SQLBuilder *sql, const UTF8Char *tableName, DB::DBReader *r)
{
	UTF8Char tmpBuff[256];
	DB::DBUtil::ColType colType;
	Text::StringBuilderUTF8 *sb;
	Data::DateTime *dt;
	UOSInt i;
	UOSInt j;

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
			Math::Vector2D *vec;
			UInt8 *binBuff;
			UOSInt colSize;
			colType = r->GetColType(i, &colSize);
			switch (colType)
			{
			case DB::DBUtil::CT_Bool:
				sql->AppendBool(r->GetBool(i));
				break;
			case DB::DBUtil::CT_DateTime:
			case DB::DBUtil::CT_DateTime2:
				r->GetDate(i, dt);
				sql->AppendDate(dt);
				break;
			case DB::DBUtil::CT_Int64:
				sql->AppendInt64(r->GetInt64(i));
				break;
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_Int16:
				sql->AppendInt32(r->GetInt32(i));
				break;
			case DB::DBUtil::CT_UInt64:
				sql->AppendUInt64((UInt64)r->GetInt64(i));
				break;
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_UInt16:
			case DB::DBUtil::CT_Byte:
				sql->AppendUInt32((UInt32)r->GetInt32(i));
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
				sql->AppendDbl(r->GetDbl(i));
				break;
			case DB::DBUtil::CT_Vector:
				vec = r->GetVector(i);
				sql->AppendVector(vec);
				SDEL_CLASS(vec);
				break;
			case DB::DBUtil::CT_Binary:
				if (r->IsNull(i))
				{
					sql->AppendStrUTF8(0);
				}
				else
				{
					UOSInt sz = r->GetBinarySize(i);
					binBuff = MemAlloc(UInt8, sz);
					r->GetBinary(i, binBuff);
					sql->AppendBinary(binBuff, sz);
					MemFree(binBuff);
				}
				break;
			case DB::DBUtil::CT_VarChar:
			case DB::DBUtil::CT_Char:
			case DB::DBUtil::CT_NVarChar:
			case DB::DBUtil::CT_NChar:
			case DB::DBUtil::CT_UUID:
			case DB::DBUtil::CT_Unknown:
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
	UOSInt i;
	UOSInt j;

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
