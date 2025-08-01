#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/DBConn.h"
#include "DB/DBTool.h"
#include "DB/MySQLConn.h"
#include "Net/MySQLUtil.h"
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

	if (mysql_real_connect((MYSQL*)this->mysql, (const Char*)this->server->v.Ptr(), (const Char*)this->uid->v.Ptr(), (const Char*)this->pwd->v.Ptr(), (const Char*)OPTSTR_CSTR(this->database).v.Ptr(), 0, 0, 0) == 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in connecting to database: "));
		this->GetLastErrorMsg(sb);
		log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		mysql_close(mysql);
		this->mysql = 0;
	}
	else
	{
		log->LogMessage(CSTR("Driver is libmysql"), IO::LogHandler::LogLevel::Action);
		mysql_set_character_set((MYSQL*)this->mysql, "utf8");

		NN<DB::DBReader> r;
		if (this->ExecuteReader(CSTR("SELECT VERSION()")).SetTo(r))
		{
			r->ReadNext();
			NN<Text::String> s;
			if (r->GetNewStr(0).SetTo(s))
			{
				this->axisAware = Net::MySQLUtil::IsAxisAware(s->ToCString());
				s->Release();
			}
			this->CloseReader(r);
		}
	}
}

DB::MySQLConn::MySQLConn(NN<Text::String> server, NN<Text::String> uid, NN<Text::String> pwd, Optional<Text::String> database, NN<IO::LogTool> log) : DB::DBConn(CSTR("MySQLConn"))
{
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
	}

	this->mysql = 0;
	this->server = server->Clone();
	this->uid = uid->Clone();
	this->pwd = pwd->Clone();
	this->database = Text::String::CopyOrNull(database);
	this->log = log;
	this->axisAware = false;
	Connect();
}

DB::MySQLConn::MySQLConn(Text::CStringNN server, Text::CStringNN uid, Text::CStringNN pwd, Text::CString database, NN<IO::LogTool> log) : DB::DBConn(CSTR("MySQLConn"))
{
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
	}

	this->mysql = 0;
	this->server = Text::String::New(server);
	this->uid = Text::String::New(uid);
	this->pwd = Text::String::New(pwd);
	this->database = Text::String::NewOrNull(database);
	this->log = log;
	this->axisAware = false;
	Connect();
}

DB::MySQLConn::MySQLConn(const WChar *server, const WChar *uid, const WChar *pwd, const WChar *database, NN<IO::LogTool> log) : DB::DBConn(CSTR("MySQLConn"))
{
	if (Sync::Interlocked::IncrementI32(useCnt) == 1)
	{
	}

	this->mysql = 0;
	this->server = Text::String::NewNotNull(server);
	this->uid = Text::String::NewNotNull(uid);
	this->pwd = Text::String::NewNotNull(pwd);
	this->database = Text::String::NewOrNull(database);
	this->log = log;
	this->axisAware = false;
	Connect();
}

DB::MySQLConn::~MySQLConn()
{
	Close();
	this->server->Release();
	OPTSTR_DEL(this->database);
	this->uid->Release();
	this->pwd->Release();
	if (Sync::Interlocked::DecrementI32(useCnt) == 0)
	{
		mysql_library_end();
	}
}

DB::SQLType DB::MySQLConn::GetSQLType() const
{
	return DB::SQLType::MySQL;
}

Bool DB::MySQLConn::IsAxisAware() const
{
	return this->axisAware;
}

DB::DBConn::ConnType DB::MySQLConn::GetConnType() const
{
	return DB::DBConn::CT_MYSQL;
}

Int8 DB::MySQLConn::GetTzQhr() const
{
	return 0;
}

void DB::MySQLConn::ForceTz(Int8 tzQhr)
{
}

