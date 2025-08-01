#include "Stdafx.h"
#include "DB/PostgreSQLConn.h"
#include "Math/Geometry/Point.h"
#include "Text/MyStringW.h"

Bool DB::PostgreSQLConn::Connect()
{
	return false;
}

DB::PostgreSQLConn::PostgreSQLConn(NN<Text::String> server, UInt16 port, Optional<Text::String> uid, Optional<Text::String> pwd, NN<Text::String> database, NN<IO::LogTool> log) : DBConn(server)
{
	this->clsData = 0;
	this->tzQhr = 0;
	this->log = log;
	this->server = server->Clone();
	this->port = port;
	this->database = database->Clone();
	this->uid = Text::String::CopyOrNull(uid);
	this->pwd = Text::String::CopyOrNull(pwd);
	this->geometryOid = 0;
	this->stgeometryOid = 0;
	this->citextOid = 0;
	this->Connect();
}

DB::PostgreSQLConn::PostgreSQLConn(Text::CStringNN server, UInt16 port, Text::CString uid, Text::CString pwd, Text::CStringNN database, NN<IO::LogTool> log) : DBConn(server)
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
	this->stgeometryOid = 0;
	this->citextOid = 0;
	this->Connect();
}

DB::PostgreSQLConn::~PostgreSQLConn()
{
	this->Close();
	this->server->Release();
	this->database->Release();
	OPTSTR_DEL(this->uid);
	OPTSTR_DEL(this->pwd);
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

void DB::PostgreSQLConn::GetConnName(NN<Text::StringBuilderUTF8> sb)
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

OSInt DB::PostgreSQLConn::ExecuteNonQuery(Text::CStringNN sql)
{
	return -2;
}

Optional<DB::DBReader> DB::PostgreSQLConn::ExecuteReader(Text::CStringNN sql)
{
	return 0;
}

void DB::PostgreSQLConn::CloseReader(NN<DB::DBReader> r)
{

}

void DB::PostgreSQLConn::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
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

Optional<DB::DBTransaction> DB::PostgreSQLConn::BeginTransaction()
{
	if (this->isTran)
	{
		return 0;
	}
	this->ExecuteNonQuery(CSTR("BEGIN"));
	this->isTran = true;
	return (DB::DBTransaction*)-1;
}

void DB::PostgreSQLConn::Commit(NN<DB::DBTransaction> tran)
{
	if (this->isTran)
	{
		this->ExecuteNonQuery(CSTR("COMMIT"));
		this->isTran = false;
	}
}

void DB::PostgreSQLConn::Rollback(NN<DB::DBTransaction> tran)
{
	if (this->isTran)
	{
		this->ExecuteNonQuery(CSTR("ROLLBACK"));
		this->isTran = false;
	}
}

UOSInt DB::PostgreSQLConn::QuerySchemaNames(NN<Data::ArrayListStringNN> names)
{
	UOSInt initCnt = names->GetCount();
	NN<DB::DBReader> r;
	if (this->ExecuteReader(CSTR("SELECT nspname FROM pg_catalog.pg_namespace")).SetTo(r))
	{
		while (r->ReadNext())
		{
			names->Add(r->GetNewStrNN(0));
		}
		this->CloseReader(r);
	}
	return names->GetCount() - initCnt;
}

UOSInt DB::PostgreSQLConn::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng == 0)
		schemaName = CSTR("public");
	DB::SQLBuilder sql(DB::SQLType::PostgreSQL, false, this->tzQhr);
	sql.AppendCmdC(CSTR("select tablename from pg_catalog.pg_tables where schemaname = "));
	sql.AppendStrC(schemaName);
	UOSInt initCnt = names->GetCount();
	NN<DB::DBReader> r;
	if (this->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			NN<Text::String> tabName;
			if (r->GetNewStr(0).SetTo(tabName))
				names->Add(tabName);
		}
		this->CloseReader(r);
	}
	return names->GetCount() - initCnt;
}

Optional<DB::DBReader> DB::PostgreSQLConn::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	Text::StringBuilderUTF8 sb;
	NN<Data::ArrayListStringNN> nncolumnNames;
	NN<Data::QueryConditions> nncondition;
	UOSInt i;
	UOSInt j;
	sb.AppendC(UTF8STRC("select "));
	if (!columnNames.SetTo(nncolumnNames) || nncolumnNames->GetCount() == 0)
	{
		sb.AppendC(UTF8STRC("*"));
	}
	else
	{
		Data::ArrayIterator<NN<Text::String>> it = nncolumnNames->Iterator();
		Bool found = false;
		while (it.HasNext())
		{
			if (found)
				sb.AppendUTF8Char(',');
			sptr = DB::DBUtil::SDBColUTF8(sbuff, it.Next()->v, DB::SQLType::PostgreSQL);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			found = true;
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
	if (condition.SetTo(nncondition))
	{
		sb.AppendC(UTF8STRC(" where "));
		Data::ArrayListNN<Data::Conditions::BooleanObject> cliCond;
		nncondition->ToWhereClause(sb, DB::SQLType::PostgreSQL, 0, 100, cliCond);
	}
	Text::CStringNN nnordering;
	if (ordering.SetTo(nnordering) && nnordering.leng > 0)
	{
		sb.AppendC(UTF8STRC(" order by "));
		sb.Append(nnordering);
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

NN<Text::String> DB::PostgreSQLConn::GetConnServer() const
{
	return this->server;
}

UInt16 DB::PostgreSQLConn::GetConnPort() const
{
	return this->port;
}

NN<Text::String> DB::PostgreSQLConn::GetConnDB() const
{
	return this->database;
}

Optional<Text::String> DB::PostgreSQLConn::GetConnUID() const
{
	return this->uid;
}

Optional<Text::String> DB::PostgreSQLConn::GetConnPWD() const
{
	return this->pwd;
}

Bool DB::PostgreSQLConn::ChangeDatabase(Text::CStringNN databaseName)
{
	return false;
}

Text::CString DB::PostgreSQLConn::ExecStatusTypeGetName(OSInt status)
{
	return CSTR("Unknown");
}

UInt32 DB::PostgreSQLConn::GetGeometryOid() const
{
	return this->geometryOid;
}

UInt32 DB::PostgreSQLConn::GetSTGeometryOid() const
{
	return this->stgeometryOid;
}

UInt32 DB::PostgreSQLConn::GetCitextOid() const
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

Optional<DB::DBTool> DB::PostgreSQLConn::CreateDBTool(NN<Text::String> serverName, UInt16 port, NN<Text::String> dbName, Optional<Text::String> uid, Optional<Text::String> pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<DB::PostgreSQLConn> conn;
	NEW_CLASSNN(conn, DB::PostgreSQLConn(serverName, port, uid, pwd, dbName, log));
	if (conn->IsConnError())
	{
		conn.Delete();
		return 0;
	}
	NN<DB::DBTool> db;
	NEW_CLASSNN(db, DB::DBTool(conn, true, log, logPrefix));
	return db;
}

Optional<DB::DBTool> DB::PostgreSQLConn::CreateDBTool(Text::CStringNN serverName, UInt16 port, Text::CStringNN dbName, Text::CString uid, Text::CString pwd, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<DB::PostgreSQLConn> conn;
	NEW_CLASSNN(conn, DB::PostgreSQLConn(serverName, port, uid, pwd, dbName, log));
	if (conn->IsConnError())
	{
		conn.Delete();
		return 0;
	}
	NN<DB::DBTool> db;
	NEW_CLASSNN(db, DB::DBTool(conn, true, log, logPrefix));
	return db;
}
