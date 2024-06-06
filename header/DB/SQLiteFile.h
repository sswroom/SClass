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
		void *db;
//		Data::ArrayList<Text::CString> tableNames;
		Bool delOnClose;
		Text::String *lastErrMsg;

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
		virtual OSInt ExecuteNonQuery(Text::CStringNN sql);
		virtual Optional<DBReader> ExecuteReader(Text::CStringNN sql);
		virtual void CloseReader(NN<DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual Bool IsLastDataError();
		virtual void Reconnect();

		virtual Optional<DB::DBTransaction> BeginTransaction();
		virtual void Commit(NN<DB::DBTransaction> tran);
		virtual void Rollback(NN<DB::DBTransaction> tran);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);

		void SetDeleteOnClose(Bool delOnClose);
		Bool IsError();
		NN<Text::String> GetFileName();

		static Optional<Math::Geometry::Vector2D> GPGeometryParse(const UInt8 *buff, UOSInt buffSize);
		
		static Optional<DBTool> CreateDBTool(NN<Text::String> fileName, NN<IO::LogTool> log, Text::CString logPrefix);
		static Optional<DBTool> CreateDBTool(Text::CStringNN fileName, NN<IO::LogTool> log, Text::CString logPrefix);
	};

	class SQLiteReader : public DB::DBReader
	{
	private:
		typedef struct
		{
			DB::DBUtil::ColType colType;
			void *colData;
			Int64 dataVal;
			Bool isNull;
		} ColumnData;
	private:
		SQLiteFile *conn;
		void *hStmt;
		UOSInt colCnt;
		DB::DBUtil::ColType *colTypes;
		Bool isFirst;
		Bool firstResult;
		
		void UpdateColTypes();
	public:
		SQLiteReader(SQLiteFile *conn, void *hStmt);
		~SQLiteReader();

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
		virtual Data::Date GetDate(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);
	};
}
#endif
