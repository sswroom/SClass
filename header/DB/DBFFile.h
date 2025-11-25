#ifndef _SM_DB_DBFFILE
#define _SM_DB_DBFFILE
#include "Data/ArrayList.hpp"
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
			UOSInt colOfst;
			UOSInt colSize;
			UOSInt colDP;
		} DBFCol;

	private:
		NN<IO::StreamData> stmData;
		Text::Encoding enc;
		UInt64 refPos;
		UInt32 rowSize;
		UOSInt colCnt;
		UOSInt rowCnt;
		UnsafeArray<DBFCol> cols;
		NN<Text::String> name;

	public:
		DBFFile(NN<IO::StreamData> stmData, UInt32 codePage);
		virtual ~DBFFile();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		Bool IsError();
		UInt32 GetCodePage();
		UOSInt GetColSize(UOSInt colIndex);
		OSInt GetColIndex(UnsafeArray<const UTF8Char> name);
		UnsafeArrayOpt<WChar> GetRecord(UnsafeArray<WChar> buff, UOSInt row, UOSInt col);
		UnsafeArrayOpt<UTF8Char> GetRecord(UnsafeArray<UTF8Char> buff, UOSInt row, UOSInt col);
		Bool GetRecord(NN<Text::StringBuilderUTF8> sb, UOSInt row, UOSInt col);
		UOSInt GetColCount();
		UOSInt GetRowCnt();
		UnsafeArrayOpt<UTF8Char> GetColumnName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		Bool ReadRowData(UOSInt row, UnsafeArray<UInt8> recordBuff);

		static Int32 GetCodePage(UInt8 langDriver);
		static UInt8 GetLangDriver(UInt32 codePage);
	};

	class DBFReader : public DB::DBReader
	{
	private:
		NN<DB::DBFFile> dbf;
		UOSInt rowCnt;
		OSInt currIndex;
		Bool recordExist;
		UnsafeArray<UInt8> recordData;
		UOSInt colCnt;
		UnsafeArray<DB::DBFFile::DBFCol> cols;
		UOSInt rowSize;
		NN<Text::Encoding> enc;
	public:
		DBFReader(NN<DB::DBFFile> dbf, UOSInt colCnt, UnsafeArray<DB::DBFFile::DBFCol> cols, UOSInt rowSize, NN<Text::Encoding> enc);
		virtual ~DBFReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual UnsafeArrayOpt<WChar> GetStr(UOSInt colIndex, UnsafeArray<WChar> buff);
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDblOrNAN(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

		virtual Bool IsNull(UOSInt colIndex);
//		virtual WChar *GetName(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);
	};
}
#endif
