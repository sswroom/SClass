#include "Stdafx.h"
#include "DB/PostgreSQLConn.h"
#include "Math/Geometry/Point.h"
#include "Text/MyStringW.h"

Bool DB::PostgreSQLConn::Connect()
{
	return false;
}

DB::PostgreSQLConn::PostgreSQLConn(NotNullPtr<Text::String> server, UInt16 port, Text::String *uid, Text::String *pwd, NotNullPtr<Text::String> database, IO::LogTool *log) : DBConn(server)
{
	this->clsData = 0;
	this->tzQhr = 0;
	this->log = log;
	this->server = server->Clone();
	this->port = port;
	this->database = database->Clone();
	this->uid = SCOPY_STRING(uid);
	this->pwd = SCOPY_STRING(pwd);
	this->geometryOid = 0;
	this->citextOid = 0;
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
	this->geometryOid = 0;
	this->citextOid = 0;
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

DB::SQLType DB::PostgreSQLConn::GetSQLType() const
{
	return DB::SQLType::PostgreSQL;
}

DB::DBConn::ConnType DB::PostgreSQLConn::GetConnType() const
{
	return DB::DBConn::CT_POSTGRESQL;
}

Int8 DB::PostgreSQLConn::GetTzQhr() const
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

void DB::PostgreSQLConn::GetLastErrorMsg(Text::StringBuilderUTF8 *str)
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

UOSInt DB::PostgreSQLConn::QuerySchemaNames(Data::ArrayList<Text::String*> *names)
{
	UOSInt initCnt = names->GetCount();
	DB::DBReader *r = this->ExecuteReader(CSTR("SELECT nspname FROM pg_catalog.pg_namespace"));
	if (r)
	{
		while (r->ReadNext())
		{
			names->Add(r->GetNewStr(0));
		}
		this->CloseReader(r);
	}
	return names->GetCount() - initCnt;
}

UOSInt DB::PostgreSQLConn::QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names)
{
	if (schemaName.leng == 0)
		schemaName = CSTR("public");
	DB::SQLBuilder sql(DB::SQLType::PostgreSQL, false, this->tzQhr);
	sql.AppendCmdC(CSTR("select tablename from pg_catalog.pg_tables where schemaname = "));
	sql.AppendStrC(schemaName);
	UOSInt initCnt = names->GetCount();
	DB::DBReader *r = this->ExecuteReader(sql.ToCString());
	if (r)
	{
		while (r->ReadNext())
		{
			NotNullPtr<Text::String> tabName;
			if (tabName.Set(r->GetNewStr(0)))
				names->Add(tabName);
		}
		this->CloseReader(r);
	}
	return names->GetCount() - initCnt;
}

