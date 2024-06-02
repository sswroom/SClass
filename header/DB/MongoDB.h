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
		IO::LogTool *log;
		Text::String *errorMsg;
	public:
		MongoDB(Text::CStringNN url, Text::CString database, IO::LogTool *log);
		virtual ~MongoDB();
		
		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *columNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		UOSInt GetDatabaseNames(NN<Data::ArrayListStringNN> names);
		void FreeDatabaseNames(NN<Data::ArrayListStringNN> names);

		static void BuildURL(NN<Text::StringBuilderUTF8> out, Text::CString userName, Text::CString password, Text::CStringNN host, UInt16 port);
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
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

//		virtual WChar *GetName(OSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);
	};
}
#endif
