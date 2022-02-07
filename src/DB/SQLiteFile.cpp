#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/DBConn.h"
#include "DB/DBTool.h"
#include "DB/SQLiteFile.h"
#include "IO/FileStream.h"
#include "IO/LogTool.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "sqlite3.h"

void DB::SQLiteFile::Init()
{
	sqlite3 *db;
	Int32 ret;
	this->delOnClose = false;
	this->lastErrMsg = 0;
	db = 0;
	NEW_CLASS(this->tableNames, Data::ArrayListStrUTF8());
	sqlite3_initialize();
	ret = sqlite3_open_v2((const Char*)fileName->v, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE, 0);
	
	if (SQLITE_OK == ret)
	{
		Text::StringBuilderUTF8 *sb;
		NEW_CLASS(sb, Text::StringBuilderUTF8());
		this->db = db;
		DB::DBReader *r = ExecuteReaderC(UTF8STRC("SELECT name FROM sqlite_master WHERE type='table'"));
		if (r)
		{
			while (r->ReadNext())
			{
				sb->ClearStr();
				r->GetStr(0, sb);
				this->tableNames->Add(Text::StrCopyNew(sb->ToString()));
			}
			this->CloseReader(r);
		}
		DEL_CLASS(sb);
	}
	else
	{
		if (db)
		{
			sqlite3_close(db);
		}
		this->db = 0;
	}
}

DB::SQLiteFile::SQLiteFile(Text::String *fileName) : DB::DBConn(fileName)
{
	this->fileName = fileName->Clone();
	this->Init();
}

DB::SQLiteFile::SQLiteFile(Text::CString fileName) : DB::DBConn(fileName)
{
	this->fileName = Text::String::New(fileName);
	this->Init();
}

DB::SQLiteFile::~SQLiteFile()
{
	UOSInt i;
	if (this->db)
	{
		sqlite3_close((sqlite3*)this->db);
		this->db = 0;
	}
	sqlite3_shutdown();
	i = this->tableNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->tableNames->GetItem(i));
	}
	DEL_CLASS(this->tableNames);
	if (this->delOnClose)
	{
		IO::Path::DeleteFile(this->fileName->v);
	}
	this->fileName->Release();
	SDEL_STRING(this->lastErrMsg);
}

DB::DBUtil::ServerType DB::SQLiteFile::GetSvrType()
{
	return DB::DBUtil::ServerType::SQLite;
}

DB::DBConn::ConnType DB::SQLiteFile::GetConnType()
{
	return CT_SQLITE;
}

Int8 DB::SQLiteFile::GetTzQhr()
{
	return 0;
}

void DB::SQLiteFile::ForceTz(Int8 tzQhr)
{
	
}

void DB::SQLiteFile::GetConnName(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC(UTF8STRC("SQLite:"));
	sb->Append(this->fileName);
}

void DB::SQLiteFile::Close()
{
	if (this->db)
	{
		sqlite3_close((sqlite3*)this->db);
		this->db = 0;
	}
}

OSInt DB::SQLiteFile::ExecuteNonQuery(const UTF8Char *sql)
{
	if (this->db)
	{
		return ExecuteNonQueryC(sql, Text::StrCharCnt(sql));
	}
	else
	{
		this->lastDataError = DE_CONN_ERROR;
		return -2;
	}
}

OSInt DB::SQLiteFile::ExecuteNonQueryC(const UTF8Char *sql, UOSInt sqlLen)
{
	OSInt chg = -2;
	if (this->db)
	{
		sqlite3_stmt *stmt;
		const char *tmp;
		if (sqlite3_prepare_v2((sqlite3*)this->db, (const Char*)sql, (Int32)sqlLen + 1, &stmt, &tmp) == SQLITE_OK)
		{
			if (sqlite3_step(stmt) == SQLITE_DONE)
			{
				chg = sqlite3_changes((sqlite3*)this->db);
				this->lastDataError = DE_NO_ERROR;
			}
			else
			{
				this->lastDataError = DE_EXEC_SQL_ERROR;
				SDEL_STRING(this->lastErrMsg);
				this->lastErrMsg = Text::String::NewNotNull((const UTF8Char*)sqlite3_errmsg((sqlite3*)this->db));
				chg = 0;
			}
			sqlite3_finalize(stmt);
			return chg;
		}
		else
		{
			this->lastDataError = DE_INIT_SQL_ERROR;
			return -2;
		}
	}
	else
	{
		this->lastDataError = DE_CONN_ERROR;
		return -2;
	}
}

