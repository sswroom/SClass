#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/DBConn.h"
#include "DB/DBTool.h"
#include "DB/MySQLConn.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
//#include <windows.h>
#include <mysql.h>
#include <wchar.h>
#include <signal.h>

Int32 DB::MySQLConn::useCnt = 0;

void DB::MySQLConn::Connect()
{
	Close();

	MYSQL *mysql = mysql_init(0);
	this->mysql = mysql;

	if (mysql_real_connect((MYSQL*)this->mysql, (const Char*)STR_PTR(this->server), (const Char*)STR_PTR(this->uid), (const Char*)STR_PTR(this->pwd), (const Char*)STR_PTR(this->database), 0, 0, 0) == 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"Error in connecting to database: ");
		this->GetErrorMsg(&sb);
		log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERROR);
		mysql_close(mysql);
		this->mysql = 0;
	}
	else
	{
		log->LogMessage((const UTF8Char*)"Driver is libmysql", IO::ILogHandler::LOG_LEVEL_ACTION);
		mysql_set_character_set((MYSQL*)this->mysql, "utf8");
	}
}

DB::MySQLConn::MySQLConn(const UTF8Char *server, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *database, IO::LogTool *log) : DB::DBConn((const UTF8Char*)"MySQLConn")
{
	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
	}

	this->mysql = 0;
	this->server = Text::String::NewOrNull(server);
	this->uid = Text::String::NewOrNull(uid);
	this->pwd = Text::String::NewOrNull(pwd);
	this->database = Text::String::NewOrNull(database);
	this->log = log;
	this->tableNames = 0;
	Connect();
}

DB::MySQLConn::MySQLConn(const WChar *server, const WChar *uid, const WChar *pwd, const WChar *database, IO::LogTool *log) : DB::DBConn((const UTF8Char*)"MySQLConn")
{
	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
	}

	this->mysql = 0;
	this->server = Text::String::NewOrNull(server);
	this->uid = Text::String::NewOrNull(uid);
	this->pwd = Text::String::NewOrNull(pwd);
	this->database = Text::String::NewOrNull(database);
	this->log = log;
	this->tableNames = 0;
	Connect();
}

DB::MySQLConn::~MySQLConn()
{
	Close();
	SDEL_STRING(this->server);
	SDEL_STRING(this->database);
	SDEL_STRING(this->uid);
	SDEL_STRING(this->pwd);
	if (this->tableNames)
	{
		UOSInt i = this->tableNames->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->tableNames->GetItem(i));
		}
		DEL_CLASS(this->tableNames);
	}
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
		mysql_library_end();
	}
}

DB::DBUtil::ServerType DB::MySQLConn::GetSvrType()
{
	return DB::DBUtil::ServerType::MySQL;
}

DB::DBConn::ConnType DB::MySQLConn::GetConnType()
{
	return DB::DBConn::CT_MYSQL;
}

Int8 DB::MySQLConn::GetTzQhr()
{
	return 0;
}

void DB::MySQLConn::ForceTz(Int8 tzQhr)
{
}

void DB::MySQLConn::GetConnName(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char*)"MySQL:");
	sb->Append(this->server);
	if (this->database)
	{
		sb->AppendChar('/', 1);
		sb->Append(this->database);
	}
}

void DB::MySQLConn::Close()
{
	if (this->mysql)
	{
		mysql_close((MYSQL*)this->mysql);
		this->mysql = 0;
	}
}

void DB::MySQLConn::Dispose()
{
	DEL_CLASS(this);
}

OSInt DB::MySQLConn::ExecuteNonQuery(const UTF8Char *sql)
{
	if (this->mysql == 0)
	{
		this->lastDataError = DE_CONN_ERROR;
		return -2;
	}

	UOSInt i = Text::StrCharCnt(sql);
	if (mysql_real_query((MYSQL*)this->mysql, (const Char*)sql, (UInt32)i) == 0)
	{
		MYSQL_RES *result;
		this->lastDataError = DE_NO_ERROR;
		result = mysql_use_result((MYSQL*)this->mysql);
		if (result)
		{
			mysql_free_result(result);
			return (OSInt)mysql_affected_rows((MYSQL*)this->mysql);
		}
		else
		{
			return (OSInt)mysql_affected_rows((MYSQL*)this->mysql);
		}
	}
	else
	{
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return -2;
	}
}

