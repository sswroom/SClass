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
#include "Math/WKBReader.h"
#include "Math/WKTWriter.h"
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
	sqlite3_initialize();
	ret = sqlite3_open_v2((const Char*)fileName->v, &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_SHAREDCACHE, 0);
	
	if (SQLITE_OK == ret)
	{
		Text::StringBuilderUTF8 sb;
		this->db = db;
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

DB::SQLiteFile::SQLiteFile(NotNullPtr<Text::String> fileName) : DB::DBConn(fileName)
{
	this->fileName = fileName->Clone();
	this->Init();
}

DB::SQLiteFile::SQLiteFile(Text::CStringNN fileName) : DB::DBConn(fileName)
{
	this->fileName = Text::String::New(fileName);
	this->Init();
}

DB::SQLiteFile::~SQLiteFile()
{
	if (this->db)
	{
		sqlite3_close((sqlite3*)this->db);
		this->db = 0;
	}
	sqlite3_shutdown();
	if (this->delOnClose)
	{
		IO::Path::DeleteFile(this->fileName->v);
	}
	this->fileName->Release();
	SDEL_STRING(this->lastErrMsg);
}

DB::SQLType DB::SQLiteFile::GetSQLType() const
{
	return DB::SQLType::SQLite;
}

DB::DBConn::ConnType DB::SQLiteFile::GetConnType() const
{
	return CT_SQLITE;
}

Int8 DB::SQLiteFile::GetTzQhr() const
{
	return 0;
}

void DB::SQLiteFile::ForceTz(Int8 tzQhr)
{
	
}

void DB::SQLiteFile::GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb)
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

OSInt DB::SQLiteFile::ExecuteNonQuery(Text::CStringNN sql)
{
	OSInt chg = -2;
	if (this->db)
	{
		sqlite3_stmt *stmt;
		const char *tmp;
		if (sqlite3_prepare_v2((sqlite3*)this->db, (const Char*)sql.v, (Int32)sql.leng + 1, &stmt, &tmp) == SQLITE_OK)
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
				this->lastErrMsg = Text::String::NewNotNullSlow((const UTF8Char*)sqlite3_errmsg((sqlite3*)this->db)).Ptr();
				chg = 0;
			}
			sqlite3_finalize(stmt);
			return chg;
		}
		else
		{
			SDEL_STRING(this->lastErrMsg);
			this->lastErrMsg = Text::String::NewNotNullSlow((const UTF8Char*)sqlite3_errmsg((sqlite3*)this->db)).Ptr();
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

DB::DBReader *DB::SQLiteFile::ExecuteReader(Text::CStringNN sql)
{
	if (this->db)
	{
		sqlite3_stmt *stmt;
		const char *tmp;
		if (sqlite3_prepare_v2((sqlite3*)this->db, (const Char*)sql.v, (Int32)sql.leng + 1, &stmt, &tmp) == SQLITE_OK)
		{
			this->lastDataError = DE_NO_ERROR;
			DB::SQLiteReader *r;
			NEW_CLASS(r, DB::SQLiteReader(this, stmt));
			return r;
		}
		else
		{
			this->lastDataError = DE_EXEC_SQL_ERROR;
			SDEL_STRING(this->lastErrMsg);
			this->lastErrMsg = Text::String::NewNotNullSlow((const UTF8Char*)sqlite3_errmsg((sqlite3*)this->db)).Ptr();
			return 0;
		}
	}
	else
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
}

void DB::SQLiteFile::CloseReader(NotNullPtr<DBReader> r)
{
	DB::SQLiteReader *rdr = (DB::SQLiteReader*)r.Ptr();
	DEL_CLASS(rdr);
}

void DB::SQLiteFile::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
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
	ret = sqlite3_open((const Char*)this->fileName->v, &db);
	if (SQLITE_OK == ret)
	{
		this->db = db;
	}
}

void *DB::SQLiteFile::BeginTransaction()
{
	ExecuteNonQuery(CSTR("begin"));
	return (void*)-1;
}

void DB::SQLiteFile::Commit(void *tran)
{
	ExecuteNonQuery(CSTR("end"));
}

void DB::SQLiteFile::Rollback(void *tran)
{
	ExecuteNonQuery(CSTR("rollback"));
}

