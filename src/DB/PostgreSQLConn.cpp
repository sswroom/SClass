#include "Stdafx.h"
#include "DB/PostgreSQLConn.h"
#include "Math/WKBReader.h"
#include "Math/Geometry/Point.h"
#include "Text/MyStringW.h"
#include <libpq-fe.h>

//#define VERBOSE
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
	Int8 tzQhr;
	PGresult *res;
	int ncol;
	int nrow;
	int currrow;
	DB::PostgreSQLConn *conn;
	UInt32 geometryOid;
	UInt32 citextOid;
public:
	PostgreSQLReader(PGresult *res, Int8 tzQhr, DB::PostgreSQLConn *conn) : DBReader()
	{
		this->tzQhr = tzQhr;
		this->res = res;
		this->currrow = -1;
		this->conn = conn;
		this->geometryOid = conn->GetGeometryOid();
		this->citextOid = conn->GetCitextOid();
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
		char *rowChg = PQcmdTuples(this->res);
#if defined(VERBOSE)
		printf("PostgreSQL: Row changed = %s\r\n", rowChg);
#endif
		if (rowChg && rowChg[0] != 0)
		{
			return Text::StrToOSInt(rowChg);
		}
		////////////////////////////
		return 0;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
		return item.GetAsI32();
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
		return item.GetAsI64();
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
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
		item.GetAsString(sb);
		return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
	}

	virtual Bool GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
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

	virtual Optional<Text::String> GetNewStr(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
		return item.GetAsNewString();
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
		return item.GetAsStringS(buff, buffSize);
	}

	virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, item))
		{
			return Data::Timestamp(0);
		}
		if (item.GetItemType() == Data::VariItem::ItemType::Null)
		{
			return Data::Timestamp(0);
		}
		return item.GetAsTimestamp();
	}

	virtual Data::Date GetDate(UOSInt colIndex)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, item))
		{
			return Data::Date(nullptr);
		}
		if (item.GetItemType() == Data::VariItem::ItemType::Null)
		{
			return Data::Date(nullptr);
		}
		return item.GetAsDate();
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
		return item.GetAsF64();
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
		return item.GetAsBool();
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
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
		this->GetVariItem(colIndex, item);
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

	virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
		return item.GetAndRemoveVector();
	}

	virtual Bool GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid)
	{
		Data::VariItem item;
		this->GetVariItem(colIndex, item);
		return item.GetAndRemoveUUID();
	}

	virtual Bool GetVariItem(UOSInt colIndex, NotNullPtr<Data::VariItem> item)
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
		if (colType == geometryOid)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendSlow((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex));
