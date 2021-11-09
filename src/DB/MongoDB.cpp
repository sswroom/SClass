#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ColDef.h"
#include "DB/MongoDB.h"
#include "Sync/Interlocked.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/TextEnc/URIEncoding.h"
#if defined(__FreeBSD__)
#include <bson.h>
#include <mongoc.h>
#define mongoc_database_get_collection_names_with_opts(a, b, c) mongoc_database_get_collection_names(a, c)
#define mongoc_client_get_database_names_with_opts(a, b, c) mongoc_client_get_database_names(a, c)
#else
#include <bson/bson.h>
#include <mongoc/mongoc.h>
#endif

Int32 DB::MongoDB::initCnt = 0;

DB::MongoDB::MongoDB(const UTF8Char *url, const UTF8Char *database, IO::LogTool *log) : DB::ReadingDB(url)
{
	mongoc_client_t *client;
	this->log = log;
	this->errorMsg = 0;
	if (Sync::Interlocked::Increment(&initCnt) == 1)
	{
		mongoc_init();
	}
	client = mongoc_client_new((const Char*)url);
	this->client = client;
	this->tableNames = 0;
	if (database)
	{
		this->database = Text::StrCopyNew(database);
	}
	else
	{
		this->database = 0;
	}
}

DB::MongoDB::~MongoDB()
{
	if (this->client)
	{
		mongoc_client_destroy((mongoc_client_t*)this->client);
	}
	SDEL_TEXT(this->database);
	if (this->tableNames)
	{
		UOSInt i;
		i = this->tableNames->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->tableNames->GetItem(i));
		}
		DEL_CLASS(this->tableNames);
	}
	if (Sync::Interlocked::Decrement(&initCnt) == 0)
	{
		mongoc_cleanup();
	}
}

UOSInt DB::MongoDB::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	if (this->database == 0 || this->client == 0)
		return 0;
	if (this->tableNames == 0)
	{
		bson_error_t error;
		NEW_CLASS(this->tableNames, Data::ArrayList<const UTF8Char*>());
		mongoc_database_t *db = mongoc_client_get_database((mongoc_client_t*)this->client, (const Char*)this->database);
		char **strv;
		strv = mongoc_database_get_collection_names_with_opts(db, 0, &error);
		SDEL_TEXT(this->errorMsg);
		if (strv == 0)
		{
			this->errorMsg = Text::StrCopyNew((const UTF8Char*)error.message);
		}
		else
		{
			OSInt i = 0;
			while (strv[i])
			{
				this->tableNames->Add(Text::StrCopyNew((const UTF8Char*)strv[i]));
				i++;
			}
			bson_strfreev(strv);
		}		
		mongoc_database_destroy(db);
	}
	names->AddAll(this->tableNames);
	return this->tableNames->GetCount();
}

DB::DBReader *DB::MongoDB::GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, DB::QueryConditions *condition)
{
	if (this->database && this->client)
	{
		mongoc_collection_t *coll = mongoc_client_get_collection((mongoc_client_t*)this->client, (const Char*)this->database, (const Char*)tableName);
		if (coll)
		{
			DB::MongoDBReader *reader;
			NEW_CLASS(reader, DB::MongoDBReader(this, coll));
			return reader;
		}
	}
	return 0;
}

void DB::MongoDB::CloseReader(DBReader *r)
{
	DB::MongoDBReader *reader = (DB::MongoDBReader*)r;
	if (reader)
	{
		DEL_CLASS(reader);
	}
}

void DB::MongoDB::GetErrorMsg(Text::StringBuilderUTF *str)
{
	if (this->errorMsg)
	{
		str->Append(this->errorMsg);
	}
}

void DB::MongoDB::Reconnect()
{

}