/*OSInt DB::MySQLConn::ExecuteNonQuery(const WChar *sql)
{
	if (this->mysql == 0)
		return -2;

	OSInt sqlLen;
	const UTF8Char *sqlBuff = Text::StrToUTF8New(sql);
	sqlLen = Text::StrCharCnt(sqlBuff);
	if (mysql_real_query((MYSQL*)this->mysql, (const Char*)sqlBuff, (Int32)sqlLen) == 0)
	{
		MYSQL_RES *result;
		result = mysql_use_result((MYSQL*)this->mysql);
		if (result)
		{
			mysql_free_result(result);
			Text::StrDelNew(sqlBuff);
			return mysql_affected_rows((MYSQL*)this->mysql);
		}
		else
		{
			Text::StrDelNew(sqlBuff);
			return mysql_affected_rows((MYSQL*)this->mysql);
		}
	}
	else
	{
		Text::StrDelNew(sqlBuff);
		return -2;
	}
}*/

DB::DBReader *DB::MySQLConn::ExecuteReader(const UTF8Char *sql)
{
	if (this->mysql == 0)
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}

	UOSInt i = Text::StrCharCnt(sql);
	if (mysql_real_query((MYSQL*)this->mysql, (const Char*)sql, (UInt32)i) == 0)
	{
		MYSQL_RES *result;
		result = mysql_use_result((MYSQL*)this->mysql);
		if (result)
		{
			DB::DBReader *r;
			this->lastDataError = DE_NO_ERROR;
			NEW_CLASS(r, DB::MySQLReader((OSInt)mysql_affected_rows((MYSQL*)this->mysql), result));
			return r;
		}
		else
		{
			this->lastDataError = DE_NO_ERROR;
			return 0;
		}
	}
	else
	{
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
}

/*DB::DBReader *DB::MySQLConn::ExecuteReader(const WChar *sql)
{
	if (this->mysql == 0)
		return 0;

	OSInt sqlLen;
	const UTF8Char *sqlBuff = Text::StrToUTF8New(sql);
	sqlLen = Text::StrCharCnt(sqlBuff);
	if (mysql_real_query((MYSQL*)this->mysql, (const Char*)sqlBuff, (Int32)sqlLen) == 0)
	{
		MYSQL_RES *result;
		result = mysql_use_result((MYSQL*)this->mysql);
		if (result)
		{
			DB::DBReader *r;
			NEW_CLASS(r, DB::MySQLReader(this, result));
			Text::StrDelNew(sqlBuff);
			return r;
		}
		else
		{
			Text::StrDelNew(sqlBuff);
			return 0;
		}
	}
	else
	{
		Text::StrDelNew(sqlBuff);
		return 0;
	}
}*/

void DB::MySQLConn::CloseReader(DB::DBReader *r)
{
	DB::MySQLReader *rdr = (DB::MySQLReader*)r;
	DEL_CLASS(rdr);
}

void DB::MySQLConn::GetErrorMsg(Text::StringBuilderUTF *str)
{
	UTF8Char *errMsg = (UTF8Char *)mysql_error((MYSQL*)this->mysql);
	str->Append(errMsg);
}

Bool DB::MySQLConn::IsLastDataError()
{
	return this->lastDataError != DE_NO_ERROR && this->lastDataError != DE_NO_CONN;
}

void DB::MySQLConn::Reconnect()
{
	Close();
	Connect();
}

void *DB::MySQLConn::BeginTransaction()
{
	return (void*)1;
}

void DB::MySQLConn::Commit(void *tran)
{
}

void DB::MySQLConn::Rollback(void *tran)
{
}

UOSInt DB::MySQLConn::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	if (this->tableNames)
	{
		names->AddAll(this->tableNames);
		return this->tableNames->GetCount();
	}
	else
	{
		UTF8Char sbuff[256];
		DB::DBReader *rdr = this->ExecuteReader((const UTF8Char*)"show tables");
		NEW_CLASS(this->tableNames, Data::ArrayList<const UTF8Char*>());
		if (rdr)
		{
			while (rdr->ReadNext())
			{
				rdr->GetStr(0, sbuff, sizeof(sbuff));
				this->tableNames->Add(Text::StrCopyNew(sbuff));
			}
			this->CloseReader(rdr);
		}
		names->AddAll(this->tableNames);
		return this->tableNames->GetCount();
	}
}

