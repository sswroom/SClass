#ifndef _SM_MATH_TSPFILE
#define _SM_MATH_TSPFILE
#include "Data/ByteBuffer.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/StreamData.h"

namespace Math
{
	class TSPFile : public DB::ReadingDB
	{
	private:
		Data::ByteBuffer ptBuff;
		Data::ByteBuffer hdrBuff;
		UOSInt rowCnt;
		UOSInt rowSize;
		Int32 fileType;

	public:
		TSPFile(NotNullPtr<IO::StreamData> fd);
		virtual ~TSPFile();

		virtual UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names);
		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(NotNullPtr<DB::DBReader> r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		UInt8 *GetRowPtr(UOSInt row) const;
		UInt8 *GetHdrPtr() const;
		UOSInt GetRowCnt() const;
		UOSInt GetRowSize() const;
	};

	class TSPReader : public DB::DBReader
	{
	private:
		TSPFile *file;
		UInt8 *currRowPtr;
		OSInt currRow;
		UOSInt rowCnt;
		UOSInt rowSize;
	public:
		TSPReader(TSPFile *file);
		virtual ~TSPReader();

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
		virtual Bool GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid);

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);

		static Text::CString GetName(UOSInt colIndex, UOSInt rowSize);
		static Bool GetColDefV(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef, UOSInt rowSize);
	};

	class TSPHReader : public DB::DBReader
	{
	private:
		TSPFile *file;
		OSInt currRow;
	public:
		TSPHReader(TSPFile *file);
		virtual ~TSPHReader();

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
		virtual Bool GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid);

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);

		static Text::CString GetName(UOSInt colIndex);
		static Bool GetColDefV(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
	};
}
#endif
