#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/ReadingDBTool.h"
#include "DB/SQLBuilder.h"
#include "DB/TableDef.h"
#include "Text/MyStringW.h"

DB::SQLBuilder::SQLBuilder(DB::DBUtil::ServerType svrType, Int32 tzQhr)
{
	this->svrType = svrType;
	this->tzQhr = tzQhr;
}

DB::SQLBuilder::SQLBuilder(DB::ReadingDBTool *db)
{
	this->svrType = db->GetSvrType();
	this->tzQhr = db->GetTzQhr();
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
	this->sb.AllocLeng(DB::DBUtil::SDBInt32Leng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBInt32(this->sb.GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendInt64(Int64 val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBInt64Leng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBInt64(this->sb.GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendUInt32(UInt32 val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBUInt32Leng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBUInt32(this->sb.GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendUInt64(UInt64 val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBUInt64Leng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBUInt64(this->sb.GetEndPtr(), val, this->svrType));
}
void DB::SQLBuilder::AppendStr(Text::String *val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(STR_PTR(val), this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), STR_PTR(val), this->svrType));
}

void DB::SQLBuilder::AppendStrC(Text::CString val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(val.v, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), val.v, this->svrType));
}

void DB::SQLBuilder::AppendStrUTF8(const UTF8Char *val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendStrW(const WChar *val)
{
	const UTF8Char *v = Text::StrToUTF8New(val);
	this->sb.AllocLeng(DB::DBUtil::SDBStrUTF8Leng(v, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBStrUTF8(this->sb.GetEndPtr(), v, this->svrType));
	Text::StrDelNew(v);
}

void DB::SQLBuilder::AppendDate(Data::DateTime *val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBDateLeng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBDate(this->sb.GetEndPtr(), val, this->svrType, (Int8)this->tzQhr));
}

void DB::SQLBuilder::AppendDbl(Double val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBDblLeng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBDbl(this->sb.GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendBool(Bool val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBBoolLeng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBBool(this->sb.GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendVector(Math::Geometry::Vector2D *vec)
{
	this->sb.AllocLeng(DB::DBUtil::SDBVectorLeng(vec, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBVector(this->sb.GetEndPtr(), vec, this->svrType));
}

void DB::SQLBuilder::AppendBinary(const UInt8 *buff, UOSInt buffSize)
{
	this->sb.AllocLeng(DB::DBUtil::SDBBinLeng(buff, buffSize, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBBin(this->sb.GetEndPtr(), buff, buffSize, this->svrType));
}

void DB::SQLBuilder::AppendTableName(DB::TableDef *table)
{
	Text::String *name;
	if ((name = table->GetDatabaseName()) != 0)
	{
		this->AppendCol(name->v);
		this->sb.AppendUTF8Char('.');
	}
	name = table->GetTableName();
	UOSInt i = name->IndexOf('.');
	if (i != INVALID_INDEX)
	{
		const UTF8Char *catalog = Text::StrCopyNewC(name->v, i);
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
	this->sb.AllocLeng(DB::DBUtil::SDBColUTF8Leng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBColUTF8(this->sb.GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendCol(const WChar *val)
{
	const UTF8Char *v = Text::StrToUTF8New(val);
	this->sb.AllocLeng(DB::DBUtil::SDBColUTF8Leng(v, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBColUTF8(this->sb.GetEndPtr(), v, this->svrType));
	Text::StrDelNew(v);
}

void DB::SQLBuilder::AppendTrim(Text::CString val)
{
	this->sb.AllocLeng(DB::DBUtil::SDBTrimLeng(val, this->svrType));
	this->sb.SetEndPtr(DB::DBUtil::SDBTrim(this->sb.GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::Clear()
{
	this->sb.ClearStr();
}

const UTF8Char *DB::SQLBuilder::ToString()
{
	return this->sb.ToString();
}

UOSInt DB::SQLBuilder::GetLength()
{
	return this->sb.GetLength();
}

Text::CString DB::SQLBuilder::ToCString()
{
	return this->sb.ToCString();
}

Text::String *DB::SQLBuilder::ToNewString()
{
	return Text::String::New(this->sb.ToString(), this->sb.GetLength());
}