void DB::MySQLConn::GetConnName(NN<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("MySQL:"));
	sb->Append(this->server);
	NN<Text::String> s;
	if (this->database.SetTo(s))
	{
		sb->AppendUTF8Char('/');
		sb->Append(s);
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

OSInt DB::MySQLConn::ExecuteNonQuery(Text::CStringNN sql)
{
	if (this->mysql == 0)
	{
		this->lastDataError = DE_CONN_ERROR;
		return -2;
	}

	if (mysql_real_query((MYSQL*)this->mysql, (const Char*)sql.v.Ptr(), (UInt32)sql.leng) == 0)
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

Optional<DB::DBReader> DB::MySQLConn::ExecuteReader(Text::CStringNN sql)
{
	if (this->mysql == 0)
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}

	if (mysql_real_query((MYSQL*)this->mysql, (const Char*)sql.v.Ptr(), (UInt32)sql.leng) == 0)
	{
		MYSQL_RES *result;
		result = mysql_use_result((MYSQL*)this->mysql);
		if (result)
		{
			NN<DB::DBReader> r;
			this->lastDataError = DE_NO_ERROR;
			NEW_CLASSNN(r, DB::MySQLReader((OSInt)mysql_affected_rows((MYSQL*)this->mysql), result));
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

void DB::MySQLConn::CloseReader(NN<DB::DBReader> r)
{
	DB::MySQLReader *rdr = (DB::MySQLReader*)r.Ptr();
	DEL_CLASS(rdr);
}

void DB::MySQLConn::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	UTF8Char *errMsg = (UTF8Char *)mysql_error((MYSQL*)this->mysql);
	str->AppendSlow(errMsg);
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

Optional<DB::DBTransaction> DB::MySQLConn::BeginTransaction()
{
	return (DB::DBTransaction*)1;
}

void DB::MySQLConn::Commit(NN<DB::DBTransaction> tran)
{
}

void DB::MySQLConn::Rollback(NN<DB::DBTransaction> tran)
{
}

UOSInt DB::MySQLConn::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt len;
	UOSInt initCnt = names->GetCount();
	NN<DB::DBReader> rdr;
	if (this->ExecuteReader(CSTR("show tables")).SetTo(rdr))
	{
		while (rdr->ReadNext())
		{
			sptr = rdr->GetStr(0, sbuff, sizeof(sbuff)).Or(sbuff);
			len = (UOSInt)(sptr - sbuff);
			names->Add(Text::String::New(sbuff, len));
		}
		this->CloseReader(rdr);
	}
	return names->GetCount() - initCnt;
}

Optional<DB::DBReader> DB::MySQLConn::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	Data::ArrayIterator<NN<Text::String>> it;
	NN<Data::ArrayListStringNN> nncolumnName;
	NN<Data::QueryConditions> nncondition;
	sb.AppendC(UTF8STRC("select "));
	if (columnNames.SetTo(nncolumnName) && (it = nncolumnName->Iterator()).HasNext())
	{
		sptr = DB::DBUtil::SDBColUTF8(sbuff, it.Next()->v, DB::SQLType::MySQL);
		sb.AppendP(sbuff, sptr);
		while (it.HasNext())
		{
			sb.AppendUTF8Char(',');
			sptr = DB::DBUtil::SDBColUTF8(sbuff, it.Next()->v, DB::SQLType::MySQL);
			sb.AppendP(sbuff, sptr);
		}
	}
	else
	{
		sb.AppendUTF8Char('*');
	}
	sb.AppendC(UTF8STRC(" from "));
	Text::CStringNN nns;
	if (schemaName.SetTo(nns) && nns.leng > 0)
	{
		sptr = DB::DBUtil::SDBColUTF8(sbuff, nns.v, DB::SQLType::MySQL);
		sb.AppendP(sbuff, sptr);
		sb.AppendUTF8Char('.');
	}
	sptr = DB::DBUtil::SDBColUTF8(sbuff, tableName.v, DB::SQLType::MySQL);
	sb.AppendP(sbuff, sptr);
	if (condition.SetTo(nncondition))
	{
		sb.AppendC(UTF8STRC(" where "));
		Data::ArrayListNN<Data::Conditions::BooleanObject> cliCond;
		nncondition->ToWhereClause(sb, DB::SQLType::MySQL, 0, 100, cliCond);
	}
	if (ordering.SetTo(nns) && nns.leng > 0)
	{
		sb.AppendC(UTF8STRC(" order by "));
		sb.Append(nns);
	}
	if (maxCnt > 0)
	{
		sb.AppendC(UTF8STRC(" LIMIT "));
		sb.AppendUOSInt(maxCnt);
	}
	return this->ExecuteReader(sb.ToCString());
}

Bool DB::MySQLConn::IsConnError()
{
	return this->mysql == 0;
}

NN<Text::String> DB::MySQLConn::GetConnServer()
{
	return this->server;
}

Optional<Text::String> DB::MySQLConn::GetConnDB()
{
	return this->database;
}

Optional<Text::String> DB::MySQLConn::GetConnUID()
{
	return this->uid;
}

Optional<Text::String> DB::MySQLConn::GetConnPWD()
{
	return this->pwd;
}

/*Optional<DB::DBTool> DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log)
{
	DB::MySQLConn *conn;
	DB::DBTool *db;
	NEW_CLASS(conn, DB::MySQLConn(serverName, uid, pwd, dbName, log));
	if (!conn->IsConnError())
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, CSTR_NULL));
		return db;
	}
	else
	{
		DEL_CLASS(conn);
		return 0;
	}
}*/

Optional<DB::DBTool> DB::MySQLConn::CreateDBTool(NN<Net::TCPClientFactory> clif, NN<Text::String> serverName, Optional<Text::String> dbName, NN<Text::String> uid, NN<Text::String> pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<DB::MySQLConn> conn;
	DB::DBTool *db;
	NEW_CLASSNN(conn, DB::MySQLConn(serverName, uid, pwd, dbName, log));
	if (!conn->IsConnError())
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		conn.Delete();
		return 0;
	}
}

Optional<DB::DBTool> DB::MySQLConn::CreateDBTool(NN<Net::TCPClientFactory> clif, Text::CStringNN serverName, Text::CString dbName, Text::CStringNN uid, Text::CStringNN pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<DB::MySQLConn> conn;
	DB::DBTool *db;
	NEW_CLASSNN(conn, DB::MySQLConn(serverName, uid, pwd, dbName, log));
	if (!conn->IsConnError())
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		conn.Delete();
		return 0;
	}
}

