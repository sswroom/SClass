#ifndef _SM_DB_DBFFILE
#define _SM_DB_DBFFILE
#include "Data/ArrayList.h"
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
			NotNullPtr<Text::String> name;
			Int32 type;
			UOSInt colOfst;
			UOSInt colSize;
			UOSInt colDP;
		} DBFCol;

	private:
		//IO::SeekableStream *stm;
		NotNullPtr<IO::StreamData> stmData;
		Text::Encoding enc;
//		Sync::Mutex *mut;
		UInt64 refPos;
		UInt32 rowSize;
		UOSInt colCnt;
		UOSInt rowCnt;
		DBFCol *cols;
		NotNullPtr<Text::String> name;

	public:
		DBFFile(NotNullPtr<IO::StreamData> stmData, UInt32 codePage);
		virtual ~DBFFile();

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names);
		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		Bool IsError();
		UInt32 GetCodePage();
		UOSInt GetColSize(UOSInt colIndex);
		OSInt GetColIndex(const UTF8Char *name);
		WChar *GetRecord(WChar *buff, UOSInt row, UOSInt col);
		UTF8Char *GetRecord(UTF8Char *buff, UOSInt row, UOSInt col);
		UOSInt GetColCount();
		UOSInt GetRowCnt();
		UTF8Char *GetColumnName(UOSInt colIndex, UTF8Char *buff);
		DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		Bool ReadRowData(UOSInt row, UInt8 *recordBuff);

		static Int32 GetCodePage(UInt8 langDriver);
		static UInt8 GetLangDriver(UInt32 codePage);
	};

	class DBFReader : public DB::DBReader
	{
	private:
		DB::DBFFile *dbf;
		UOSInt rowCnt;
		OSInt currIndex;
		Bool recordExist;
		UInt8 *recordData;
		UOSInt colCnt;
		DB::DBFFile::DBFCol *cols;
		UOSInt rowSize;
		Text::Encoding *enc;
	public:
		DBFReader(DB::DBFFile *dbf, UOSInt colCnt, DB::DBFFile::DBFCol *cols, UOSInt rowSize, Text::Encoding *enc);
		virtual ~DBFReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Text::String *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Geometry::Vector2D *GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

		virtual Bool IsNull(UOSInt colIndex);
//		virtual WChar *GetName(UOSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
	};
}
#endif