DB::DBReader *DB::PostgreSQLConn::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
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
			sptr = DB::DBUtil::SDBColUTF8(sbuff, columnNames->GetItem(i)->v, DB::SQLType::PostgreSQL);
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
			sptr = DB::DBUtil::SDBColUTF8(sbuff, &tableName.v[i], DB::SQLType::PostgreSQL);
			sb.AppendP(sbuff, sptr);
			break;
		}
		sptr = Text::StrConcatC(sbuff, &tableName.v[i], (UOSInt)j);
		sptr2 = DB::DBUtil::SDBColUTF8(sptr + 1, sbuff, DB::SQLType::PostgreSQL);
		sb.AppendP(sptr + 1, sptr2);
		sb.AppendUTF8Char('.');
		i += j + 1;
	}
	if (condition)
	{
		sb.AppendC(UTF8STRC(" where "));
		condition->ToWhereClause(&sb, DB::SQLType::PostgreSQL, 0, 100, 0);
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

NotNullPtr<Text::String> DB::PostgreSQLConn::GetConnServer() const
{
	return this->server;
}

UInt16 DB::PostgreSQLConn::GetConnPort() const
{
	return this->port;
}

NotNullPtr<Text::String> DB::PostgreSQLConn::GetConnDB() const
{
	return this->database;
}

Text::String *DB::PostgreSQLConn::GetConnUID() const
{
	return this->uid;
}

Text::String *DB::PostgreSQLConn::GetConnPWD() const
{
	return this->pwd;
}

Bool DB::PostgreSQLConn::ChangeDatabase(Text::CString databaseName)
{
	return false;
}

Text::CString DB::PostgreSQLConn::ExecStatusTypeGetName(OSInt status)
{
	return CSTR("Unknown");
}

UInt32 DB::PostgreSQLConn::GetGeometryOid()
{
	return this->geometryOid;
}

UInt32 DB::PostgreSQLConn::GetCitextOid()
{
	return this->citextOid;
}

DB::DBUtil::ColType DB::PostgreSQLConn::DBType2ColType(UInt32 dbType)
{
	switch (dbType)
	{
	case 16: //bool
		return DB::DBUtil::CT_Bool;
	case 17: //bytea
		return DB::DBUtil::CT_Binary;
	case 18: //char
		return DB::DBUtil::CT_UTF32Char;
	case 19: //name
		return DB::DBUtil::CT_VarUTF32Char;
	case 20: //int8
		return DB::DBUtil::CT_Int64;
	case 21: //int2
		return DB::DBUtil::CT_Int16;
	case 22: //int2vector
		return DB::DBUtil::CT_Binary;
	case 23: //int4
		return DB::DBUtil::CT_Int32;
	case 24: //regproc
		return DB::DBUtil::CT_VarUTF32Char;
	case 25: //text
		return DB::DBUtil::CT_VarUTF32Char;
	case 26: //oid
		return DB::DBUtil::CT_Int32;
	case 28: //xid
		return DB::DBUtil::CT_Int32;
	case 30: //oidvector
		return DB::DBUtil::CT_Binary;
	case 194: //pg_node_tree
		return DB::DBUtil::CT_VarUTF32Char;
	case 600: //point
		return DB::DBUtil::CT_Vector;
	case 700: //float4
		return DB::DBUtil::CT_Float;
	case 701: //float8
		return DB::DBUtil::CT_Double;
	case 1002: //_char
		return DB::DBUtil::CT_VarUTF32Char;
	case 1005: //_int2
		return DB::DBUtil::CT_Binary;
	case 1009: //_text
		return DB::DBUtil::CT_VarUTF32Char;
	case 1016: //_int8
		return DB::DBUtil::CT_Binary;
	case 1021: //_float4
		return DB::DBUtil::CT_Binary;
	case 1028: //_oid
		return DB::DBUtil::CT_Binary;
	case 1034: //_aclitem
		return DB::DBUtil::CT_VarUTF32Char;
	case 1042: //bpchar
		return DB::DBUtil::CT_UTF32Char;
	case 1043: //varchar
		return DB::DBUtil::CT_VarUTF32Char;
	case 1082: //date
		return DB::DBUtil::CT_Date;
	case 1114: //timestamp
		return DB::DBUtil::CT_DateTime;
	case 1184: //timestamptz
		return DB::DBUtil::CT_DateTime;
	case 1700: //numeric
		return DB::DBUtil::CT_Double;
	case 2277: //anyarray
		return DB::DBUtil::CT_VarUTF32Char;
	case 2950: //uuid
		return DB::DBUtil::CT_UUID;
	case 3220: //pg_lsn  ////////////////////////////////
		return DB::DBUtil::CT_VarUTF32Char;
	case 3361: //pg_ndistinct  ////////////////////////////////
		return DB::DBUtil::CT_VarUTF32Char;
	case 3402: //pg_dependencies  ////////////////////////////////
		return DB::DBUtil::CT_VarUTF32Char;
	case 3802: //jsonb ////////////////////////////////
		return DB::DBUtil::CT_VarUTF32Char;
	case 5017: //pg_mcv_list  ////////////////////////////////
		return DB::DBUtil::CT_VarUTF32Char;
	case 12028: //_pg_statistic  ////////////////////////////////
		return DB::DBUtil::CT_VarUTF32Char;
	case 16468: //hstore ////////////////////////////////
		return DB::DBUtil::CT_VarUTF32Char;
	case 34012: //citext
		return DB::DBUtil::CT_VarUTF32Char;
	case 34122: //geometry
		return DB::DBUtil::CT_Vector;
	default:
#if defined(VERBOSE)
		printf("PostgreSQL: Unknown type %d\r\n", dbType);
#endif
		return DB::DBUtil::CT_Unknown;
	}
}