/*			if (sb.ToString()[0] != '(' || !sb.EndsWith(')'))
			{
				return false;
			}*/
			UInt8 *wkb = MemAlloc(UInt8, sb.GetLength() >> 1);
			UOSInt wkbLen = sb.Hex2Bytes(wkb);
			Math::WKBReader reader(0);
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (reader.ParseWKB(wkb, wkbLen, 0).SetTo(vec))
			{
				MemFree(wkb);
				item->SetVectorDirect(vec);
				return true;
			}
			else
			{
				MemFree(wkb);
				return false;
			}
		}
		else if (colType == citextOid)
		{
			item->SetStrSlow((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex));
			return true;
		}
		switch (colType)
		{
		case 16: //bool
			item->SetBool(PQgetvalue(this->res, this->currrow, (int)colIndex)[0] == 't');
			return true;
		case 19: //name
		case 24: //regproc
		case 25: //text
		case 194: //pg_node_tree
		case 869: //inet
		case 1009: //_text
		case 1034: //_aclitem
		case 1042: //bpchar
		case 1043: //varchar
		case 2277: //anyarray
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
		case 26: //oid
		case 28: //xid
			item->SetI32(Text::StrToInt32(PQgetvalue(this->res, this->currrow, (int)colIndex)));
			return true;
		case 18: //char
			item->SetU8((UInt8)(PQgetvalue(this->res, this->currrow, (int)colIndex)[0]));
			return true;
		case 700: //float4
			item->SetF32((Single)Text::StrToDouble(PQgetvalue(this->res, this->currrow, (int)colIndex)));
			return true;
		case 701: //float8
		case 1700: //numeric
			item->SetF64(Text::StrToDouble(PQgetvalue(this->res, this->currrow, (int)colIndex)));
			return true;
		case 1082: //date
			item->SetDate(Data::Date(Text::CStringNN::FromPtr((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex))));
			return true;
		case 1114: //timestamp
		case 1184: //timestamptz
			item->SetDate(Data::Timestamp(Text::CStringNN::FromPtr((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex)), this->tzQhr));
			return true;
		case 2950: //uuid
		{
			NotNullPtr<Data::UUID> uuid;
			NEW_CLASSNN(uuid, Data::UUID(Text::CStringNN::FromPtr((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex))));
			item->SetUUIDDirect(uuid);
			return true;
		}
		case 1002: //_char
		{
			Text::StringBuilderUTF8 arr;
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
				if (sarr[0].leng == 1)
				{
					arr.AppendUTF8Char(sarr[0].v[0]);
				}
				if (scnt != 2)
				{
					break;
				}
			}
			item->SetStr(arr.ToString(), arr.GetLength());
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
				if (sarr[0].ToInt16(v))
				{
					arr.Add(v);
				}
				if (scnt != 2)
				{
					break;
				}
			}
			Int16 *bytes = arr.GetPtr(scnt);
			item->SetByteArr((const UInt8*)bytes, scnt * sizeof(v));
			return true;
		}
		case 1028: //_oid
		{
			Data::ArrayList<Int32> arr;
			Int32 v;
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
				if (sarr[0].ToInt32(v))
				{
					arr.Add(v);
				}
				if (scnt != 2)
				{
					break;
				}
			}
			Int32 *bytes = arr.GetPtr(scnt);
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
				if (sarr[0].ToInt64(v))
				{
					arr.Add(v);
				}
				if (scnt != 2)
				{
					break;
				}
			}
			Int64 *bytes = arr.GetPtr(scnt);
			item->SetByteArr((const UInt8*)bytes, scnt * sizeof(v));
			return true;
		}
		case 1021: //_float4
		{
			Data::ArrayList<Single> arr;
			Double dv;
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
				if (sarr[0].ToDouble(dv))
				{
					arr.Add((Single)dv);
				}
				if (scnt != 2)
				{
					break;
				}
			}
			Single *bytes = arr.GetPtr(scnt);
			item->SetByteArr((const UInt8*)bytes, scnt * sizeof(Single));
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
			NotNullPtr<Math::Geometry::Point> pt;
			NEW_CLASSNN(pt, Math::Geometry::Point(0, sarr[0].ToDouble(), sarr[1].ToDouble()));
			item->SetVectorDirect(pt);
			return true;
		}
		case 22: //int2vector
		{
			Data::ArrayList<Int16> arr;
			Int16 v;
			Text::StringBuilderUTF8 sb;
			sb.AppendSlow((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex));
			Text::PString sarr[2];
			UOSInt scnt;
			sarr[1] = sb;
			while (true)
			{
				scnt = Text::StrSplitP(sarr, 2, sarr[1], ' ');
				if (sarr[0].ToInt16(v))
				{
					arr.Add(v);
				}
				if (scnt != 2)
				{
					break;
				}
			}
			Int16 *bytes = arr.GetPtr(scnt);
			item->SetByteArr((const UInt8*)bytes, scnt * sizeof(v));
			return true;
		}
		case 30: //oidvector
		{
			Data::ArrayList<Int32> arr;
			Int32 v;
			Text::StringBuilderUTF8 sb;
			sb.AppendSlow((const UTF8Char*)PQgetvalue(this->res, this->currrow, (int)colIndex));
			Text::PString sarr[2];
			UOSInt scnt;
			sarr[1] = sb;
			while (true)
			{
				scnt = Text::StrSplitP(sarr, 2, sarr[1], ' ');
				if (sarr[0].ToInt32(v))
				{
					arr.Add(v);
				}
				if (scnt != 2)
				{
					break;
				}
			}
			Int32 *bytes = arr.GetPtr(scnt);
			item->SetByteArr((const UInt8*)bytes, scnt * sizeof(v));
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
		if (!this->GetVariItem(colIndex, item))
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

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
	{
		Oid oid = PQftype(this->res, (int)colIndex);
		if (colSize.IsNotNull())
		{
			int len = PQfsize(this->res, (int)colIndex);
			if (len < 0)
			{
				colSize.SetNoCheck(65535);
			}
			else
			{
				colSize.SetNoCheck((UOSInt)len);
			}
		}
		return this->conn->DBType2ColType(oid);
	}

	virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
	{
		if (colIndex >= (UOSInt)this->ncol)
		{
			return false;
		}
		char *name = PQfname(this->res, (int)colIndex);
		NotNullPtr<const UTF8Char> colName;
		if (!colName.Set((const UTF8Char*)name))
		{
			return false;
		}
		colDef->SetColName(colName);
		colDef->SetColType(this->conn->DBType2ColType(PQftype(this->res, (int)colIndex)));
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

Bool DB::PostgreSQLConn::Connect()
{
	if (this->clsData->conn == 0)
	{
		NotNullPtr<Text::String> s;
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("host="));
		sb.Append(this->server);
		sb.AppendC(UTF8STRC(" port="));
		sb.AppendU16(this->port);
		sb.AppendC(UTF8STRC(" dbname="));
		sb.Append(this->database);
		if (this->uid.SetTo(s))
		{
			sb.AppendC(UTF8STRC(" user="));
			sb.Append(s);
		}
		if (this->pwd.SetTo(s))
		{
			sb.AppendC(UTF8STRC(" password="));
			sb.Append(s);
		}
		this->clsData->conn = PQconnectdb((const char*)sb.ToString());
		this->isTran = false;
		if (PQstatus(this->clsData->conn) != CONNECTION_OK)
		{
			if (this->log->HasHandler())
			{
				char *msg = PQerrorMessage(this->clsData->conn);
				UOSInt msgLen = Text::StrCharCnt(msg);
				while (msgLen > 0)
				{
					if (msg[msgLen - 1] != 13 && msg[msgLen - 1] != 10)
						break;
					msgLen--;
					msg[msgLen] = 0;
				}
				this->log->LogMessage(Text::CStringNN((const UTF8Char*)msg, msgLen), IO::LogHandler::LogLevel::Error);
				this->log->LogStackTrace(IO::LogHandler::LogLevel::ErrorDetail);
#if defined(VERBOSE)
				printf("PostgreSQL: Error, \"%s\"\r\n", msg);
#endif
			}
			return false;
		}
		else
		{
#if defined(VERBOSE)
			printf("PostgreSQL: DB Connected\r\n");
#endif
			return true;
		}
	}
	else
	{
		return true;
	}
}

void DB::PostgreSQLConn::InitConnection()
{
	NotNullPtr<DB::DBReader> r;
	if (this->ExecuteReader(CSTR("select oid, typname from pg_catalog.pg_type")).SetTo(r))
	{
		Text::StringBuilderUTF8 sb;
		while (r->ReadNext())
		{
			sb.ClearStr();
			r->GetStr(1, sb);
			if (sb.Equals(UTF8STRC("geometry")))
			{
				this->geometryOid = (UInt32)r->GetInt32(0);
			}
			else if (sb.Equals(UTF8STRC("citext")))
			{
				this->citextOid = (UInt32)r->GetInt32(0);
			}
		}
		this->CloseReader(r);
	}
	if (this->ExecuteReader(CSTR("select now()")).SetTo(r))
	{
		if (r->ReadNext())
		{
			this->tzQhr = r->GetTimestamp(0).GetTimeZoneQHR();
		}
		this->CloseReader(r);
	}
}

DB::PostgreSQLConn::PostgreSQLConn(NotNullPtr<Text::String> server, UInt16 port, Text::String *uid, Text::String *pwd, NotNullPtr<Text::String> database, NotNullPtr<IO::LogTool> log) : DBConn(server)
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
	this->geometryOid = 0;
	this->citextOid = 0;
	if (this->Connect()) this->InitConnection();
}

