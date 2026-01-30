#ifndef _SM_DB_MONGODB
#define _SM_DB_MONGODB
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
		Optional<Text::String> database;
		Optional<IO::LogTool> log;
		Optional<Text::String> errorMsg;
	public:
		MongoDB(Text::CStringNN url, Text::CString database, Optional<IO::LogTool> log);
		virtual ~MongoDB();
		
		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		UIntOS GetDatabaseNames(NN<Data::ArrayListStringNN> names);
		void FreeDatabaseNames(NN<Data::ArrayListStringNN> names);

		static void BuildURL(NN<Text::StringBuilderUTF8> out, Text::CString userName, Text::CString password, Text::CStringNN host, UInt16 port);
	};

	class MongoDBReader : public DB::DBReader
	{
	private:
		NN<MongoDB> conn;
		void *coll;
		void *query;
		void *cursor;
		const void *doc;

	public:
		MongoDBReader(NN<MongoDB> conn, void *coll);
		virtual ~MongoDBReader();

		virtual Bool ReadNext();
		virtual UIntOS ColCount();
		virtual IntOS GetRowChanged();

		virtual Int32 GetInt32(UIntOS colIndex);
		virtual Int64 GetInt64(UIntOS colIndex);
		virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff);
		virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize);
		virtual Data::Timestamp GetTimestamp(UIntOS colIndex);
		virtual Double GetDblOrNAN(UIntOS colIndex);
		virtual Bool GetBool(UIntOS colIndex);
		virtual UIntOS GetBinarySize(UIntOS colIndex);
		virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex);
		virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid);

		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UIntOS colIndex);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);
	};
}
#endif
