#ifndef _SM_DB_COLDEF
#define _SM_DB_COLDEF
#include "DB/ReadingDB.h"
#include "DB/SQLBuilder.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace DB
{
	class ColDef
	{
	private:
		Text::String *colName;
		DB::DBUtil::ColType colType;
		UOSInt colSize;
		UOSInt colDP;
		Bool notNull;
		Bool pk;
		Bool autoInc;
		Text::String *defVal;
		Text::String *attr;

	public:
		ColDef(Text::CString colName);
		ColDef(Text::String *colName);
		~ColDef();

		Text::String *GetColName();
		DB::DBUtil::ColType GetColType();
		UOSInt GetColSize();
		UOSInt GetColDP();
		Bool IsNotNull();
		Bool IsPK();
		Bool IsAutoInc();
		Text::String *GetDefVal();
		Text::String *GetAttr();
		Bool GetDefVal(DB::SQLBuilder *sql);

		void SetColName(const UTF8Char *colName);
		void SetColName(Text::CString colName);
		void SetColName(Text::String *colName);
		void SetColType(DB::DBUtil::ColType colType);
		void SetColSize(UOSInt colSize);
		void SetColDP(UOSInt colDP);
		void SetNotNull(Bool notNull);
		void SetPK(Bool pk);
		void SetAutoInc(Bool autoInc);
		void SetDefVal(const UTF8Char *defVal);
		void SetDefVal(Text::String *defVal);
		void SetAttr(const UTF8Char *attr);
		void SetAttr(Text::String *attr);

		void Set(ColDef *colDef);

		UTF8Char *ToColTypeStr(UTF8Char *sbuff);
		ColDef *Clone();
	};
}
#endif