DB::DBReader *DB::MySQLConn::GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"select * from ");
	UOSInt i = 0;
	UOSInt j;
	while (true)
	{
		j = Text::StrIndexOf(&tableName[i], '.');
		if (j == INVALID_INDEX)
		{
			DB::DBUtil::SDBColUTF8(sbuff, &tableName[i], DB::DBUtil::ServerType::MySQL);
			sb.Append(sbuff);
			break;
		}
		sptr = Text::StrConcatC(sbuff, &tableName[i], (UOSInt)j);
		DB::DBUtil::SDBColUTF8(sptr + 1, sbuff, DB::DBUtil::ServerType::MySQL);
		sb.Append(sptr + 1);
		sb.AppendChar('.', 1);
		i += j + 1;
	}
	if (maxCnt > 0)
	{
		sb.Append((const UTF8Char*)" LIMIT ");
		sb.AppendUOSInt(maxCnt);
	}
	return this->ExecuteReader(sb.ToString());
}

Bool DB::MySQLConn::IsConnError()
{
	return this->mysql == 0;
}

Text::String *DB::MySQLConn::GetConnServer()
{
	return this->server;
}

Text::String *DB::MySQLConn::GetConnDB()
{
	return this->database;
}

Text::String *DB::MySQLConn::GetConnUID()
{
	return this->uid;
}

Text::String *DB::MySQLConn::GetConnPWD()
{
	return this->pwd;
}

DB::DBTool *DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log)
{
	DB::MySQLConn *conn;
	DB::DBTool *db;
	NEW_CLASS(conn, DB::MySQLConn(serverName, uid, pwd, dbName, log));
	if (!conn->IsConnError())
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, 0));
		return db;
	}
	else
	{
		DEL_CLASS(conn);
		return 0;
	}
}

DB::DBTool *DB::MySQLConn::CreateDBTool(Net::SocketFactory *sockf, const UTF8Char *serverName, const UTF8Char *dbName, const UTF8Char *uid, const UTF8Char *pwd, IO::LogTool *log, const UTF8Char *logPrefix)
{
	DB::MySQLConn *conn;
	DB::DBTool *db;
	NEW_CLASS(conn, DB::MySQLConn(serverName, uid, pwd, dbName, log));
	if (!conn->IsConnError())
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		DEL_CLASS(conn);
		return 0;
	}
}

DB::DBTool *DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log, const UTF8Char *logPrefix)
{
	DB::MySQLConn *conn;
	DB::DBTool *db;
	NEW_CLASS(conn, DB::MySQLConn(serverName, uid, pwd, dbName, log));
	if (!conn->IsConnError())
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		DEL_CLASS(conn);
		return 0;
	}
}

DB::MySQLReader::MySQLReader(OSInt rowChanged, void *result)
{
	this->rowChanged = rowChanged;
	this->result = result;
	this->colCount = mysql_num_fields((MYSQL_RES *)result);
	this->row = 0;
	this->lengs = 0;
	this->names = MemAlloc(WChar *, this->colCount);
	UOSInt i;
	UOSInt j;
	i = this->colCount;
	while (i-- > 0)
	{
		MYSQL_FIELD *field = mysql_fetch_field_direct((MYSQL_RES*)this->result, (UInt32)i);
		j = Text::StrUTF8_WCharCntC((const UTF8Char*)field->name, field->name_length);
		this->names[i] = MemAlloc(WChar, j + 1);
		Text::StrUTF8_WCharC(this->names[i], (const UTF8Char*)field->name, field->name_length, 0);
	}
}

DB::MySQLReader::~MySQLReader()
{
	mysql_free_result((MYSQL_RES*)result);
	UOSInt i = this->colCount;
	while (i-- > 0)
	{
		MemFree(this->names[i]);
	}
	MemFree(this->names);

}


Bool DB::MySQLReader::ReadNext()
{
	MYSQL_ROW row;
	row = mysql_fetch_row((MYSQL_RES*)this->result);
	if (row == 0)
		return false;
	this->row = row;
	this->lengs = (UInt32*)mysql_fetch_lengths((MYSQL_RES*)this->result);
	return true;
}