DB::PostgreSQLConn::PostgreSQLConn(Text::CStringNN server, UInt16 port, Text::CString uid, Text::CString pwd, Text::CString database, NotNullPtr<IO::LogTool> log) : DBConn(server)
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
	this->geometryOid = 0;
	this->citextOid = 0;
	if (this->Connect()) this->InitConnection();
}

DB::PostgreSQLConn::~PostgreSQLConn()
{
	this->Close();
	this->server->Release();
	this->database->Release();
	OPTSTR_DEL(this->uid);
	OPTSTR_DEL(this->pwd);
	MemFree(this->clsData);
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

void DB::PostgreSQLConn::GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("PostgreSQL"));
	sb->AppendC(UTF8STRC(" - "));
	sb->Append(this->database);
}

void DB::PostgreSQLConn::Close()
{
	if (this->clsData->conn)
	{
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

OSInt DB::PostgreSQLConn::ExecuteNonQuery(Text::CStringNN sql)
{
	if (this->clsData->conn == 0)
	{
		return -2;
	}
	PGresult *res = PQexec(this->clsData->conn, (const char*)sql.v);
	ExecStatusType status = PQresultStatus(res);
#if defined(VERBOSE)
	printf("PostgreSQL: ExecuteNonQuery: %s\r\n", sql.v);
	printf("PostgreSQL: ExecuteNonQuery status = %d (%s)\r\n", status, ExecStatusTypeGetName(status).v);
#endif
	if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
	{
		PQclear(res);
		return -2;
	}
	OSInt ret = -1;
	char *val = PQcmdTuples(res);
#if defined(VERBOSE)
	printf("PostgreSQL: ExecuteNonQuery row changed = %s\r\n", val);
#endif
	if (val)
	{
		Text::StrToOSInt(val, ret);
	}
	PQclear(res);
	return ret;
}

Optional<DB::DBReader> DB::PostgreSQLConn::ExecuteReader(Text::CStringNN sql)
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
	if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK)
	{
#if defined(VERBOSE)
		printf("PostgreSQL: Error: %s\r\n", PQerrorMessage(this->clsData->conn));
#endif
		PQclear(res);
		return 0;
	}
	return NEW_CLASS_D(PostgreSQLReader(res, this->tzQhr, this));
}