/*Optional<DB::DBTool> DB::MySQLConn::CreateDBTool(const WChar *serverName, const WChar *dbName, const WChar *uid, const WChar *pwd, IO::LogTool *log, Text::CString logPrefix)
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
}*/

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
		return Text::StrToInt32Ch(((MYSQL_ROW)row)[colIndex]);
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
		return Text::StrToInt64Ch(((MYSQL_ROW)this->row)[colIndex]);
	}
	else
	{
		return 0;
	}
}

UnsafeArrayOpt<WChar> DB::MySQLReader::GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
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

Bool DB::MySQLReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return false;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		sb->AppendSlow((const UTF8Char*)((MYSQL_ROW)this->row)[colIndex]);
		return true;
	}
	else
	{
		return false;
	}
}

Optional<Text::String> DB::MySQLReader::GetNewStr(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return Text::String::NewOrNullSlow((const UTF8Char*)((MYSQL_ROW)this->row)[colIndex]);
	}
	else
	{
		return 0;
	}
}

UnsafeArrayOpt<UTF8Char> DB::MySQLReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
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

Data::Timestamp DB::MySQLReader::GetTimestamp(UOSInt colIndex)
{
	if (this->row == 0)
		return Data::Timestamp(0);
	if (colIndex >= this->colCount)
		return Data::Timestamp(0);
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		const UTF8Char *s = (const UTF8Char*)((MYSQL_ROW)this->row)[colIndex];
		return Data::Timestamp(Text::CStringNN::FromPtr(s), 0);
	}
	else
	{
		return Data::Timestamp(0);
	}
}

