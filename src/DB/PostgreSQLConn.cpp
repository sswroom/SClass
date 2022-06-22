#include "Stdafx.h"
#include "DB/PostgreSQLConn.h"
#include "Math/Point.h"
#include "Text/MyStringW.h"
#include <libpq-fe.h>

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

struct DB::PostgreSQLConn::ClassData
{
	PGconn *conn;
};

class PostgreSQLReader : public DB::DBReader
{
private:
	PGresult *res;
	int ncol;
	int nrow;
	int currrow;
public:
	PostgreSQLReader(PGresult *res) : DBReader()
	{
		this->res = res;
		this->currrow = -1;
		this->ncol = PQnfields(this->res);
		this->nrow = PQntuples(this->res);
	}

	virtual ~PostgreSQLReader()
	{
		PQclear(this->res);
	}

public:
	virtual Bool ReadNext()
	{
		this->currrow++;
		if (this->currrow >= this->nrow)
		{
			return false;
		}
		return true;
	}

	virtual UOSInt ColCount()
	{
		return (UOSInt)this->ncol;
	}

	virtual OSInt GetRowChanged()
	{
#if defined(VERBOSE)
		char *rowChg = PQcmdTuples(this->res);
		printf("PostgreSQL: Row changed = %s\r\n", rowChg);
#endif
		////////////////////////////
		return 0;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		return item.GetAsI32();
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		return item.GetAsI64();
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		if (item.GetItemType() == Data::VariItem::ItemType::Null)
		{
			return 0;
		}
		else if (item.GetItemType() == Data::VariItem::ItemType::CStr)
		{
			return Text::StrUTF8_WChar(buff, item.GetItemValue().cstr.v, 0);
		}
		else if (item.GetItemType() == Data::VariItem::ItemType::Str)
		{
			Text::String *str = item.GetItemValue().str;
			return Text::StrUTF8_WChar(buff, str->v, 0);
		}
		Text::StringBuilderUTF8 sb;
		item.GetAsString(&sb);
		return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
	}

	virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		if (item.GetItemType() == Data::VariItem::ItemType::Null)
		{
			return false;
		}
		else
		{
			item.GetAsString(sb);
			return true;
		}
	}

	virtual Text::String *GetNewStr(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		return item.GetAsNewString();
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		return item.GetAsStringS(buff, buffSize);
	}

	virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return DateErrType::DET_ERROR;
		}
		if (item.GetItemType() == Data::VariItem::ItemType::Null)
		{
			return DateErrType::DET_NULL;
		}
		Data::Timestamp ts = item.GetAsTimestamp();
		outVal->SetValue(ts.ticks, ts.tzQhr);
		return DateErrType::DET_OK;
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		return item.GetAsF64();
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		return item.GetAsBool();
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		Data::ReadonlyArray<UInt8> *arr = item.GetAndRemoveByteArr();
		if (arr)
		{
			UOSInt ret = arr->GetCount();
			DEL_CLASS(arr);
			return ret;
		}
		return 0;
	}

	virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		Data::ReadonlyArray<UInt8> *arr = item.GetAndRemoveByteArr();
		if (arr)
		{
			UOSInt ret = arr->GetCount();
			MemCopyNO(buff, arr->GetArray(), ret);
			DEL_CLASS(arr);
			return ret;
		}
		return 0;
	}

	virtual Math::Vector2D *GetVector(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		return item.GetAndRemoveVector();
	}

	virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, &item);
		return item.GetAndRemoveUUID();
	}

	virtual Bool GetVariItem(UOSInt colIndex, Data::VariItem *item)
	{
		if (this->currrow < 0 || this->currrow >= this->nrow)
		{
			return false;
		}
		if (colIndex >= (UOSInt)this->ncol)
		{
			return false;
		}
		if (PQgetisnull(this->res, this->currrow, (int)colIndex))
		{
			item->SetNull();
			return true;
		}
		Oid colType = PQftype(this->res, (int)colIndex);
		switch (colType)
		{
		case 16: //bool
			item->SetBool(PQgetvalue(this->res, this->currrow, (int)colIndex)[0] == 't');
			return true;
		case 19: //name
		case 25: //text
		case 1042: //bpchar
		case 1043: //varchar
		case 3802: //jsonb ///////////////////////////////////
		case 16468: //hstore ////////////////////////////////
			item->SetStrSlow((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex));
			return true;
		case 20: //int8
			item->SetI64(Text::StrToInt64(PQgetvalue(this->res, this->currrow, (int)colIndex)));
			return true;
		case 21: //int2
			item->SetI16(Text::StrToInt16(PQgetvalue(this->res, this->currrow, (int)colIndex)));
			return true;
		case 23: //int4
			item->SetI32(Text::StrToInt32(PQgetvalue(this->res, this->currrow, (int)colIndex)));
			return true;
		case 701: //float8
		case 1700: //numeric
			item->SetF64(Text::StrToDouble(PQgetvalue(this->res, this->currrow, (int)colIndex)));
			return true;
		case 1184: //timestamptz
		{
			Data::DateTime dt;
			dt.SetValue(Text::CString::FromPtr((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex)));
			item->SetDate(&dt);
			return true;
		}
		case 2950: //uuid
		{
			Data::UUID *uuid;
			NEW_CLASS(uuid, Data::UUID(Text::CString::FromPtr((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex))));
			item->SetUUIDDirect(uuid);
			return true;
		}
		case 1005: //_int2
		{
			Data::ArrayList<Int16> arr;
			Int16 v;
			Text::StringBuilderUTF8 sb;
			sb.AppendSlow((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex));
			if (sb.ToString()[0] != '{' || !sb.EndsWith('}'))
			{
				return false;
			}
			Text::PString sarr[2];
			UOSInt scnt;
			sb.RemoveChars(1);
			sarr[1] = sb.Substring(1);
			while (true)
			{
				scnt = Text::StrSplitP(sarr, 2, sarr[1], ',');
				if (sarr[0].ToInt16(&v))
				{
					arr.Add(v);
				}
				if (scnt != 2)
				{
					break;
				}
			}
			Int16 *bytes = arr.GetArray(&scnt);
			item->SetByteArr((const UInt8*)bytes, scnt * sizeof(v));
			return true;
		}
		case 1016: //_int8
		{
			Data::ArrayList<Int64> arr;
			Int64 v;
			Text::StringBuilderUTF8 sb;
			sb.AppendSlow((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex));
			if (sb.ToString()[0] != '{' || !sb.EndsWith('}'))
			{
				return false;
			}
			Text::PString sarr[2];
			UOSInt scnt;
			sb.RemoveChars(1);
			sarr[1] = sb.Substring(1);
			while (true)
			{
				scnt = Text::StrSplitP(sarr, 2, sarr[1], ',');
				if (sarr[0].ToInt64(&v))
				{
					arr.Add(v);
				}
				if (scnt != 2)
				{
					break;
				}
			}
			Int64 *bytes = arr.GetArray(&scnt);
			item->SetByteArr((const UInt8*)bytes, scnt * sizeof(v));
			return true;
		}
		case 600: //point
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendSlow((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex));
			if (sb.ToString()[0] != '(' || !sb.EndsWith(')'))
			{
				return false;
			}
			Text::PString sarr[3];
			sb.RemoveChars(1);
			if (Text::StrSplitP(sarr, 3, sb.Substring(1), ',') != 2)
			{
				return false;
			}
			Math::Point *pt;
			NEW_CLASS(pt, Math::Point(0, sarr[0].ToDouble(), sarr[1].ToDouble()));
			item->SetVectorDirect(pt);
			return true;
		}
		case 17: //bytea
		{
			const UTF8Char *val = (const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex);
			if (val[0] == '\\' && val[1] == 'x')
			{
				UOSInt len = Text::StrCharCnt(val);
				UInt8 *buff = MemAlloc(UInt8, (len >> 1) - 1);
				len = Text::StrHex2Bytes(val + 2, buff);
				item->SetByteArr(buff, len);
				MemFree(buff);
				return true;
			}
			else
			{
				item->SetByteArr(val, (UOSInt)PQgetlength(this->res, this->currrow, (int)colIndex));
			}
			return true;
		}
		//////////////////////////////////
		default:
#if defined(VERBOSE)
			printf("PostgreSQL: Unsupported type: %d, val = %s\r\n", colType, PQgetvalue(this->res, this->currrow, (int)colIndex));
#endif
			return false;
		}
	}

	virtual Bool IsNull(UOSInt colIndex)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return false;
		}
		return item.GetItemType() == Data::VariItem::ItemType::Null;
	}

	virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
	{
		char *name = PQfname(this->res, (int)colIndex);
		if (name)
		{
			return Text::StrConcat(buff, (const UTF8Char*)name);
		}
		return 0;
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize)
	{
		Oid oid = PQftype(this->res, (int)colIndex);
		return DB::PostgreSQLConn::DBType2ColType(oid);
	}

	virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef)
	{
		if (colIndex >= (UOSInt)this->ncol)
		{
			return false;
		}
		char *name = PQfname(this->res, (int)colIndex);
		if (name == 0)
		{
			return false;
		}
		colDef->SetColName((const UTF8Char*)name);
		colDef->SetColType(DB::PostgreSQLConn::DBType2ColType(PQftype(this->res, (int)colIndex)));
		int len = PQfsize(this->res, (int)colIndex);
		if (len < 0)
		{
			colDef->SetColSize(65535);
		}
		else
		{
			colDef->SetColSize((UOSInt)len);
		}
		//////////////////////////////////
		return true;
	}
};

