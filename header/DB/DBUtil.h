#ifndef _SM_DB_DBUTIL
#define _SM_DB_DBUTIL
#include "Data/ArrayList.h"
#include "Data/Class.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"
namespace DB
{
	enum class Charset
	{
		UTF8,
		UTF8MB4,
		Latin1
	};

	enum class Language
	{
		General,
		Swedish,
		Unicode0900
	};

	struct Collation
	{
		Charset charset;
		Language lang;
		Bool caseSensitive;
		Bool accentSensitive;
		Bool kanaSensitive;
		Bool widthSensitive;
	};

	enum class SQLType
	{
		Unknown,
		MSSQL,
		MySQL,
		Oracle,
		Access,
		SQLite,
		WBEM,
		MDBTools,
		PostgreSQL
	};

	class DBUtil
	{
	public:

		typedef enum
		{
			CT_Unknown,
			CT_UTF8Char,
			CT_UTF16Char,
			CT_UTF32Char,
			CT_VarUTF8Char,
			CT_VarUTF16Char,
			CT_VarUTF32Char,
			CT_Date,
			CT_DateTime,
			CT_DateTimeTZ,
			CT_Double,
			CT_Float,
			CT_Decimal,
			CT_Bool,
			CT_Byte,
			CT_Int16,
			CT_Int32,
			CT_Int64,
			CT_UInt16,
			CT_UInt32,
			CT_UInt64,
			CT_Binary,
			CT_Vector,
			CT_UUID
		} ColType;

		static UTF8Char *SDBStrUTF8(UTF8Char *sqlstr, const UTF8Char *str, SQLType sqlType);
		static UOSInt SDBStrUTF8Leng(const UTF8Char *str, SQLType sqlType);
		static UTF8Char *SDBStrW(UTF8Char *sqlstr, const WChar *str, SQLType sqlType);
		static UOSInt SDBStrWLeng(const WChar *str, SQLType sqlType);
		static UTF8Char *SDBInt32(UTF8Char *sqlstr, Int32 val, SQLType sqlType);
		static UOSInt SDBInt32Leng(Int32 val, SQLType sqlType);
		static UTF8Char *SDBInt64(UTF8Char *sqlstr, Int64 val, SQLType sqlType);
		static UOSInt SDBInt64Leng(Int64 val, SQLType sqlType);
		static UTF8Char *SDBUInt32(UTF8Char *sqlstr, UInt32 val, SQLType sqlType);
		static UOSInt SDBUInt32Leng(UInt32 val, SQLType sqlType);
		static UTF8Char *SDBUInt64(UTF8Char *sqlstr, UInt64 val, SQLType sqlType);
		static UOSInt SDBUInt64Leng(UInt64 val, SQLType sqlType);
		static UTF8Char *SDBDateTime(UTF8Char *sqlstr, Data::DateTime *dat, SQLType sqlType, Int8 tzQhr);
		static UOSInt SDBDateTimeLeng(Data::DateTime *dat, SQLType sqlType);
		static UTF8Char *SDBTS(UTF8Char *sqlstr, const Data::Timestamp &ts, SQLType sqlType, Int8 tzQhr);
		static UOSInt SDBTSLeng(const Data::Timestamp &ts, SQLType sqlType);
		static UTF8Char *SDBDate(UTF8Char *sqlstr, const Data::Date &d, SQLType sqlType);
		static UOSInt SDBDateLeng(const Data::Date &d, SQLType sqlType);
		static UTF8Char *SDBDbl(UTF8Char *sqlstr, Double val, SQLType sqlType);
		static UOSInt SDBDblLeng(Double val, SQLType sqlType);
		static UTF8Char *SDBSng(UTF8Char *sqlstr, Single val, SQLType sqlType);
		static UOSInt SDBSngLeng(Single val, SQLType sqlType);
		static UTF8Char *SDBBool(UTF8Char *sqlstr, Bool val, SQLType sqlType);
		static UOSInt SDBBoolLeng(Bool val, SQLType sqlType);
		static UTF8Char *SDBBin(UTF8Char *sqlstr, const UInt8 *buff, UOSInt size, SQLType sqlType);
		static UOSInt SDBBinLeng(const UInt8 *buff, UOSInt size, SQLType sqlType);
		static UTF8Char *SDBVector(UTF8Char *sqlstr, Math::Geometry::Vector2D *vec, SQLType sqlType, Bool axisAware);
		static UOSInt SDBVectorLeng(Math::Geometry::Vector2D *vec, SQLType sqlType);
		static UTF8Char *SDBColUTF8(UTF8Char *sqlstr, const UTF8Char *colName, SQLType sqlType);
		static UOSInt SDBColUTF8Leng(const UTF8Char *colName, SQLType sqlType);
		static UTF8Char *SDBColW(UTF8Char *sqlstr, const WChar *colName, SQLType sqlType);
		static UOSInt SDBColWLeng(const WChar *colName, SQLType sqlType);
		static UTF8Char *SDBTrim(UTF8Char *sqlstr, Text::CString val, SQLType sqlType);
		static UOSInt SDBTrimLeng(Text::CString val, SQLType sqlType);
		static DB::DBUtil::ColType ParseColType(SQLType sqlType, const UTF8Char *typeName, UOSInt *colSize, UOSInt *colDP);
		static UTF8Char *ColTypeGetString(UTF8Char *sbuff, DB::DBUtil::ColType colType, UOSInt colSize, UOSInt colDP);
		static UTF8Char *SDBCharset(UTF8Char *sqlstr, Charset charset, SQLType sqlType);
		static UTF8Char *SDBCollationName(UTF8Char *sqlstr, Charset charset, Language lang, SQLType sqlType, Bool *requireAS);
		static UTF8Char *SDBCollation(UTF8Char *sqlstr, const Collation *collation, SQLType sqlType);
		static Bool CollationParseMySQL(Text::CString collName, Collation *collation);

