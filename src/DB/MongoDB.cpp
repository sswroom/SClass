#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "DB/MongoDB.h"
#include "DB/TableDef.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextBinEnc/URIEncoding.h"

#if defined(__FreeBSD__) || defined(CPU_X86_32) || (defined(__GNUC__) && __GNUC__ <= 6)
#include <bson.h>
#include <mongoc.h>
#if !MONGOC_CHECK_VERSION(1, 9, 0)
#define mongoc_database_get_collection_names_with_opts(a, b, c) mongoc_database_get_collection_names(a, c)
#define mongoc_client_get_database_names_with_opts(a, b, c) mongoc_client_get_database_names(a, c)
#endif
#if !MONGOC_CHECK_VERSION(1, 5, 0)
#define mongoc_collection_find_with_opts(a, b, c, d) mongoc_collection_find(a, MONGOC_QUERY_NONE, 0, 0, 0, b, 0, d)
#endif
#if !BSON_CHECK_VERSION(1, 7, 0)
#define bson_as_canonical_extended_json(a, b) bson_as_json(a, b)
#endif
#else
#include <bson/bson.h>
#include <mongoc/mongoc.h>
#endif

Int32 DB::MongoDB::initCnt = 0;

DB::MongoDB::MongoDB(Text::CStringNN url, Text::CString database, IO::LogTool *log) : DB::ReadingDB(url)
{
	mongoc_client_t *client;
	this->log = log;
	this->errorMsg = 0;
	if (Sync::Interlocked::IncrementI32(initCnt) == 1)
	{
		mongoc_init();
	}
	client = mongoc_client_new((const Char*)url.v.Ptr());
	this->client = client;
	this->database = Text::String::NewOrNull(database).OrNull();
}

DB::MongoDB::~MongoDB()
{
	if (this->client)
	{
		mongoc_client_destroy((mongoc_client_t*)this->client);
	}
	SDEL_STRING(this->database);
	if (Sync::Interlocked::DecrementI32(initCnt) == 0)
	{
		mongoc_cleanup();
	}
}

UOSInt DB::MongoDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (this->database == 0 || this->client == 0 || schemaName.leng != 0)
		return 0;

	UOSInt initCnt = names->GetCount();
	bson_error_t error;
	mongoc_database_t *db = mongoc_client_get_database((mongoc_client_t*)this->client, (const Char*)this->database->v.Ptr());
	char **strv;
	strv = mongoc_database_get_collection_names_with_opts(db, 0, &error);
	SDEL_STRING(this->errorMsg);
	if (strv == 0)
	{
		this->errorMsg = Text::String::NewNotNullSlow((const UTF8Char*)error.message).Ptr();
	}
	else
	{
		OSInt i = 0;
		while (strv[i])
		{
			names->Add(Text::String::NewNotNullSlow((const UTF8Char*)strv[i]));
			i++;
		}
		bson_strfreev(strv);
	}		
	mongoc_database_destroy(db);
	return names->GetCount() - initCnt;
}

Optional<DB::DBReader> DB::MongoDB::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	if (this->database && this->client)
	{
		mongoc_collection_t *coll = mongoc_client_get_collection((mongoc_client_t*)this->client, (const Char*)this->database->v.Ptr(), (const Char*)tableName.v.Ptr());
		if (coll)
		{
			NN<DB::MongoDBReader> reader;
			NEW_CLASSNN(reader, DB::MongoDBReader(this, coll));
			return reader;
		}
	}
	return 0;
}

Optional<DB::TableDef> DB::MongoDB::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DB::ColDef> colDef;
	DB::TableDef *tab;
	NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
	NEW_CLASSNN(colDef, DB::ColDef(CSTR("Data")));
	colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
	colDef->SetColSize(65536);
	colDef->SetColDP(0);
	colDef->SetDefVal(Text::CString(nullptr));
	colDef->SetAttr(Text::CString(nullptr));
	colDef->SetAutoIncNone();
	colDef->SetNotNull(true);
	colDef->SetPK(false);
	tab->AddCol(colDef);
	return tab;
}

void DB::MongoDB::CloseReader(NN<DBReader> r)
{
	DB::MongoDBReader *reader = (DB::MongoDBReader*)r.Ptr();
	DEL_CLASS(reader);
}

void DB::MongoDB::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	if (this->errorMsg)
	{
		str->Append(this->errorMsg);
	}
}

void DB::MongoDB::Reconnect()
{

}

UOSInt DB::MongoDB::GetDatabaseNames(NN<Data::ArrayListStringNN> names)
{
	bson_error_t error;
	SDEL_STRING(this->errorMsg);
	if (this->client == 0)
		return 0;
	char **strv = mongoc_client_get_database_names_with_opts((mongoc_client_t*)this->client, 0, &error);
	if (strv == 0)
	{
		this->errorMsg = Text::String::NewNotNullSlow((const UTF8Char*)error.message).Ptr();
		return 0;
	}
	else
	{
		UOSInt i = 0;
		while (strv[i])
		{
			names->Add(Text::String::NewNotNullSlow((const UTF8Char*)strv[i]));
			i++;
		}
		bson_strfreev(strv);
		return i;
	}
}

void DB::MongoDB::FreeDatabaseNames(NN<Data::ArrayListStringNN> names)
{
	UOSInt i = names->GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(names->GetItem(i));
	}
	names->Clear();
}