void DB::PostgreSQLConn::Connect()
{
	if (this->clsData->conn == 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("host="));
		sb.Append(this->server);
		sb.AppendC(UTF8STRC(" port="));
		sb.AppendU16(this->port);
		sb.AppendC(UTF8STRC(" dbname="));
		sb.Append(this->database);
		if (this->uid)
		{
			sb.AppendC(UTF8STRC(" user="));
			sb.Append(this->uid);
		}
		if (this->pwd)
		{
			sb.AppendC(UTF8STRC(" password="));
			sb.Append(this->pwd);
		}
		this->clsData->conn = PQconnectdb((const char*)sb.ToString());
		this->isTran = false;
		if (this->log)
		{
			if (PQstatus(this->clsData->conn) != CONNECTION_OK)
			{
				char *msg = PQerrorMessage(this->clsData->conn);
				UOSInt msgLen = Text::StrCharCnt(msg);
				this->log->LogMessage(Text::CString((const UTF8Char*)msg, msgLen), IO::ILogHandler::LOG_LEVEL_ERROR);
#if defined(VERBOSE)
				printf("PostgreSQL: Error, %s\r\n", msg);
#endif				
			}
#if defined(VERBOSE)
			else
			{
				printf("PostgreSQL: DB Connected\r\n");
			}
#endif
		}
	}
}

void DB::PostgreSQLConn::ClearTableNames()
{
	if (this->tableNames)
	{
		UOSInt i = this->tableNames->GetCount();
		while (i-- > 0)
		{
			this->tableNames->GetItem(i)->Release();
		}
		DEL_CLASS(this->tableNames);
		this->tableNames = 0;
	}
}

DB::PostgreSQLConn::PostgreSQLConn(Text::String *server, UInt16 port, Text::String *uid, Text::String *pwd, Text::String *database, IO::LogTool *log) : DBConn(server)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->conn = 0;
	this->tzQhr = 0;
	this->log = log;
	this->server = server->Clone();
	this->port = port;
	this->database = database->Clone();
	this->uid = SCOPY_STRING(uid);
	this->pwd = SCOPY_STRING(pwd);
	this->tableNames = 0;
	this->Connect();
}