OSInt DB::MySQLReader::GetRowChanged()
{
	return this->rowChanged;
}

Int32 DB::MySQLReader::GetInt32(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return Text::StrToInt32(((MYSQL_ROW)row)[colIndex]);
	}
	else
	{
		return 0;
	}
}

Int64 DB::MySQLReader::GetInt64(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return Text::StrToInt64(((MYSQL_ROW)this->row)[colIndex]);
	}
	else
	{
		return 0;
	}
}

WChar *DB::MySQLReader::GetStr(UOSInt colIndex, WChar *buff)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return Text::StrUTF8_WChar(buff, (const UTF8Char*)((MYSQL_ROW)this->row)[colIndex], 0);
	}
	else
	{
		return 0;
	}
}

Bool DB::MySQLReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return false;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		sb->Append((const UTF8Char*)((MYSQL_ROW)this->row)[colIndex]);
		return true;
	}
	else
	{
		return false;
	}
}

Text::String *DB::MySQLReader::GetNewStr(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return Text::String::NewOrNull((const UTF8Char*)((MYSQL_ROW)this->row)[colIndex]);
	}
	else
	{
		return 0;
	}
}

UTF8Char *DB::MySQLReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return Text::StrConcatS(buff, (const UTF8Char*)((MYSQL_ROW)this->row)[colIndex], buffSize);
	}
	else
	{
		return 0;
	}
}

DB::DBReader::DateErrType DB::MySQLReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	if (this->row == 0)
		return DB::DBReader::DET_ERROR;
	if (colIndex >= this->colCount)
		return DB::DBReader::DET_ERROR;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		outVal->SetValue(((MYSQL_ROW)this->row)[colIndex]);
		return DB::DBReader::DET_OK;
	}
	else
	{
		return DB::DBReader::DET_NULL;
	}
}

Double DB::MySQLReader::GetDbl(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return Text::StrToDouble(((MYSQL_ROW)this->row)[colIndex]);
	}
	else
	{
		return 0;
	}
}

Bool DB::MySQLReader::GetBool(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return Text::StrToBool(((MYSQL_ROW)this->row)[colIndex]);
	}
	else
	{
		return 0;
	}
}

UOSInt DB::MySQLReader::GetBinarySize(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	return this->lengs[colIndex];
}

UOSInt DB::MySQLReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	MemCopyNO(buff, ((MYSQL_ROW)this->row)[colIndex], this->lengs[colIndex]);
	return this->lengs[colIndex];
}

Math::Vector2D *DB::MySQLReader::GetVector(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	return 0;
}

Bool DB::MySQLReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}

UOSInt DB::MySQLReader::ColCount()
{
	return this->colCount;
}

UTF8Char *DB::MySQLReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	return Text::StrWChar_UTF8(buff, this->names[colIndex]);
}

Bool DB::MySQLReader::IsNull(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return true;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return false;
	}
	else
	{
		return true;
	}
}

DB::DBUtil::ColType DB::MySQLReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	if (this->row == 0)
		return DB::DBUtil::CT_Unknown;
	if (colIndex >= this->colCount)
		return DB::DBUtil::CT_Unknown;
	MYSQL_FIELD *field = mysql_fetch_field_direct((MYSQL_RES*)this->result, (UInt32)colIndex);
	return ToColType(field->type, field->flags, field->length);
}

Bool DB::MySQLReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (this->row == 0)
		return false;
	if (colIndex >= this->colCount)
		return false;
	UTF8Char sbuff[256];
	MYSQL_FIELD *field = mysql_fetch_field_direct((MYSQL_RES*)this->result, (UInt32)colIndex);
	Text::StrConcatC(sbuff, (UInt8*)field->name, field->name_length);
	colDef->SetColName(sbuff);
	colDef->SetColType(ToColType(field->type, field->flags, field->length));
	colDef->SetColSize(field->length);
	colDef->SetColDP(field->decimals);
	if (field->def)
	{
		Text::StrConcatC(sbuff, (UInt8*)field->def, field->def_length);
		colDef->SetDefVal(sbuff);
	}
	else
	{
		colDef->SetDefVal((const UTF8Char*)0);
	}
	colDef->SetAttr((const UTF8Char*)0);
	colDef->SetAutoInc((field->flags & AUTO_INCREMENT_FLAG) != 0);
	colDef->SetNotNull((field->flags & NOT_NULL_FLAG) != 0);
	colDef->SetPK((field->flags & PRI_KEY_FLAG) != 0);
	return true;
}