		static UTF8Char *DB2FieldName(UTF8Char *fieldNameBuff, const UTF8Char *dbName);
		static UTF8Char *Field2DBName(UTF8Char *dbNameBuff, const UTF8Char *fieldName);
		static Bool HasSchema(SQLType sqlType);
		template <class T> static Bool SaveCSV(IO::Stream *stm, Data::ArrayList<T*> *list, Data::Class *cls);
	};
}

template <class T> Bool DB::DBUtil::SaveCSV(IO::Stream *stm, Data::ArrayList<T*> *list, Data::Class *cls)
{
	UTF8Char sbuff[512];
	NotNullPtr<Text::String> s;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Bool succ = true;
	UOSInt i = 0;
	UOSInt j = cls->GetFieldCount();
	UOSInt k;
	UOSInt l;
	while (i < j)
	{
		if (i > 0)
		{
			sb.AppendUTF8Char(',');
		}
		DB::DBUtil::Field2DBName(sbuff, cls->GetFieldName(i)->v);
		s = Text::String::NewCSVRec(sbuff);
		sb.AppendC(s->v, s->leng);
		s->Release();
		i++;
	}
	sb.AppendC(UTF8STRC("\r\n"));
	if (stm->Write(sb.ToString(), sb.GetCharCnt()) != sb.GetCharCnt()) succ = false;

	Data::VariItem itm;
	k = 0;
	l = list->GetCount();
	while (k < l)
	{
		T *o = list->GetItem(k);
		sb.ClearStr();
		i = 0;
		j = cls->GetFieldCount();
		while (i < j)
		{
			if (i > 0)
			{
				sb.AppendUTF8Char(',');
			}
			cls->GetValue(itm, i, o);
			Data::VariItem::ItemType itmType = itm.GetItemType();
			if (itmType == Data::VariItem::ItemType::Null)
			{
				sb.AppendC(UTF8STRC("\"\""));
			}
			else if (itmType == Data::VariItem::ItemType::Str)
			{
				s = Text::String::NewCSVRec(itm.GetItemValue().str->v);
				sb.AppendC(s->v, s->leng);
				s->Release();
			}
			else
			{
				sb2.ClearStr();
				itm.GetAsString(sb2);
				s = Text::String::NewCSVRec(sb2.ToString());
				sb.AppendC(s->v, s->leng);
				s->Release();
			}
			i++;
		}
		sb.AppendC(UTF8STRC("\r\n"));
		if (stm->Write(sb.ToString(), sb.GetCharCnt()) != sb.GetCharCnt())
		{
			succ = false;
			break;
		}

		k++;
	}
	return succ;
}

#endif