DB::PostgreSQLConn::PostgreSQLConn(Text::CString server, UInt16 port, Text::CString uid, Text::CString pwd, Text::CString database, IO::LogTool *log) : DBConn(server)
{
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->conn = 0;
	this->tzQhr = 0;
	this->log = log;
	this->server = Text::String::New(server);
	this->port = port;
	this->database = Text::String::New(database);
	this->uid = Text::String::NewOrNull(uid);
	this->pwd = Text::String::NewOrNull(pwd);
	this->tableNames = 0;
	this->Connect();
}

DB::PostgreSQLConn::~PostgreSQLConn()
{
	this->Close();
	this->server->Release();
	this->database->Release();
	SDEL_STRING(this->uid);
	SDEL_STRING(this->pwd);
	MemFree(this->clsData);
}

DB::DBUtil::ServerType DB::PostgreSQLConn::GetSvrType()
{
	return DB::DBUtil::ServerType::PostgreSQL;
}

DB::DBConn::ConnType DB::PostgreSQLConn::GetConnType()
{
	return DB::DBConn::CT_POSTGRESQL;
}

Int8 DB::PostgreSQLConn::GetTzQhr()
{
	return this->tzQhr;
}

void DB::PostgreSQLConn::ForceTz(Int8 tzQhr)
{
	this->tzQhr = tzQhr;
}

void DB::PostgreSQLConn::GetConnName(Text::StringBuilderUTF8 *sb)
{
	sb->AppendC(UTF8STRC("PostgreSQL"));
}

void DB::PostgreSQLConn::Close()
{
	if (this->clsData->conn)
	{
		this->ClearTableNames();
		PQfinish(this->clsData->conn);
		this->clsData->conn = 0;
#if defined(VERBOSE)
		printf("PostgreSQL: DB disconnected\r\n");
#endif
	}
}

void DB::PostgreSQLConn::Dispose()
{
	DEL_CLASS(this);
}

OSInt DB::PostgreSQLConn::ExecuteNonQuery(Text::CString sql)
{
	if (this->clsData->conn == 0)
	{
		return -2;
	}
	PGresult *res = PQexec(this->clsData->conn, (const char*)sql.v);
#if defined(VERBOSE)
	printf("PostgreSQL: ExecuteNonQuery: %s\r\n", sql.v);
	ExecStatusType status = PQresultStatus(res);
	printf("PostgreSQL: ExecuteNonQuery status = %d (%s)\r\n", status, ExecStatusTypeGetName(status).v);
#endif
	if (status != PGRES_TUPLES_OK)
	{
		PQclear(res);
		return -2;
	}
	OSInt ret = -1;
	char *val = PQcmdTuples(res);
#if defined(VERBOSE)
	printf("PostgreSQL: ExecuteNonQuery row changed = %s\r\n", val);
#endif
	////////////////////////////
	PQclear(res);
	return ret;
}

DB::DBReader *DB::PostgreSQLConn::ExecuteReader(Text::CString sql)
{
	if (this->clsData->conn == 0)
	{
		return 0;
	}
	PGresult *res = PQexec(this->clsData->conn, (const char*)sql.v);
	ExecStatusType status = PQresultStatus(res);
#if defined(VERBOSE)
	printf("PostgreSQL: ExecuteReader: %s\r\n", sql.v);
	printf("PostgreSQL: ExecuteReader result = %d (%s)\r\n", status, ExecStatusTypeGetName(status).v);
#endif
	if (status != PGRES_TUPLES_OK)
	{
		PQclear(res);
		return 0;
	}
	return NEW_CLASS_D(PostgreSQLReader(res));
}

void DB::PostgreSQLConn::CloseReader(DB::DBReader *r)
{

}

