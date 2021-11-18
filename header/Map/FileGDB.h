#ifndef _SM_MAP_FILEGDB
#define _SM_MAP_FILEGDB
#include "Data/ArrayListStrUTF8.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"

namespace Map
{
	class FileGDB : public DB::ReadingDB
	{
	private:
		void *gdb;
		const WChar *wPath;
		Data::ArrayListStrUTF8 *tables;
		Data::ArrayListStrUTF8 *features;

	public:
		FileGDB(const UTF8Char *filePath);
		virtual ~FileGDB();

		Bool IsError();

		virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names); // no need to release
		virtual DB::DBReader *GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF *str);
		virtual void Reconnect();
	};

	class FileGDBReader : public DB::DBReader
	{
	private:
		void *gdb;
		void *table;
		void *rows;
		void *currRow;
		void *fieldDefs;
	public:
		FileGDBReader(void *gdb, void *table, void *rows);
		virtual ~FileGDBReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb);
		virtual const UTF8Char *GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal); //return 0 = ok, 1 = NULL, 2 = Error
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Math::Vector2D *GetVector(UOSInt colIndex);

		virtual Bool IsNull(UOSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual void DelNewStr(const UTF8Char *s);
	};
}
#endif