DB::DBReader *DB::SQLiteFile::ExecuteReader(const UTF8Char *sql)
{
	if (this->db)
	{
		return ExecuteReaderC(sql, Text::StrCharCnt(sql));
	}
	else
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
}

DB::DBReader *DB::SQLiteFile::ExecuteReaderC(const UTF8Char *sql, UOSInt sqlLen)
{
	if (this->db)
	{
		sqlite3_stmt *stmt;
		const char *tmp;
		if (sqlite3_prepare_v2((sqlite3*)this->db, (const Char*)sql, (Int32)sqlLen + 1, &stmt, &tmp) == SQLITE_OK)
		{
			this->lastDataError = DE_NO_ERROR;
			DB::SQLiteReader *r;
			NEW_CLASS(r, DB::SQLiteReader(this, stmt));
			return r;
		}
		else
		{
			this->lastDataError = DE_EXEC_SQL_ERROR;
			return 0;
		}
	}
	else
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
}

void DB::SQLiteFile::CloseReader(DBReader *r)
{
	DB::SQLiteReader *rdr = (DB::SQLiteReader*)r;
	DEL_CLASS(rdr);
}

void DB::SQLiteFile::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
	if (this->lastErrMsg)
	{
		str->Append(this->lastErrMsg);
	}
}

Bool DB::SQLiteFile::IsLastDataError()
{
	return this->lastDataError == DE_EXEC_SQL_ERROR;
}

void DB::SQLiteFile::Reconnect()
{
	sqlite3 *db;
	if (this->db)
	{
		sqlite3_close((sqlite3*)this->db);
		this->db = 0;
	}

	Int32 ret;
	ret = sqlite3_open((const Char*)this->fileName, &db);
	if (SQLITE_OK == ret)
	{
		this->db = db;
	}
}

void *DB::SQLiteFile::BeginTransaction()
{
	ExecuteNonQueryC(UTF8STRC("begin"));
	return (void*)-1;
}

void DB::SQLiteFile::Commit(void *tran)
{
	ExecuteNonQueryC(UTF8STRC("end"));
}

void DB::SQLiteFile::Rollback(void *tran)
{
	ExecuteNonQueryC(UTF8STRC("end"));
}

UOSInt DB::SQLiteFile::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	names->AddAll(this->tableNames);
	return this->tableNames->GetCount();
}

DB::DBReader *DB::SQLiteFile::GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("select * from "));
	sb.AppendSlow(tableName);
	if (maxCnt > 0)
	{
		sb.AppendC(UTF8STRC(" LIMIT "));
		sb.AppendOSInt((OSInt)maxCnt);
	}
	return ExecuteReaderC(sb.ToString(), sb.GetLength());
}

void DB::SQLiteFile::SetDeleteOnClose(Bool delOnClose)
{
	this->delOnClose = delOnClose;
}

Bool DB::SQLiteFile::IsError()
{
	return this->db == 0;
}

Text::String *DB::SQLiteFile::GetFileName()
{
	return this->fileName;
}

void DB::SQLiteReader::UpdateColTypes()
{
	UOSInt i;
	i = this->colCnt;
	while (i-- > 0)
	{
		if (this->colTypes[i] == DB::DBUtil::CT_Unknown)
		{
			int colType = sqlite3_column_type((sqlite3_stmt*)this->hStmt, (int)i);
			if (colType == SQLITE_INTEGER)
			{
				this->colTypes[i] = DB::DBUtil::CT_Int32;
			}
			else if (colType == SQLITE_FLOAT)
			{
				this->colTypes[i] = DB::DBUtil::CT_Double;
			}
			else if (colType == SQLITE_TEXT)
			{
				this->colTypes[i] = DB::DBUtil::CT_VarChar;
			}
			else if (colType == SQLITE_BLOB)
			{
				this->colTypes[i] = DB::DBUtil::CT_Binary;
			}
			else if (colType == SQLITE_NULL)
			{
				this->colTypes[i] = DB::DBUtil::CT_Unknown;
			}
			else
			{
				this->colTypes[i] = DB::DBUtil::CT_Unknown;
			}
		}
	}
}