void DB::PostgreSQLConn::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
	if (this->clsData->conn)
	{
		char *msg = PQerrorMessage(this->clsData->conn);
		str->AppendSlow((const UTF8Char*)msg);
	}
}

Bool DB::PostgreSQLConn::IsLastDataError()
{
	return false;
}

void DB::PostgreSQLConn::Reconnect()
{
	this->Close();
	this->Connect();
}

void *DB::PostgreSQLConn::BeginTransaction()
{
	if (this->isTran)
	{
		return 0;
	}
	this->ExecuteNonQuery(CSTR("BEGIN"));
	this->isTran = true;
	return (void*)-1;
}

void DB::PostgreSQLConn::Commit(void *tran)
{
	if (this->isTran)
	{
		this->ExecuteNonQuery(CSTR("COMMIT"));
		this->isTran = false;
	}
}

void DB::PostgreSQLConn::Rollback(void *tran)
{
	if (this->isTran)
	{
		this->ExecuteNonQuery(CSTR("ROLLBACK"));
		this->isTran = false;
	}
}

UOSInt DB::PostgreSQLConn::GetTableNames(Data::ArrayList<Text::CString> *names)
{
	if (this->tableNames == 0)
	{
		NEW_CLASS(this->tableNames, Data::ArrayList<Text::String*>());
		DB::DBReader *r = this->ExecuteReader(CSTR("select tablename from pg_catalog.pg_tables where schemaname = 'public'"));
		if (r)
		{
			while (r->ReadNext())
			{
				this->tableNames->Add(r->GetNewStr(0));
			}
			this->CloseReader(r);
		}
	}
	UOSInt i = 0;
	UOSInt j = this->tableNames->GetCount();
	while (i < j)
	{
		names->Add(this->tableNames->GetItem(i)->ToCString());
		i++;
	}
	return j;
}

DB::DBReader *DB::PostgreSQLConn::QueryTableData(Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	sb.AppendC(UTF8STRC("select "));
	if (columnNames == 0 || columnNames->GetCount() == 0)
	{
		sb.AppendC(UTF8STRC("*"));
	}
	else
	{
		i = 0;
		j = columnNames->GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sb.AppendUTF8Char(',');
			}
			sptr = DB::DBUtil::SDBColUTF8(sbuff, columnNames->GetItem(i)->v, DB::DBUtil::ServerType::PostgreSQL);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			i++;
		}
	}
	sb.AppendC(UTF8STRC(" from "));
	i = 0;
	while (true)
	{
		j = Text::StrIndexOfChar(&tableName.v[i], '.');
		if (j == INVALID_INDEX)
		{
			sptr = DB::DBUtil::SDBColUTF8(sbuff, &tableName.v[i], DB::DBUtil::ServerType::PostgreSQL);
			sb.AppendP(sbuff, sptr);
			break;
		}
		sptr = Text::StrConcatC(sbuff, &tableName.v[i], (UOSInt)j);
		sptr2 = DB::DBUtil::SDBColUTF8(sptr + 1, sbuff, DB::DBUtil::ServerType::PostgreSQL);
		sb.AppendP(sptr + 1, sptr2);
		sb.AppendUTF8Char('.');
		i += j + 1;
	}
	if (condition)
	{
		sb.AppendC(UTF8STRC(" where "));
		condition->ToWhereClause(&sb, DB::DBUtil::ServerType::PostgreSQL, 0, 100, 0);
	}
	if (ordering.leng > 0)
	{
		sb.AppendC(UTF8STRC(" order by "));
		sb.Append(ordering);
	}
	if (maxCnt > 0)
	{
		sb.AppendC(UTF8STRC(" LIMIT "));
		sb.AppendUOSInt(maxCnt);
	}
	if (ofst > 0)
	{
		sb.AppendC(UTF8STRC(" OFFSET "));
		sb.AppendUOSInt(ofst);
	}
	return this->ExecuteReader(sb.ToCString());
}

