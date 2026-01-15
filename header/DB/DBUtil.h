#ifndef _SM_DB_DBUTIL
#define _SM_DB_DBUTIL
#include "Data/ArrayListNN.hpp"
#include "Data/NamedClass.hpp"
#include "Data/Class.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Math/Geometry/Vector2D.h"
#include "Text/JSONBuilder.h"
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

		static UnsafeArray<UTF8Char> SDBStrUTF8(UnsafeArray<UTF8Char> sqlstr, UnsafeArrayOpt<const UTF8Char> str, SQLType sqlType);
		static UIntOS SDBStrUTF8Leng(UnsafeArrayOpt<const UTF8Char> str, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBStrW(UnsafeArray<UTF8Char> sqlstr, UnsafeArrayOpt<const WChar> str, SQLType sqlType);
		static UIntOS SDBStrWLeng(UnsafeArrayOpt<const WChar> str, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBInt32(UnsafeArray<UTF8Char> sqlstr, Int32 val, SQLType sqlType);
		static UIntOS SDBInt32Leng(Int32 val, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBInt64(UnsafeArray<UTF8Char> sqlstr, Int64 val, SQLType sqlType);
		static UIntOS SDBInt64Leng(Int64 val, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBUInt32(UnsafeArray<UTF8Char> sqlstr, UInt32 val, SQLType sqlType);
		static UIntOS SDBUInt32Leng(UInt32 val, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBUInt64(UnsafeArray<UTF8Char> sqlstr, UInt64 val, SQLType sqlType);
		static UIntOS SDBUInt64Leng(UInt64 val, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBDateTime(UnsafeArray<UTF8Char> sqlstr, Optional<Data::DateTime> dat, SQLType sqlType, Int8 tzQhr);
		static UIntOS SDBDateTimeLeng(Optional<Data::DateTime> dat, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBTS(UnsafeArray<UTF8Char> sqlstr, const Data::Timestamp &ts, SQLType sqlType, Int8 tzQhr);
		static UIntOS SDBTSLeng(const Data::Timestamp &ts, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBDate(UnsafeArray<UTF8Char> sqlstr, const Data::Date &d, SQLType sqlType);
		static UIntOS SDBDateLeng(const Data::Date &d, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBDbl(UnsafeArray<UTF8Char> sqlstr, Double val, SQLType sqlType);
		static UIntOS SDBDblLeng(Double val, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBSng(UnsafeArray<UTF8Char> sqlstr, Single val, SQLType sqlType);
		static UIntOS SDBSngLeng(Single val, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBBool(UnsafeArray<UTF8Char> sqlstr, Bool val, SQLType sqlType);
		static UIntOS SDBBoolLeng(Bool val, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBBin(UnsafeArray<UTF8Char> sqlstr, UnsafeArrayOpt<const UInt8> buff, UIntOS size, SQLType sqlType);
		static UIntOS SDBBinLeng(UnsafeArrayOpt<const UInt8> buff, UIntOS size, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBVector(UnsafeArray<UTF8Char> sqlstr, Optional<Math::Geometry::Vector2D> vec, SQLType sqlType, Bool axisAware);
		static UIntOS SDBVectorLeng(Optional<Math::Geometry::Vector2D> vec, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBColUTF8(UnsafeArray<UTF8Char> sqlstr, UnsafeArray<const UTF8Char> colName, SQLType sqlType);
		static UIntOS SDBColUTF8Leng(UnsafeArray<const UTF8Char> colName, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBColW(UnsafeArray<UTF8Char> sqlstr, UnsafeArray<const WChar> colName, SQLType sqlType);
		static UIntOS SDBColWLeng(UnsafeArray<const WChar> colName, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBTrim(UnsafeArray<UTF8Char> sqlstr, Text::CStringNN val, SQLType sqlType);
		static UIntOS SDBTrimLeng(Text::CStringNN val, SQLType sqlType);
		static DB::DBUtil::ColType ParseColType(SQLType sqlType, UnsafeArray<const UTF8Char> typeName, InOutParam<UIntOS> colSize, InOutParam<UIntOS> colDP);
		static UnsafeArray<UTF8Char> ColTypeGetString(UnsafeArray<UTF8Char> sbuff, DB::DBUtil::ColType colType, UIntOS colSize, UIntOS colDP);
		static UnsafeArray<UTF8Char> SDBCharset(UnsafeArray<UTF8Char> sqlstr, Charset charset, SQLType sqlType);
		static UnsafeArray<UTF8Char> SDBCollationName(UnsafeArray<UTF8Char> sqlstr, Charset charset, Language lang, SQLType sqlType, OutParam<Bool> requireAS);
		static UnsafeArray<UTF8Char> SDBCollation(UnsafeArray<UTF8Char> sqlstr, NN<const Collation> collation, SQLType sqlType);
		static Bool CollationParseMySQL(Text::CStringNN collName, NN<Collation> collation);

		static UnsafeArray<UTF8Char> DB2FieldName(UnsafeArray<UTF8Char> fieldNameBuff, UnsafeArray<const UTF8Char> dbName);
		static UnsafeArray<UTF8Char> Field2DBName(UnsafeArray<UTF8Char> dbNameBuff, Optional<Text::String> fieldName);
		static UnsafeArray<UTF8Char> Field2DBName(UnsafeArray<UTF8Char> dbNameBuff, UnsafeArray<const UTF8Char> fieldName);
		static Bool HasSchema(SQLType sqlType);
		static Bool IsNo3DGeometry(SQLType sqlType);
		template <class T> static Bool SaveCSV(NN<IO::Stream> stm, NN<Data::ArrayListNN<T>> list, NN<Data::Class> cls);
		template <typename T> static Bool Model2JSON(NN<Text::JSONBuilder> json, NN<T> obj);
	};

	Text::CStringNN SQLTypeGetName(SQLType val);
}

template <class T> Bool DB::DBUtil::SaveCSV(NN<IO::Stream> stm, NN<Data::ArrayListNN<T>> list, NN<Data::Class> cls)
{
	UTF8Char sbuff[512];
	NN<Text::String> s;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sb2;
	Bool succ = true;
	UIntOS i = 0;
	UIntOS j = cls->GetFieldCount();
	UIntOS k;
	UIntOS l;
	while (i < j)
	{
		if (i > 0)
		{
			sb.AppendUTF8Char(',');
		}
		DB::DBUtil::Field2DBName(sbuff, cls->GetFieldName(i));
		s = Text::String::NewCSVRec(sbuff);
		sb.AppendC(s->v, s->leng);
		s->Release();
		i++;
	}
	sb.AppendC(UTF8STRC("\r\n"));
	if (stm->Write(sb.ToByteArray()) != sb.GetCharCnt()) succ = false;

	Data::VariItem itm;
	k = 0;
	l = list->GetCount();
	while (k < l)
	{
		NN<T> o = list->GetItemNoCheck(k);
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
		if (stm->Write(sb.ToByteArray()) != sb.GetCharCnt())
		{
			succ = false;
			break;
		}

		k++;
	}
	return succ;
}

template <typename T> Bool DB::DBUtil::Model2JSON(NN<Text::JSONBuilder> json, NN<T> obj)
{
	NN<Data::NamedClass<T>> cls = obj->CreateClass();
	NN<Text::String> name;
	Data::VariItem item;
	UIntOS i = 0;
	UIntOS j = cls->GetFieldCount();
	while (i < j)
	{
		if (cls->GetFieldName(i).SetTo(name))
		{
			cls->GetValue(item, i, obj.Ptr());
			json->ObjectAddVarItem(name->ToCString(), item);
		}
		i++;
	}
	cls.Delete();
	return true;
}

#endif