UOSInt DB::MongoDB::GetDatabaseNames(Data::ArrayList<const UTF8Char*> *names)
{
	bson_error_t error;
	SDEL_TEXT(this->errorMsg);
	if (this->client == 0)
		return 0;
	char **strv = mongoc_client_get_database_names_with_opts((mongoc_client_t*)this->client, 0, &error);
	if (strv == 0)
	{
		this->errorMsg = Text::StrCopyNew((const UTF8Char*)error.message);
		return 0;
	}
	else
	{
		UOSInt i = 0;
		while (strv[i])
		{
			names->Add(Text::StrCopyNew((const UTF8Char*)strv[i]));
			i++;
		}
		bson_strfreev(strv);
		return i;
	}
}

void DB::MongoDB::FreeDatabaseNames(Data::ArrayList<const UTF8Char*> *names)
{
	UOSInt i = names->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(names->GetItem(i));
	}
	names->Clear();
}

void DB::MongoDB::BuildURL(Text::StringBuilderUTF *out, const UTF8Char *userName, const UTF8Char *password, const UTF8Char *host, UInt16 port)
{
	UTF8Char sbuff[256];
	out->Append((const UTF8Char*)"mongodb://");
	if (userName)
	{
		Text::TextEnc::URIEncoding::URIEncode(sbuff, userName);
		out->Append(sbuff);
		if (password)
		{
			out->AppendChar(':', 1);
			Text::TextEnc::URIEncoding::URIEncode(sbuff, password);
			out->Append(sbuff);
		}
		out->AppendChar('@', 1);
	}
	out->Append(host);
	out->AppendChar(':', 1);
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

WChar *DB::MongoDBReader::GetStr(UOSInt colIndex, WChar *buff)
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

Bool DB::MongoDBReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	if (colIndex != 0)
		return false;
	if (this->doc)
	{
		char *str = bson_as_canonical_extended_json((const bson_t*)this->doc, 0);
		sb->Append((const UTF8Char*)str);
		bson_free(str);
		return true;
	}
	else
	{
		return false;
	}
}

const UTF8Char *DB::MongoDBReader::GetNewStr(UOSInt colIndex)
{
	if (colIndex != 0)
		return 0;
	if (this->doc)
	{
		char *str = bson_as_canonical_extended_json((const bson_t*)this->doc, 0);
		const UTF8Char *ret = Text::StrCopyNew((const UTF8Char*)str);
		bson_free(str);
		return ret;
	}
	else
	{
		return 0;
	}
}

UTF8Char *DB::MongoDBReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
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

DB::DBReader::DateErrType DB::MongoDBReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	return DB::DBReader::DET_ERROR;
}

Double DB::MongoDBReader::GetDbl(UOSInt colIndex)
{
	return 0;
}

Bool DB::MongoDBReader::GetBool(UOSInt colIndex)
{
	return 0;
}

UOSInt DB::MongoDBReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt DB::MongoDBReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	return 0;
}

Math::Vector2D *DB::MongoDBReader::GetVector(UOSInt colIndex)
{
	return 0;
}

Bool DB::MongoDBReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
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

UTF8Char *DB::MongoDBReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	if (colIndex != 0)
		return 0;
	return Text::StrConcat(buff, (const UTF8Char*)"Data");
}

Bool DB::MongoDBReader::IsNull(UOSInt colIndex)
{
	if (colIndex != 0)
		return true;
	return false;
}

DB::DBUtil::ColType DB::MongoDBReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex != 0)
		return DB::DBUtil::CT_Unknown;
	return DB::DBUtil::CT_VarChar;
}

Bool DB::MongoDBReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex != 0)
		return false;
	colDef->SetColName((const UTF8Char*)"Data");
	colDef->SetColType(DB::DBUtil::CT_VarChar);
	colDef->SetColSize(65536);
	colDef->SetColDP(0);
	colDef->SetDefVal(0);
	colDef->SetAttr(0);
	colDef->SetAutoInc(false);
	colDef->SetNotNull(true);
	colDef->SetPK(false);
	return true;
}

void DB::MongoDBReader::DelNewStr(const UTF8Char *s)
{
	Text::StrDelNew(s);
}
