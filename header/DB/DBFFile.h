#ifndef _SM_DB_DBFFILE
#define _SM_DB_DBFFILE
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/StreamData.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "Text/StringBuilderUTF8.h"

namespace DB
{
	class DBFFile : public DB::ReadingDB
	{
	public:
		typedef struct
		{
			NN<Text::String> name;
			Int32 type;
			UIntOS colOfst;
			UIntOS colSize;
			UIntOS colDP;
		} DBFCol;

	private:
		NN<IO::StreamData> stmData;
		Text::Encoding enc;
		UInt64 refPos;
		UInt32 rowSize;
		UIntOS colCnt;
		UIntOS rowCnt;
		UnsafeArray<DBFCol> cols;
		NN<Text::String> name;

	public:
		DBFFile(NN<IO::StreamData> stmData, UInt32 codePage);
		virtual ~DBFFile();

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		Bool IsError();
		UInt32 GetCodePage();
		UIntOS GetColSize(UIntOS colIndex);
		IntOS GetColIndex(UnsafeArray<const UTF8Char> name);
		UnsafeArrayOpt<WChar> GetRecord(UnsafeArray<WChar> buff, UIntOS row, UIntOS col);
		UnsafeArrayOpt<UTF8Char> GetRecord(UnsafeArray<UTF8Char> buff, UIntOS row, UIntOS col);
		Bool GetRecord(NN<Text::StringBuilderUTF8> sb, UIntOS row, UIntOS col);
		UIntOS GetColCount();
		UIntOS GetRowCnt();
		UnsafeArrayOpt<UTF8Char> GetColumnName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		DB::DBUtil::ColType GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize);
		Bool GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef);
		Bool ReadRowData(UIntOS row, UnsafeArray<UInt8> recordBuff);

		static Int32 GetCodePage(UInt8 langDriver);
		static UInt8 GetLangDriver(UInt32 codePage);
	};

	class DBFReader : public DB::DBReader
	{
	private:
		NN<DB::DBFFile> dbf;
		UIntOS rowCnt;
		IntOS currIndex;
		Bool recordExist;
		UnsafeArray<UInt8> recordData;
		UIntOS colCnt;
		UnsafeArray<DB::DBFFile::DBFCol> cols;
		UIntOS rowSize;
		NN<Text::Encoding> enc;
	public:
		DBFReader(NN<DB::DBFFile> dbf, UIntOS colCnt, UnsafeArray<DB::DBFFile::DBFCol> cols, UIntOS rowSize, NN<Text::Encoding> enc);
		virtual ~DBFReader();

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

		virtual Bool IsNull(UIntOS colIndex);
//		virtual WChar *GetName(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);
	};
}
#endif