void DB::PostgreSQLConn::CloseReader(NotNullPtr<DB::DBReader> r)
{
	PostgreSQLReader *reader = (PostgreSQLReader*)r.Ptr();
	DEL_CLASS(reader);
}

void DB::PostgreSQLConn::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
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
	if (this->Connect())
	{
		this->InitConnection();
	}
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

void DB::PostgreSQLConn::Commit(NotNullPtr<DB::DBTransaction> tran)
{
	if (this->isTran)
	{
		this->ExecuteNonQuery(CSTR("COMMIT"));
		this->isTran = false;
	}
}

void DB::PostgreSQLConn::Rollback(NotNullPtr<DB::DBTransaction> tran)
{
	if (this->isTran)
	{
		this->ExecuteNonQuery(CSTR("ROLLBACK"));
		this->isTran = false;
	}
}

UOSInt DB::PostgreSQLConn::QuerySchemaNames(NotNullPtr<Data::ArrayListStringNN> names)
{
	UOSInt initCnt = names->GetCount();
	NotNullPtr<DB::DBReader> r;
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
		
UOSInt DB::PostgreSQLConn::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names)
{
	if (schemaName.leng == 0)
		schemaName = CSTR("public");
	DB::SQLBuilder sql(DB::SQLType::PostgreSQL, false, this->tzQhr);
	sql.AppendCmdC(CSTR("select tablename from pg_catalog.pg_tables where schemaname = "));
	sql.AppendStrC(schemaName);
	UOSInt initCnt = names->GetCount();
	NotNullPtr<DB::DBReader> r;
	if (this->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		while (r->ReadNext())
		{
			NotNullPtr<Text::String> tabName;
			if (r->GetNewStr(0).SetTo(tabName))
				names->Add(tabName);
		}
		this->CloseReader(r);
	}
	return names->GetCount() - initCnt;
}