UOSInt DB::SQLiteFile::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names)
{
	if (schemaName.leng > 0)
		return 0;
	UOSInt initCnt = names->GetCount();
	NotNullPtr<DB::DBReader> r;
	if (r.Set(ExecuteReader(CSTR("SELECT name FROM sqlite_master WHERE type='table'"))))
	{
		Text::StringBuilderUTF8 sb;
		NotNullPtr<Text::String> name;
		while (r->ReadNext())
		{
			name = r->GetNewStrBNN(0, sb);
			names->Add(name);
		}
		this->CloseReader(r);
	}
	return names->GetCount() - initCnt;
}

DB::DBReader *DB::SQLiteFile::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sb.AppendC(UTF8STRC("select "));
	if (columnNames == 0 || columnNames->GetCount() == 0)
	{
		sb.AppendC(UTF8STRC("*"));
	}
	else
	{
		UOSInt i = 0;
		UOSInt j = columnNames->GetCount();
		while (i < j)
		{
			if (i > 0) sb.AppendC(UTF8STRC(","));
			sptr = DB::DBUtil::SDBColUTF8(sbuff, columnNames->GetItem(i)->v, DB::SQLType::SQLite);
			sb.Append(CSTRP(sbuff, sptr));
			i++;
		}
	}
	sb.AppendC(UTF8STRC(" from "));
	sptr = DB::DBUtil::SDBColUTF8(sbuff, tableName.v, DB::SQLType::SQLite);
	sb.Append(CSTRP(sbuff, sptr));
	if (maxCnt > 0)
	{
		sb.AppendC(UTF8STRC(" LIMIT "));
		sb.AppendOSInt((OSInt)maxCnt);
	}
	return ExecuteReader(sb.ToCString());
}

void DB::SQLiteFile::SetDeleteOnClose(Bool delOnClose)
{
	this->delOnClose = delOnClose;
}

Bool DB::SQLiteFile::IsError()
{
	return this->db == 0;
}

NotNullPtr<Text::String> DB::SQLiteFile::GetFileName()
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
				this->colTypes[i] = DB::DBUtil::CT_VarUTF8Char;
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

Math::Geometry::Vector2D *DB::SQLiteFile::GPGeometryParse(const UInt8 *buff, UOSInt buffSize)
{
	if (buffSize < 8 || buff[0] != 'G' || buff[1] != 'P')
	{
		return 0;
	}

	UOSInt ofst;
	switch ((buff[3] & 0xE) >> 1)
	{
	case 0:
		ofst = 8;
		break;
	case 1:
		ofst = 40;
		break;
	case 2:
		ofst = 56;
		break;
	case 3:
		ofst = 56;
		break;
	case 4:
		ofst = 72;
		break;
	default:
		return 0;
	}

	if (buffSize <= ofst)
	{
		return 0;
	}
	UInt32 srsId;
	if (buff[3] & 1)
	{
		srsId = ReadUInt32(&buff[4]);
	}
	else
	{
		srsId = ReadMUInt32(&buff[4]);
	}
	Math::WKBReader reader(srsId);
	return reader.ParseWKB(buff + ofst, buffSize - ofst, 0);
}

Optional<DB::DBTool> DB::SQLiteFile::CreateDBTool(NotNullPtr<Text::String> fileName, NotNullPtr<IO::LogTool> log, Text::CString logPrefix)
{
	NotNullPtr<DB::SQLiteFile> conn;
	NEW_CLASSNN(conn, DB::SQLiteFile(fileName));
	if (conn->IsError())
	{
		conn.Delete();
		return 0;
	}
	NotNullPtr<DB::DBTool> db;
	NEW_CLASSNN(db, DBTool(conn, true, log, logPrefix));
	return db;
}

Optional<DB::DBTool> DB::SQLiteFile::CreateDBTool(Text::CStringNN fileName, NotNullPtr<IO::LogTool> log, Text::CString logPrefix)
{
	NotNullPtr<DB::SQLiteFile> conn;
	NEW_CLASSNN(conn, DB::SQLiteFile(fileName));
	if (conn->IsError())
	{
		conn.Delete();
		return 0;
	}
	NotNullPtr<DB::DBTool> db;
	NEW_CLASSNN(db, DBTool(conn, true, log, logPrefix));
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
	if (colIndex >= this->colCnt)
		return 0;
	Text::StringBuilderUTF8 sb;
	if (!this->GetStr(colIndex, sb))
		return 0;
	return Text::String::New(sb.ToCString()).Ptr();
}

