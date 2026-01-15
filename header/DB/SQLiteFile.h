#ifndef _SM_DB_SQLITEFILE
#define _SM_DB_SQLITEFILE
#include "Data/ArrayListStrUTF8.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/DBTool.h"
#include "IO/StreamData.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class SQLiteFile : public DB::DBConn
	{
	private:
		NN<Text::String> fileName;
		AnyType db;
//		Data::ArrayList<Text::CString> tableNames;
		Bool delOnClose;
		Optional<Text::String> lastErrMsg;

		void Init();
	public:
		SQLiteFile(NN<Text::String> fileName);
		SQLiteFile(Text::CStringNN fileName);
		virtual ~SQLiteFile();

		virtual DB::SQLType GetSQLType() const;
		virtual ConnType GetConnType() const;
		virtual Int8 GetTzQhr() const;
		virtual void ForceTz(Int8 tzQhr);
		virtual void GetConnName(NN<Text::StringBuilderUTF8> sb);
		virtual void Close();
		virtual IntOS ExecuteNonQuery(Text::CStringNN sql);
		virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);

		void SetDeleteOnClose(Bool delOnClose);
		Bool IsError();
		NN<Text::String> GetFileName();

		static Optional<Math::Geometry::Vector2D> GPGeometryParse(UnsafeArray<const UInt8> buff, UIntOS buffSize);
		
		static Optional<DBTool> CreateDBTool(NN<Text::String> fileName, NN<IO::LogTool> log, Text::CString logPrefix);
		static Optional<DBTool> CreateDBTool(Text::CStringNN fileName, NN<IO::LogTool> log, Text::CString logPrefix);
	};

	class SQLiteReader : public DB::DBReader
	{
	private:
		typedef struct
		{
			DB::DBUtil::ColType colType;
			AnyType colData;
			Int64 dataVal;
			Bool isNull;
		} ColumnData;
	private:
		NN<SQLiteFile> conn;
		AnyType hStmt;
		UIntOS colCnt;
		UnsafeArray<DB::DBUtil::ColType> colTypes;
		Bool isFirst;
		Bool firstResult;
		
		void UpdateColTypes();
	public:
		SQLiteReader(NN<SQLiteFile> conn, AnyType hStmt);
		~SQLiteReader();

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
		virtual Data::Date GetDate(UIntOS colIndex);
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
