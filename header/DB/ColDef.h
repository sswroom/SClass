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
	public:
		enum class AutoIncType
		{
			None,
			Default,
			Always
		};
	private:
		Text::String *colName;
		DB::DBUtil::ColType colType;
		Text::String *nativeType;
		UOSInt colSize;
		UOSInt colDP;
		Bool notNull;
		Bool pk;
		AutoIncType autoInc;
		Int64 autoIncStartIndex;
		Int64 autoIncStep;
		Text::String *defVal;
		Text::String *attr;

		static void AppendDefVal(DB::SQLBuilder *sql, Text::CString defVal);
	public:
		ColDef(Text::CString colName);
		ColDef(Text::String *colName);
		~ColDef();

		Text::String *GetColName() const;
		DB::DBUtil::ColType GetColType() const;
		Text::String *GetNativeType() const;
		UOSInt GetColSize() const;
		UOSInt GetColDP() const;
		Bool IsNotNull() const;
		Bool IsPK() const;
		Bool IsAutoInc() const;
		AutoIncType GetAutoIncType() const;
		Int64 GetAutoIncStartIndex() const;
		Int64 GetAutoIncStep() const;
		Text::String *GetDefVal() const;
		Text::String *GetAttr() const;
		Bool GetDefVal(DB::SQLBuilder *sql) const;

		void SetColName(const UTF8Char *colName);
		void SetColName(Text::CString colName);
		void SetColName(Text::String *colName);
		void SetColType(DB::DBUtil::ColType colType);
		void SetNativeType(Text::String *nativeType);
		void SetNativeType(Text::CString nativeType);
		void SetColSize(UOSInt colSize);
		void SetColDP(UOSInt colDP);
		void SetNotNull(Bool notNull);
		void SetPK(Bool pk);
		void SetAutoIncNone();
		void SetAutoInc(AutoIncType autoInc, Int64 startIndex, Int64 incStep);
		void SetDefVal(Text::CString defVal);
		void SetDefVal(Text::String *defVal);
		void SetAttr(Text::CString attr);
		void SetAttr(Text::String *attr);

		void Set(const ColDef *colDef);

		UTF8Char *ToColTypeStr(UTF8Char *sbuff) const;
		ColDef *Clone() const;
	};
}
#endif