DB::DBUtil::ColType DB::MySQLReader::ToColType(Int32 dbType, UInt32 flags, UOSInt colSize)
{
	switch (dbType)
	{
	case MYSQL_TYPE_DECIMAL:
		return DB::DBUtil::CT_Double;
	case MYSQL_TYPE_TINY:
		if (colSize == 1)
			return DB::DBUtil::CT_Bool;
		else
			return DB::DBUtil::CT_Byte;
	case MYSQL_TYPE_SHORT:
		if (flags & UNSIGNED_FLAG)
		{
			return DB::DBUtil::CT_UInt16;
		}
		else
		{
			return DB::DBUtil::CT_Int16;
		}
	case MYSQL_TYPE_LONG:
		if (flags & UNSIGNED_FLAG)
		{
			return DB::DBUtil::CT_UInt32;
		}
		else
		{
			return DB::DBUtil::CT_Int32;
		}
	case MYSQL_TYPE_FLOAT:
		return DB::DBUtil::CT_Float;
	case MYSQL_TYPE_DOUBLE:
		return DB::DBUtil::CT_Double;
	case MYSQL_TYPE_NULL:
		return DB::DBUtil::CT_Unknown;
	case MYSQL_TYPE_TIMESTAMP:
		return DB::DBUtil::CT_DateTime;
	case MYSQL_TYPE_LONGLONG:
		if (flags & UNSIGNED_FLAG)
		{
			return DB::DBUtil::CT_UInt64;
		}
		else
		{
			return DB::DBUtil::CT_Int64;
		}
	case MYSQL_TYPE_INT24:
		return DB::DBUtil::CT_Int32;
	case MYSQL_TYPE_DATE:
		return DB::DBUtil::CT_DateTime;
	case MYSQL_TYPE_TIME:
		return DB::DBUtil::CT_DateTime;
	case MYSQL_TYPE_DATETIME:
		return DB::DBUtil::CT_DateTime;
	case MYSQL_TYPE_YEAR:
		return DB::DBUtil::CT_Int32;
	case MYSQL_TYPE_NEWDATE:
		return DB::DBUtil::CT_DateTime;
	case MYSQL_TYPE_VARCHAR:
		return DB::DBUtil::CT_VarChar;
	case MYSQL_TYPE_BIT:
		return DB::DBUtil::CT_Bool;
	case MYSQL_TYPE_NEWDECIMAL:
		return DB::DBUtil::CT_Double;
	case MYSQL_TYPE_ENUM:
		return DB::DBUtil::CT_Int32;
	case MYSQL_TYPE_SET:
		return DB::DBUtil::CT_Int32;
	case MYSQL_TYPE_TINY_BLOB:
		if (flags & BINARY_FLAG)
		{
			return DB::DBUtil::CT_Binary;
		}
		else
		{
			return DB::DBUtil::CT_VarChar;
		}
	case MYSQL_TYPE_MEDIUM_BLOB:
		if (flags & BINARY_FLAG)
		{
			return DB::DBUtil::CT_Binary;
		}
		else
		{
			return DB::DBUtil::CT_VarChar;
		}
	case MYSQL_TYPE_LONG_BLOB:
		if (flags & BINARY_FLAG)
		{
			return DB::DBUtil::CT_Binary;
		}
		else
		{
			return DB::DBUtil::CT_VarChar;
		}
	case MYSQL_TYPE_BLOB:
		if (flags & BINARY_FLAG)
		{
			return DB::DBUtil::CT_Binary;
		}
		else
		{
			return DB::DBUtil::CT_VarChar;
		}
	case MYSQL_TYPE_VAR_STRING:
		return DB::DBUtil::CT_VarChar;
	case MYSQL_TYPE_STRING:
		return DB::DBUtil::CT_VarChar;
	case MYSQL_TYPE_GEOMETRY:
		return DB::DBUtil::CT_Unknown;
	default:
		return DB::DBUtil::CT_Unknown;
	}
}