void DB::MongoDB::BuildURL(NN<Text::StringBuilderUTF8> out, Text::CString userName, Text::CString password, Text::CStringNN host, UInt16 port)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	out->AppendC(UTF8STRC("mongodb://"));
	Text::CStringNN nns;
	if (userName.SetTo(nns))
	{
		sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
		out->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		if (password.SetTo(nns))
		{
			out->AppendUTF8Char(':');
			sptr = Text::TextBinEnc::URIEncoding::URIEncode(sbuff, nns.v);
			out->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		}
		out->AppendUTF8Char('@');
	}
	out->Append(host);
	out->AppendUTF8Char(':');
	out->AppendU16(port);
}

DB::MongoDBReader::MongoDBReader(MongoDB *conn, void *coll)
{
	this->conn = conn;
	this->coll = coll;
	this->query = bson_new();
	this->cursor = mongoc_collection_find_with_opts((mongoc_collection_t*)this->coll, (bson_t*)this->query, 0, 0);
	this->doc = 0;
}

DB::MongoDBReader::~MongoDBReader()
{
	if (this->cursor)
	{
		mongoc_cursor_destroy((mongoc_cursor_t*)this->cursor);
	}
	if (this->query)
	{
		bson_destroy((bson_t*)this->query);
	}
	if (this->coll)
	{
		mongoc_collection_destroy((mongoc_collection_t*)this->coll);
	}
}


Bool DB::MongoDBReader::ReadNext()
{
	const bson_t *doc;
	if (mongoc_cursor_next((mongoc_cursor_t*)this->cursor, &doc))
	{
		this->doc = doc;
		return true;
	}
	this->doc = 0;
	return false;
}

Int32 DB::MongoDBReader::GetInt32(UOSInt colIndex)
{
	return 0;
}

Int64 DB::MongoDBReader::GetInt64(UOSInt colIndex)
{
	return 0;
}

UnsafeArrayOpt<WChar> DB::MongoDBReader::GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
{
	if (colIndex != 0)
		return 0;
	if (this->doc)
	{
		char *str = bson_as_canonical_extended_json((const bson_t*)this->doc, 0);
		buff = Text::StrUTF8_WChar(buff, (const UTF8Char*)str, 0);
		bson_free(str);
		return buff;
	}
	else
	{
		return 0;
	}
}

Bool DB::MongoDBReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	if (colIndex != 0)
		return false;
	if (this->doc)
	{
		char *str = bson_as_canonical_extended_json((const bson_t*)this->doc, 0);
		sb->AppendSlow((const UTF8Char*)str);
		bson_free(str);
		return true;
	}
	else
	{
		return false;
	}
}

Optional<Text::String> DB::MongoDBReader::GetNewStr(UOSInt colIndex)
{
	if (colIndex != 0)
		return 0;
	if (this->doc)
	{
		char *str = bson_as_canonical_extended_json((const bson_t*)this->doc, 0);
		NN<Text::String> ret = Text::String::NewNotNullSlow((const UTF8Char*)str);
		bson_free(str);
		return ret;
	}
	else
	{
		return 0;
	}
}

UnsafeArrayOpt<UTF8Char> DB::MongoDBReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
{
	if (colIndex != 0)
		return 0;
	if (this->doc)
	{
		char *str = bson_as_canonical_extended_json((const bson_t*)this->doc, 0);
		buff = Text::StrConcatS(buff, (const UTF8Char*)str, buffSize);
		bson_free(str);
		return buff;
	}
	else
	{
		return 0;
	}
}

Data::Timestamp DB::MongoDBReader::GetTimestamp(UOSInt colIndex)
{
	return Data::Timestamp(0);
}

Double DB::MongoDBReader::GetDblOrNAN(UOSInt colIndex)
{
	return NAN;
}

Bool DB::MongoDBReader::GetBool(UOSInt colIndex)
{
	return 0;
}

UOSInt DB::MongoDBReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt DB::MongoDBReader::GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
{
	return 0;
}

Optional<Math::Geometry::Vector2D> DB::MongoDBReader::GetVector(UOSInt colIndex)
{
	return 0;
}

Bool DB::MongoDBReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	return false;
}

UOSInt DB::MongoDBReader::ColCount()
{
	return 1;
}

OSInt DB::MongoDBReader::GetRowChanged()
{
	return 0;
}

UnsafeArrayOpt<UTF8Char> DB::MongoDBReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	if (colIndex != 0)
		return 0;
	return Text::StrConcatC(buff, UTF8STRC("Data"));
}

Bool DB::MongoDBReader::IsNull(UOSInt colIndex)
{
	if (colIndex != 0)
		return true;
	return false;
}

DB::DBUtil::ColType DB::MongoDBReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex != 0)
		return DB::DBUtil::CT_Unknown;
	colSize.Set(0xffffffff);
	return DB::DBUtil::CT_VarUTF8Char;
}

Bool DB::MongoDBReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	if (colIndex != 0)
		return false;
	colDef->SetColName(CSTR("Data"));
	colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
	colDef->SetColSize(65536);
	colDef->SetColDP(0);
	colDef->SetDefVal(Text::CString(nullptr));
	colDef->SetAttr(Text::CString(nullptr));
	colDef->SetAutoIncNone();
	colDef->SetNotNull(true);
	colDef->SetPK(false);
	return true;
}
