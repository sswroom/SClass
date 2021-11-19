#ifndef _SM_DB_DBUTIL
#define _SM_DB_DBUTIL
#include "Data/ArrayList.h"
#include "Data/Class.h"
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Math/Vector2D.h"
#include "Text/StringBuilderUTF8.h"
namespace DB
{
	class DBUtil
	{
	public:
		enum class ServerType
		{
			Unknown,
			MSSQL,
			MySQL,
			Oracle,
			Access,
			Text,
			SQLite,
			WBEM,
			MDBTools
		};

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
			CT_DateTime2, //acc = 0.1ms
			CT_UUID
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

		static UTF8Char *DB2FieldName(UTF8Char *fieldNameBuff, const UTF8Char *dbName);
		static UTF8Char *Field2DBName(UTF8Char *dbNameBuff, const UTF8Char *fieldName);
		template <class T> static Bool SaveCSV(IO::Stream *stm, Data::ArrayList<T*> *list, Data::Class *cls);
	};
}

template <class T> Bool DB::DBUtil::SaveCSV(IO::Stream *stm, Data::ArrayList<T*> *list, Data::Class *cls)
{
	UTF8Char sbuff[512];
	const UTF8Char *csptr;
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
			sb.AppendChar(',', 1);
		}
		DB::DBUtil::Field2DBName(sbuff, cls->GetFieldName(i));
		csptr = Text::StrToNewCSVRec(sbuff);
		sb.Append(csptr);
		Text::StrDelNew(csptr);
		i++;
	}
	sb.Append((const UTF8Char*)"\r\n");
	if (stm->Write(sb.ToString(), sb.GetCharCnt()) != sb.GetCharCnt()) succ = false;

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
				sb.AppendChar(',', 1);
			}
			Data::VariItem *itm = cls->GetNewValue(i, o);
			if (itm->GetItemType() == Data::VariItem::ItemType::Null)
			{
				sb.Append((const UTF8Char*)"\"\"");
			}
			else
			{
				sb2.ClearStr();
				itm->GetAsString(&sb2);
				csptr = Text::StrToNewCSVRec(sb2.ToString());
				sb.Append(csptr);
				Text::StrDelNew(csptr);
			}
			DEL_CLASS(itm);
			i++;
		}
		sb.Append((const UTF8Char*)"\r\n");
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
