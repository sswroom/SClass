#ifndef _SM_DB_DBUTIL
#define _SM_DB_DBUTIL
#include "Data/DateTime.h"
#include "Math/Vector2D.h"

namespace DB
{
	class DBUtil
	{
	public:
		typedef enum
		{
			SVR_TYPE_UNKNOWN = 0,
			SVR_TYPE_MSSQL = 1,
			SVR_TYPE_MYSQL = 2,
			SVR_TYPE_ORACLE = 3,
			SVR_TYPE_ACCESS = 4,
			SVR_TYPE_TEXT = 5,
			SVR_TYPE_SQLITE = 6,
			SVR_TYPE_WBEM = 7,
			SVR_TYPE_MDBTOOLS = 8
		} ServerType;

		typedef enum
		{
			CT_Unknown,
			CT_UInt32,
			CT_Int32,
			CT_VarChar,
			CT_Char,
			CT_DateTime, //acc = 1/333s
			CT_Double,
			CT_Float,
			CT_Bool,
			CT_Byte,
			CT_Int16,
			CT_Int64,
			CT_UInt16,
			CT_UInt64,
			CT_Binary,
			CT_Vector,
			CT_NVarChar,
			CT_NChar,
			CT_DateTime2 //acc = 0.1ms
		} ColType;

		static UTF8Char *SDBStrUTF8(UTF8Char *sqlstr, const UTF8Char *str, ServerType svrType);
		static UOSInt SDBStrUTF8Leng(const UTF8Char *str, ServerType svrType);
		static UTF8Char *SDBStrW(UTF8Char *sqlstr, const WChar *str, ServerType svrType);
		static UOSInt SDBStrWLeng(const WChar *str, ServerType svrType);
		static UTF8Char *SDBInt32(UTF8Char *sqlstr, Int32 val, ServerType svrType);
		static UOSInt SDBInt32Leng(Int32 val, ServerType svrType);
		static UTF8Char *SDBInt64(UTF8Char *sqlstr, Int64 val, ServerType svrType);
		static UOSInt SDBInt64Leng(Int64 val, ServerType svrType);
		static UTF8Char *SDBUInt32(UTF8Char *sqlstr, UInt32 val, ServerType svrType);
		static UOSInt SDBUInt32Leng(UInt32 val, ServerType svrType);
		static UTF8Char *SDBUInt64(UTF8Char *sqlstr, UInt64 val, ServerType svrType);
		static UOSInt SDBUInt64Leng(UInt64 val, ServerType svrType);
		static UTF8Char *SDBDate(UTF8Char *sqlstr, Data::DateTime *dat, ServerType svrType, Int8 tzQhr);
		static UOSInt SDBDateLeng(Data::DateTime *dat, ServerType svrType);
		static UTF8Char *SDBDbl(UTF8Char *sqlstr, Double val, ServerType svrType);
		static UOSInt SDBDblLeng(Double val, ServerType svrType);
		static UTF8Char *SDBSng(UTF8Char *sqlstr, Single val, ServerType svrType);
		static UOSInt SDBSngLeng(Single val, ServerType svrType);
		static UTF8Char *SDBBool(UTF8Char *sqlstr, Bool val, ServerType svrType);
		static UOSInt SDBBoolLeng(Bool val, ServerType svrType);
		static UTF8Char *SDBBin(UTF8Char *sqlstr, const UInt8 *buff, UOSInt size, ServerType svrType);
		static UOSInt SDBBinLeng(const UInt8 *buff, UOSInt size, ServerType svrType);
		static UTF8Char *SDBVector(UTF8Char *sqlstr, Math::Vector2D *vec, ServerType svrType);
		static UOSInt SDBVectorLeng(Math::Vector2D *vec, ServerType svrType);
		static UTF8Char *SDBColUTF8(UTF8Char *sqlstr, const UTF8Char *colName, ServerType svrType);
		static UOSInt SDBColUTF8Leng(const UTF8Char *colName, ServerType svrType);
		static UTF8Char *SDBColW(UTF8Char *sqlstr, const WChar *colName, ServerType svrType);
		static UOSInt SDBColWLeng(const WChar *colName, ServerType svrType);
		static UTF8Char *SDBTrim(UTF8Char *sqlstr, const UTF8Char *val, ServerType svrType);
		static UOSInt SDBTrimLeng(const UTF8Char *val, ServerType svrType);
		static DB::DBUtil::ColType ParseColType(ServerType svrType, const UTF8Char *typeName, UOSInt *colSize);
		static UTF8Char *ColTypeGetString(UTF8Char *sbuff, DB::DBUtil::ColType colType, UOSInt colSize);
	};
}
#endif
