#include "Stdafx.h"
#include "DB/PostgreSQLConn.h"
#include "Math/Point.h"
#include "Text/MyStringW.h"

void DB::PostgreSQLConn::Connect()
{
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
	this->clsData = 0;
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
	this->clsData = 0;
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
}

void DB::PostgreSQLConn::Dispose()
{
	DEL_CLASS(this);
}

OSInt DB::PostgreSQLConn::ExecuteNonQuery(Text::CString sql)
{
	return -2;
}

DB::DBReader *DB::PostgreSQLConn::ExecuteReader(Text::CString sql)
{
	return 0;
}

void DB::PostgreSQLConn::CloseReader(DB::DBReader *r)
{

}

void DB::PostgreSQLConn::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
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
	return true;
}

Text::String *DB::PostgreSQLConn::GetConnServer()
{
	return this->server;
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
	return CSTR("Unknown");
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