Optional<DB::DBReader> DB::PostgreSQLConn::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("select "));
	if (columnNames == 0 || columnNames->GetCount() == 0)
	{
		sb.AppendC(UTF8STRC("*"));
	}
	else
	{
		Data::ArrayIterator<NotNullPtr<Text::String>> it = columnNames->Iterator();
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
	if (schemaName.leng > 0)
	{
		sptr = DB::DBUtil::SDBColUTF8(sbuff, schemaName.v, DB::SQLType::PostgreSQL);
		sb.AppendP(sbuff, sptr);
		sb.AppendUTF8Char('.');
	}
		sptr = DB::DBUtil::SDBColUTF8(sbuff, tableName.v, DB::SQLType::PostgreSQL);
		sb.AppendP(sbuff, sptr);
	if (condition)
	{
		Data::ArrayListNN<Data::QueryConditions::Condition> cliCond;
		sb.AppendC(UTF8STRC(" where "));
		condition->ToWhereClause(sb, DB::SQLType::PostgreSQL, 0, 100, cliCond);
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

Optional<Text::String> DB::PostgreSQLConn::GetConnUID() const
{
	return this->uid;
}

Optional<Text::String> DB::PostgreSQLConn::GetConnPWD() const
{
	return this->pwd;
}

Bool DB::PostgreSQLConn::ChangeDatabase(Text::CString databaseName)
{
	NotNullPtr<Text::String> oldDB = this->database;
	this->database = Text::String::New(databaseName);
	this->Reconnect();
	if (this->clsData->conn)
	{
		oldDB->Release();
		return true;
	}
	else
	{
		this->database->Release();
		this->database = oldDB;
		this->Reconnect();
		return false;
	}
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
	if (dbType == this->geometryOid)
		return DB::DBUtil::CT_Vector;
	if (dbType == this->citextOid)
		return DB::DBUtil::CT_VarUTF32Char;
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
	case 869: //inet
		return DB::DBUtil::CT_VarUTF32Char;
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
	default:
#if defined(VERBOSE)
		printf("PostgreSQL: Unknown type %d\r\n", dbType);
#endif
		return DB::DBUtil::CT_Unknown;
	}
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

Optional<DB::DBTool> DB::PostgreSQLConn::CreateDBTool(NotNullPtr<Text::String> serverName, UInt16 port, NotNullPtr<Text::String> dbName, Text::String *uid, Text::String *pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix)
{
	NotNullPtr<DB::PostgreSQLConn> conn;
	NEW_CLASSNN(conn, DB::PostgreSQLConn(serverName, port, uid, pwd, dbName, log));
	if (conn->IsConnError())
	{
		conn.Delete();
		return 0;
	}
	NotNullPtr<DB::DBTool> db;
	NEW_CLASSNN(db, DB::DBTool(conn, true, log, logPrefix));
	return db;
}

Optional<DB::DBTool> DB::PostgreSQLConn::CreateDBTool(Text::CStringNN serverName, UInt16 port, Text::CString dbName, Text::CString uid, Text::CString pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix)
{
	NotNullPtr<DB::PostgreSQLConn> conn;
	NEW_CLASSNN(conn, DB::PostgreSQLConn(serverName, port, uid, pwd, dbName, log));
	if (conn->IsConnError())
	{
		conn.Delete();
		return 0;
	}
	NotNullPtr<DB::DBTool> db;
	NEW_CLASSNN(db, DB::DBTool(conn, true, log, logPrefix));
	return db;
}
