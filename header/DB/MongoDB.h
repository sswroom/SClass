#ifndef _SM_DB_MONGODB
#define _SM_DB_MONGODB
#include "Data/ArrayList.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/LogTool.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class MongoDB : public DB::ReadingDB
	{
	private:
		static Int32 initCnt;
	private:
		void *client;
		Text::String *database;
		Data::ArrayList<const UTF8Char*> *tableNames;
		IO::LogTool *log;
		Text::String *errorMsg;
	public:
		MongoDB(Text::CString url, Text::CString database, IO::LogTool *log);
		virtual ~MongoDB();
		
		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names); // no need to release
		virtual DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *columNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual void CloseReader(DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();

		UOSInt GetDatabaseNames(Data::ArrayList<const UTF8Char*> *names);
		void FreeDatabaseNames(Data::ArrayList<const UTF8Char*> *names);

		static void BuildURL(Text::StringBuilderUTF8 *out, Text::CString userName, Text::CString password, Text::CString host, UInt16 port);
	};

	class MongoDBReader : public DB::DBReader
	{
	private:
		MongoDB *conn;
		void *coll;
		void *query;
		void *cursor;
		const void *doc;

	public:
		MongoDBReader(MongoDB *conn, void *coll);
		virtual ~MongoDBReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb);
		virtual Text::String *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

//		virtual WChar *GetName(OSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
	};
}
#endif