DB::DBTool *DB::SQLiteFile::CreateDBTool(Text::String *fileName, IO::LogTool *log, Text::CString logPrefix)
{
	DB::SQLiteFile *conn;
	NEW_CLASS(conn, DB::SQLiteFile(fileName));
	if (conn->IsError())
	{
		DEL_CLASS(conn);
		return 0;
	}
	DB::DBTool *db;
	NEW_CLASS(db, DBTool(conn, true, log, logPrefix));
	return db;
}

DB::DBTool *DB::SQLiteFile::CreateDBTool(Text::CString fileName, IO::LogTool *log, Text::CString logPrefix)
{
	DB::SQLiteFile *conn;
	NEW_CLASS(conn, DB::SQLiteFile(fileName));
	if (conn->IsError())
	{
		DEL_CLASS(conn);
		return 0;
	}
	DB::DBTool *db;
	NEW_CLASS(db, DBTool(conn, true, log, logPrefix));
	return db;
}

DB::SQLiteReader::SQLiteReader(DB::SQLiteFile *conn, void *hStmt)
{
	UOSInt i;
	this->conn = conn;
	this->hStmt = hStmt;
	this->isFirst = true;
	this->firstResult = (sqlite3_step((sqlite3_stmt*)this->hStmt) == SQLITE_ROW);
	this->colCnt = ColCount();
	this->colTypes = MemAlloc(DB::DBUtil::ColType, this->colCnt);
	i = this->colCnt;
	while (i-- > 0)
	{
		this->colTypes[i] = DB::DBUtil::CT_Unknown;
	}
	this->UpdateColTypes();
}

DB::SQLiteReader::~SQLiteReader()
{
	sqlite3_finalize((sqlite3_stmt*)this->hStmt);
	MemFree(this->colTypes);
}

Bool DB::SQLiteReader::ReadNext()
{
	if (isFirst)
	{
		this->isFirst = false;
		return this->firstResult;
	}
	Bool ret = sqlite3_step((sqlite3_stmt*)this->hStmt) == SQLITE_ROW;
	if (ret)
	{
		this->UpdateColTypes();
	}
	return ret;
}

UOSInt DB::SQLiteReader::ColCount()
{
	return (UOSInt)sqlite3_column_count((sqlite3_stmt*)this->hStmt);
}

OSInt DB::SQLiteReader::GetRowChanged()
{
	return -1;
}

Int32 DB::SQLiteReader::GetInt32(UOSInt colIndex)
{
	return sqlite3_column_int((sqlite3_stmt*)this->hStmt, (int)colIndex);
}

Int64 DB::SQLiteReader::GetInt64(UOSInt colIndex)
{
	return sqlite3_column_int64((sqlite3_stmt*)this->hStmt, (int)colIndex);
}

WChar *DB::SQLiteReader::GetStr(UOSInt colIndex, WChar *buff)
{
#if _WCHAR_SIZE == 2
	const void *outp = sqlite3_column_text16((sqlite3_stmt*)this->hStmt, (int)colIndex);
	if (outp == 0)
		return 0;
	else
		return Text::StrConcat(buff, (const WChar *)outp);
#else
	const void *outp = sqlite3_column_text16((sqlite3_stmt*)this->hStmt, (int)colIndex);
	if (outp == 0)
		return 0;
	else
	{
		return Text::StrUTF16_UTF32(buff, (const UTF16Char*)outp);
	}
#endif
}

Text::String *DB::SQLiteReader::GetNewStr(UOSInt colIndex)
{
	const void *outp = sqlite3_column_text16((sqlite3_stmt*)this->hStmt, (int)colIndex);
	if (outp == 0)
		return 0;
	else
		return Text::String::NewNotNull((const UTF16Char *)outp);
}

