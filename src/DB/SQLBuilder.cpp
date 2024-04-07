#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ReadingDBTool.h"
#include "DB/SQLBuilder.h"
#include "DB/TableDef.h"
#include "Text/MyStringW.h"

DB::SQLBuilder::SQLBuilder(DB::SQLType sqlType, Bool axisAware, Int32 tzQhr)
{
	this->sqlType = sqlType;
	this->tzQhr = tzQhr;
	this->axisAware = axisAware;
}

DB::SQLBuilder::SQLBuilder(NotNullPtr<const DB::ReadingDBTool> db)
{
	this->sqlType = db->GetSQLType();
	this->tzQhr = db->GetTzQhr();
	this->axisAware = db->IsAxisAware();
}

DB::SQLBuilder::~SQLBuilder()
{
}

void DB::SQLBuilder::AppendCmdSlow(const UTF8Char *val)
{
	this->sb.AppendSlow(val);
}

void DB::SQLBuilder::AppendCmdC(Text::CString val)
{
	this->sb.Append(val);
}

void DB::SQLBuilder::AppendInt32(Int32 val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBInt32Leng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBInt32(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::AppendInt64(Int64 val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBInt64Leng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBInt64(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::AppendUInt32(UInt32 val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBUInt32Leng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBUInt32(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::AppendUInt64(UInt64 val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBUInt64Leng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBUInt64(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::AppendStr(Text::String *val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(STR_PTR(val), this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), STR_PTR(val), this->sqlType));
}

void DB::SQLBuilder::AppendStr(Optional<Text::String> val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(OPTSTR_CSTR(val).v, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), OPTSTR_CSTR(val).v, this->sqlType));
}

void DB::SQLBuilder::AppendStr(NotNullPtr<Text::String> val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(val->v, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), val->v, this->sqlType));
}

void DB::SQLBuilder::AppendStrC(Text::CString val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(val.v, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), val.v, this->sqlType));
}

void DB::SQLBuilder::AppendStrUTF8(const UTF8Char *val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::AppendStrW(const WChar *val)
{
	const UTF8Char *v = Text::StrToUTF8New(val);
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(v, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), v, this->sqlType));
	Text::StrDelNew(v);
}

void DB::SQLBuilder::AppendDateTime(Data::DateTime *val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBDateTimeLeng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBDateTime(this->sb.GetEndPtr(), val, this->sqlType, (Int8)this->tzQhr));
}

void DB::SQLBuilder::AppendTS(const Data::Timestamp &val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBTSLeng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBTS(this->sb.GetEndPtr(), val, this->sqlType, (Int8)this->tzQhr));
}

void DB::SQLBuilder::AppendDate(const Data::Date &val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBDateLeng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBDate(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::AppendDbl(Double val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBDblLeng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBDbl(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::AppendBool(Bool val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBBoolLeng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBBool(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::AppendVector(Optional<Math::Geometry::Vector2D> vec)
{
	this->sb.AllocLeng(DB::DBUtil::SDBVectorLeng(vec, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBVector(this->sb.GetEndPtr(), vec, this->sqlType, this->axisAware));
}

void DB::SQLBuilder::AppendBinary(const UInt8 *buff, UOSInt buffSize)
{
	this->sb.AllocLeng(DB::DBUtil::SDBBinLeng(buff, buffSize, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBBin(this->sb.GetEndPtr(), buff, buffSize, this->sqlType));
}

void DB::SQLBuilder::AppendTableName(NotNullPtr<DB::TableDef> table)
{
	NotNullPtr<Text::String> name;
	if (table->GetDatabaseName().SetTo(name))
	{
		this->AppendCol(name->v);
		this->sb.AppendUTF8Char('.');
	}
	name = table->GetTableName();
	UOSInt i = name->IndexOf('.');
	if (i != INVALID_INDEX)
	{
		const UTF8Char *catalog = Text::StrCopyNewC(name->v, i).Ptr();
		this->AppendCol(catalog);
		this->sb.AppendUTF8Char('.');
		Text::StrDelNew(catalog);
		this->AppendCol(name->v + i + 1);
	}
	else
	{
		this->AppendCol(name->v);
	}
}

void DB::SQLBuilder::AppendCol(const UTF8Char *val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBColUTF8Leng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBColUTF8(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::AppendCol(const WChar *val)
{
	const UTF8Char *v = Text::StrToUTF8New(val);
	this->sb.AllocLeng(DB::DBUtil::SDBColUTF8Leng(v, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBColUTF8(this->sb.GetEndPtr(), v, this->sqlType));
	Text::StrDelNew(v);
}

void DB::SQLBuilder::AppendTrim(Text::CString val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBTrimLeng(val, this->sqlType));
	this->sb.SetEndPtr(DB::DBUtil::SDBTrim(this->sb.GetEndPtr(), val, this->sqlType));
}

void DB::SQLBuilder::Clear()
{
	this->sb.ClearStr();
}

const UTF8Char *DB::SQLBuilder::ToString() const
{
	return this->sb.ToString();
}

UOSInt DB::SQLBuilder::GetLength() const
{
	return this->sb.GetLength();
}

Text::CStringNN DB::SQLBuilder::ToCString() const
{
	return this->sb.ToCString();
}

NotNullPtr<Text::String> DB::SQLBuilder::ToNewString() const
{
	return Text::String::New(this->sb.ToCString());
}

DB::SQLType DB::SQLBuilder::GetSQLType() const
{
	return this->sqlType;
}

Bool DB::SQLBuilder::SupportSchema() const
{
	return this->sqlType == DB::SQLType::MSSQL || this->sqlType == DB::SQLType::PostgreSQL;
}
