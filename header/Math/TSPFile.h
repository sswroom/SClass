#ifndef _SM_MATH_TSPFILE
#define _SM_MATH_TSPFILE
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "IO/IStreamData.h"

namespace Math
{
	class TSPFile : public DB::ReadingDB
	{
	private:
		UInt8 *ptBuff;
		UInt8 *hdrBuff;
		UOSInt rowCnt;
		UOSInt rowSize;
		Int32 fileType;

	public:
		TSPFile(IO::IStreamData *fd);
		virtual ~TSPFile();

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names); // no need to release
		virtual DB::DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();

		UInt8 *GetRowPtr(UOSInt row);
		UInt8 *GetHdrPtr();
		UOSInt GetRowCnt();
		UOSInt GetRowSize();
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

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);

		const UTF8Char *GetName(UOSInt colIndex);
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

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);

		const UTF8Char *GetName(UOSInt colIndex);
	};
}
#endif
