#ifndef _SM_DB_DBREADER
#define _SM_DB_DBREADER
#include "Data/Class.h"
#include "Data/VariObject.h"
#include "DB/DBUtil.h"
#include "Math/Vector2D.h"
#include "Text/StringBuilderUTF.h"

namespace DB
{
	class ColDef;
	class TableDef;
	class DBReader
	{
	public:
		typedef enum
		{
			DET_OK = 0,
			DET_NULL = 1,
			DET_ERROR = 2
		} DateErrType;

	protected:
		DBReader(){};
		virtual ~DBReader(){};

	public:
		virtual Bool ReadNext() = 0;
		virtual UOSInt ColCount() = 0;
		virtual OSInt GetRowChanged() = 0; //-1 = error
		virtual Int32 GetInt32(UOSInt colIndex) = 0;
		virtual Int64 GetInt64(UOSInt colIndex) = 0;
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff) = 0;
		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb) = 0;
		Bool GetStrN(UOSInt colIndex, Text::StringBuilderUTF *sb) { sb->ClearStr(); return GetStr(colIndex, sb); }
		virtual const UTF8Char *GetNewStr(UOSInt colIndex) = 0;
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize) = 0;
		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal) = 0;
		virtual Double GetDbl(UOSInt colIndex) = 0;
		virtual Bool GetBool(UOSInt colIndex) = 0;
		virtual UOSInt GetBinarySize(UOSInt colIndex) = 0;
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff) = 0;
		virtual Math::Vector2D *GetVector(UOSInt colIndex) = 0;
		virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid) = 0;

		virtual Bool IsNull(UOSInt colIndex) = 0;
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff) = 0;
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize) = 0;
		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef) = 0;
		virtual void DelNewStr(const UTF8Char *s) = 0;

		TableDef *GenTableDef(const UTF8Char *tableName);
		Data::VariObject *CreateVariObject();
		Data::Class *CreateClass();
	};
}
#endif