Bool DB::SQLiteReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb)
{
	const void *outp = sqlite3_column_text16((sqlite3_stmt*)this->hStmt, (int)colIndex);
	if (outp == 0)
		return false;
	else
	{
		Text::String *s = Text::String::NewNotNull((const UTF16Char*)outp);
		sb->Append(s);
		s->Release();
		return true;
	}
}

UTF8Char *DB::SQLiteReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	const UTF8Char *outp = (const UTF8Char*)sqlite3_column_text((sqlite3_stmt*)this->hStmt, (int)colIndex);
	if (outp == 0)
		return 0;
	else
		return Text::StrConcatS(buff, outp, buffSize);
}

DB::DBReader::DateErrType DB::SQLiteReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	Text::StringBuilderUTF8 sb;
	if (!GetStr(colIndex, &sb))
		return DB::DBReader::DET_NULL;
	outVal->SetValue(sb.ToString(), sb.GetLength());
	return DB::DBReader::DET_OK;
}

Double DB::SQLiteReader::GetDbl(UOSInt colIndex)
{
	return sqlite3_column_double((sqlite3_stmt*)this->hStmt, (int)colIndex);
}

Bool DB::SQLiteReader::GetBool(UOSInt colIndex)
{
	return GetInt32(colIndex) != 0;
}

UOSInt DB::SQLiteReader::GetBinarySize(UOSInt colIndex)
{
	return (UOSInt)sqlite3_column_bytes((sqlite3_stmt*)this->hStmt, (int)colIndex);
}

Math::Vector2D *DB::SQLiteReader::GetVector(UOSInt colIndex)
{
	return 0;
}

UOSInt DB::SQLiteReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	UOSInt leng = GetBinarySize(colIndex);
	if (leng > 0)
	{
		const void *data = sqlite3_column_blob((sqlite3_stmt*)this->hStmt, (int)colIndex);
		if (data)
		{
			MemCopyNO(buff, data, leng);
			return leng;
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

Bool DB::SQLiteReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}

UTF8Char *DB::SQLiteReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	const Char *name = sqlite3_column_name((sqlite3_stmt*)this->hStmt, (int)colIndex);
	if (name == 0)
		return 0;
	return Text::StrConcat(buff, (const UTF8Char*)name);
}

Bool DB::SQLiteReader::IsNull(UOSInt colIndex)
{
	return sqlite3_column_type((sqlite3_stmt*)this->hStmt, (int)colIndex) == SQLITE_NULL;
}

DB::DBUtil::ColType DB::SQLiteReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex >= this->colCnt)
		return DB::DBUtil::CT_Unknown;
	DB::DBUtil::ColType colType = this->colTypes[colIndex];;
	if (colType == DB::DBUtil::CT_Int32)
	{
		if (colSize)
			*colSize = 4;
		return DB::DBUtil::CT_Int32;
	}
	else if (colType == DB::DBUtil::CT_Double)
	{
		if (colSize)
			*colSize = 8;
		return DB::DBUtil::CT_Double;
	}
	else if (colType == DB::DBUtil::CT_VarChar)
	{
		if (colSize)
			*colSize = GetBinarySize(colIndex);
		return DB::DBUtil::CT_VarChar;
	}
	else if (colType == DB::DBUtil::CT_Binary)
	{
		if (colSize)
			*colSize = GetBinarySize(colIndex);
		return DB::DBUtil::CT_Binary;
	}
	else
	{
		return DB::DBUtil::CT_Unknown;
	}
}

Bool DB::SQLiteReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	const Char *name = sqlite3_column_name((sqlite3_stmt*)this->hStmt, (int)colIndex);
	UOSInt colSize;
	DB::DBUtil::ColType colType;
	if (name == 0)
		return false;
	colDef->SetColName((const UTF8Char*)name);
	colType = GetColType(colIndex, &colSize);
	colDef->SetColType(colType);
	colDef->SetColSize(colSize);
	if (colType == DB::DBUtil::CT_Double)
		colDef->SetColDP(10);
	else
		colDef->SetColDP(0);
	colDef->SetNotNull(false);
	colDef->SetPK(false);
	colDef->SetAutoInc(false);
	colDef->SetDefVal((const UTF8Char*)0);
	colDef->SetAttr((const UTF8Char*)0);
	return true;
}