Double DB::MySQLReader::GetDblOrNAN(UOSInt colIndex)
{
	if (this->row == 0)
		return NAN;
	if (colIndex >= this->colCount)
		return NAN;
	if (((MYSQL_ROW)this->row)[colIndex])
	{
		return Text::StrToDoubleOrNANCh(((MYSQL_ROW)this->row)[colIndex]);
	}
	else
	{
		return NAN;
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
		return Text::StrToBoolCh(((MYSQL_ROW)this->row)[colIndex]);
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

UOSInt DB::MySQLReader::GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	MemCopyNO(buff.Ptr(), ((MYSQL_ROW)this->row)[colIndex], this->lengs[colIndex]);
	return this->lengs[colIndex];
}

Optional<Math::Geometry::Vector2D> DB::MySQLReader::GetVector(UOSInt colIndex)
{
	if (this->row == 0)
		return 0;
	if (colIndex >= this->colCount)
		return 0;
	return 0;
}

Bool DB::MySQLReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	return false;
}

UOSInt DB::MySQLReader::ColCount()
{
	return this->colCount;
}

UnsafeArrayOpt<UTF8Char> DB::MySQLReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
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

DB::DBUtil::ColType DB::MySQLReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (this->row == 0)
		return DB::DBUtil::CT_Unknown;
	if (colIndex >= this->colCount)
		return DB::DBUtil::CT_Unknown;
	MYSQL_FIELD *field = mysql_fetch_field_direct((MYSQL_RES*)this->result, (UInt32)colIndex);
	colSize.Set(field->length);
	return ToColType(field->type, field->flags, field->length);
}

Bool DB::MySQLReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	if (this->row == 0)
		return false;
	if (colIndex >= this->colCount)
		return false;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	MYSQL_FIELD *field = mysql_fetch_field_direct((MYSQL_RES*)this->result, (UInt32)colIndex);
	sptr = Text::StrConcatC(sbuff, (UInt8*)field->name, field->name_length);
	colDef->SetColName(CSTRP(sbuff, sptr));
	colDef->SetColType(ToColType(field->type, field->flags, field->length));
	colDef->SetColSize(field->length);
	colDef->SetColDP(field->decimals);
	if (field->def)
	{
		sptr = Text::StrConcatC(sbuff, (UInt8*)field->def, field->def_length);
		colDef->SetDefVal(CSTRP(sbuff, sptr));
	}
	else
	{
		colDef->SetDefVal(CSTR_NULL);
	}
	colDef->SetAttr(CSTR_NULL);
	if (field->flags & AUTO_INCREMENT_FLAG)
	{
		colDef->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
	}
	else
	{
		colDef->SetAutoIncNone();
	}
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
		return DB::DBUtil::CT_VarUTF8Char;
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
			return DB::DBUtil::CT_VarUTF8Char;
		}
	case MYSQL_TYPE_MEDIUM_BLOB:
		if (flags & BINARY_FLAG)
		{
			return DB::DBUtil::CT_Binary;
		}
		else
		{
			return DB::DBUtil::CT_VarUTF8Char;
		}
	case MYSQL_TYPE_LONG_BLOB:
		if (flags & BINARY_FLAG)
		{
			return DB::DBUtil::CT_Binary;
		}
		else
		{
			return DB::DBUtil::CT_VarUTF8Char;
		}
	case MYSQL_TYPE_BLOB:
		if (flags & BINARY_FLAG)
		{
			return DB::DBUtil::CT_Binary;
		}
		else
		{
			return DB::DBUtil::CT_VarUTF8Char;
		}
	case MYSQL_TYPE_VAR_STRING:
		return DB::DBUtil::CT_VarUTF8Char;
	case MYSQL_TYPE_STRING:
		return DB::DBUtil::CT_VarUTF8Char;
	case MYSQL_TYPE_GEOMETRY:
		return DB::DBUtil::CT_Unknown;
	default:
		return DB::DBUtil::CT_Unknown;
	}
}