Bool DB::SQLiteReader::GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (colIndex >= this->colCnt)
		return false;
	if (this->colTypes[colIndex] == DB::DBUtil::CT_Binary)
	{
		UOSInt len = (UOSInt)sqlite3_column_bytes((sqlite3_stmt*)this->hStmt, (int)colIndex);
		if (len > 0)
		{
			const void *data = sqlite3_column_blob((sqlite3_stmt*)this->hStmt, (int)colIndex);
			if (data)
			{
				NotNullPtr<Math::Geometry::Vector2D> vec;
				if (vec.Set(DB::SQLiteFile::GPGeometryParse((const UInt8 *)data, len)))
				{
					Math::WKTWriter wkt;
					wkt.ToText(sb, vec);
					vec.Delete();
				}
				else
				{
					sb->AppendC(UTF8STRC("0x"));
					sb->AppendHexBuff((const UInt8*)data, len, 0, Text::LineBreakType::None);
				}
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		const void *outp = sqlite3_column_text16((sqlite3_stmt*)this->hStmt, (int)colIndex);
		if (outp == 0)
			return false;
		else
		{
			NotNullPtr<Text::String> s = Text::String::NewNotNull((const UTF16Char*)outp);
			sb->Append(s);
			s->Release();
			return true;
		}
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

Data::Timestamp DB::SQLiteReader::GetTimestamp(UOSInt colIndex)
{
	Text::StringBuilderUTF8 sb;
	if (!GetStr(colIndex, sb))
		return Data::Timestamp(0);
	return Data::Timestamp(sb.ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
}

Data::Date DB::SQLiteReader::GetDate(UOSInt colIndex)
{
	Text::StringBuilderUTF8 sb;
	if (!GetStr(colIndex, sb))
		return Data::Date(nullptr);
	return Data::Date(sb.ToCString());
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

Math::Geometry::Vector2D *DB::SQLiteReader::GetVector(UOSInt colIndex)
{
	UOSInt leng = GetBinarySize(colIndex);
	if (leng > 0)
	{
		const void *data = sqlite3_column_blob((sqlite3_stmt*)this->hStmt, (int)colIndex);
		if (data)
		{
			return DB::SQLiteFile::GPGeometryParse((const UInt8*)data, leng);
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

Bool DB::SQLiteReader::GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid)
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

DB::DBUtil::ColType DB::SQLiteReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex >= this->colCnt)
		return DB::DBUtil::CT_Unknown;
	DB::DBUtil::ColType colType = this->colTypes[colIndex];;
	if (colType == DB::DBUtil::CT_Int32)
	{
		colSize.Set(4);
		return DB::DBUtil::CT_Int32;
	}
	else if (colType == DB::DBUtil::CT_Double)
	{
		colSize.Set(8);
		return DB::DBUtil::CT_Double;
	}
	else if (colType == DB::DBUtil::CT_VarUTF8Char)
	{
		colSize.Set(GetBinarySize(colIndex));
		return DB::DBUtil::CT_VarUTF8Char;
	}
	else if (colType == DB::DBUtil::CT_Binary)
	{
		colSize.Set(GetBinarySize(colIndex));
		return DB::DBUtil::CT_Binary;
	}
	else
	{
		return DB::DBUtil::CT_Unknown;
	}
}

Bool DB::SQLiteReader::GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	const Char *name = sqlite3_column_name((sqlite3_stmt*)this->hStmt, (int)colIndex);
	UOSInt colSize;
	DB::DBUtil::ColType colType;
	NotNullPtr<const UTF8Char> colName;
	if (!colName.Set((const UTF8Char*)name))
		return false;
	colDef->SetColName(colName);
	colType = GetColType(colIndex, colSize);
	colDef->SetColType(colType);
	colDef->SetColSize(colSize);
	if (colType == DB::DBUtil::CT_Double)
		colDef->SetColDP(10);
	else
		colDef->SetColDP(0);
	colDef->SetNotNull(false);
	colDef->SetPK(false);
	colDef->SetAutoIncNone();
	colDef->SetDefVal(CSTR_NULL);
	colDef->SetAttr(CSTR_NULL);
	return true;
}