Bool DB::PostgreSQLConn::IsConnError()
{
	if (this->clsData->conn == 0)
		return true;
	return PQstatus(this->clsData->conn) != CONNECTION_OK;
}

Text::String *DB::PostgreSQLConn::GetConnServer()
{
	return this->server;
}

UInt16 DB::PostgreSQLConn::GetConnPort()
{
	return this->port;
}

Text::String *DB::PostgreSQLConn::GetConnDB()
{
	return this->database;
}

Text::String *DB::PostgreSQLConn::GetConnUID()
{
	return this->uid;
}

Text::String *DB::PostgreSQLConn::GetConnPWD()
{
	return this->pwd;
}

Text::CString DB::PostgreSQLConn::ExecStatusTypeGetName(OSInt status)
{
	switch (status)
	{
	case PGRES_EMPTY_QUERY:
		return CSTR("PGRES_EMPTY_QUERY");
	case PGRES_COMMAND_OK:
		return CSTR("PGRES_COMMAND_OK");
	case PGRES_TUPLES_OK:
		return CSTR("PGRES_TUPLES_OK");
	case PGRES_COPY_OUT:
		return CSTR("PGRES_COPY_OUT");
	case PGRES_COPY_IN:
		return CSTR("PGRES_COPY_IN");
	case PGRES_BAD_RESPONSE:
		return CSTR("PGRES_BAD_RESPONSE");
	case PGRES_NONFATAL_ERROR:
		return CSTR("PGRES_NONFATAL_ERROR");
	case PGRES_FATAL_ERROR:
		return CSTR("PGRES_FATAL_ERROR");
	case PGRES_COPY_BOTH:
		return CSTR("PGRES_COPY_BOTH");
	case PGRES_SINGLE_TUPLE:
		return CSTR("PGRES_SINGLE_TUPLE");
#if defined(LIBPQ_HAS_PIPELINING) && LIBPQ_HAS_PIPELINING
	case PGRES_PIPELINE_SYNC:
		return CSTR("PGRES_PIPELINE_SYNC");
	case PGRES_PIPELINE_ABORTED:
		return CSTR("PGRES_PIPELINE_ABORTED");
#endif
	default:
		return CSTR("Unknown");
	}
}

DB::DBUtil::ColType DB::PostgreSQLConn::DBType2ColType(UInt32 dbType)
{
	switch (dbType)
	{
	case 16: //bool
		return DB::DBUtil::CT_Bool;
	case 17: //binary
		return DB::DBUtil::CT_Binary;
	case 19: //name
		return DB::DBUtil::CT_VarChar;
	case 20: //int8
		return DB::DBUtil::CT_Int64;
	case 21: //int2
		return DB::DBUtil::CT_Int16;
	case 23: //int4
		return DB::DBUtil::CT_Int32;
	case 25: //text
		return DB::DBUtil::CT_VarChar;
	case 600: //point
		return DB::DBUtil::CT_Vector;
	case 701: //float8
		return DB::DBUtil::CT_Double;
	case 1005: //_int2
		return DB::DBUtil::CT_Binary;
	case 1016: //_int8
		return DB::DBUtil::CT_Binary;
	case 1042: //bpchar
		return DB::DBUtil::CT_Char;
	case 1043: //varchar
		return DB::DBUtil::CT_VarChar;
	case 1184: //timestamptz
		return DB::DBUtil::CT_DateTime2;
	case 1700: //numeric
		return DB::DBUtil::CT_Double;
	case 2950: //uuid
		return DB::DBUtil::CT_UUID;
	case 3802: //jsonb ////////////////////////////////
		return DB::DBUtil::CT_VarChar;
	case 16468: //hstore ////////////////////////////////
		return DB::DBUtil::CT_VarChar;
	default:
#if defined(VERBOSE)
		printf("PostgreSQL: Unknown type %d\r\n", dbType);
#endif
		return DB::DBUtil::CT_Unknown;
	}
}
