#ifndef _SM_DB_COLDEF
#define _SM_DB_COLDEF
#include "DB/ReadingDB.h"
#include "DB/SQLBuilder.h"

namespace DB
{
	class ColDef
	{
	private:
		const UTF8Char *colName;
		DB::DBUtil::ColType colType;
		UOSInt colSize;
		UOSInt colDP;
		Bool notNull;
		Bool isPK;
		Bool autoInc;
		const UTF8Char *defVal;
		const UTF8Char *attr;

	public:
		ColDef(const UTF8Char *colName);
		~ColDef();

		const UTF8Char *GetColName();
		DB::DBUtil::ColType GetColType();
		UOSInt GetColSize();
		UOSInt GetColDP();
		Bool GetIsNotNull();
		Bool GetIsPK();
		Bool GetIsAutoInc();
		const UTF8Char *GetDefVal();
		const UTF8Char *GetAttr();
		Bool GetDefVal(DB::SQLBuilder *sql);

		void SetColName(const UTF8Char *colName);
		void SetColType(DB::DBUtil::ColType colType);
		void SetColSize(UOSInt colSize);
		void SetColDP(UOSInt colDP);
		void SetIsNotNull(Bool notNull);
		void SetIsPK(Bool isPK);
		void SetIsAutoInc(Bool autoInc);
		void SetDefVal(const UTF8Char *defVal);
		void SetAttr(const UTF8Char *attr);

		UTF8Char *ToColTypeStr(UTF8Char *sbuff);
	};
}
#endif
