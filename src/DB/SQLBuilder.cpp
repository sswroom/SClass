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
	NEW_CLASS(sb, Text::StringBuilderUTF8());
}

DB::SQLBuilder::SQLBuilder(DB::ReadingDBTool *db)
{
	this->svrType = db->GetSvrType();
	this->tzQhr = db->GetTzQhr();
	NEW_CLASS(sb, Text::StringBuilderUTF8());
}

DB::SQLBuilder::~SQLBuilder()
{
	DEL_CLASS(sb);
}

void DB::SQLBuilder::AppendCmd(const UTF8Char *val)
{
	sb->Append(val);
}

void DB::SQLBuilder::AppendInt32(Int32 val)
{
	sb->AllocLeng(DB::DBUtil::SDBInt32Leng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBInt32(sb->GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendInt64(Int64 val)
{
	sb->AllocLeng(DB::DBUtil::SDBInt64Leng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBInt64(sb->GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendUInt32(UInt32 val)
{
	sb->AllocLeng(DB::DBUtil::SDBUInt32Leng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBUInt32(sb->GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendUInt64(UInt64 val)
{
	sb->AllocLeng(DB::DBUtil::SDBUInt64Leng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBUInt64(sb->GetEndPtr(), val, this->svrType));
}
void DB::SQLBuilder::AppendStr(Text::String *val)
{
	sb->AllocLeng(DB::DBUtil::SDBStrUTF8Leng(STR_PTR(val), this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBStrUTF8(sb->GetEndPtr(), STR_PTR(val), this->svrType));
}

void DB::SQLBuilder::AppendStrUTF8(const UTF8Char *val)
{
	sb->AllocLeng(DB::DBUtil::SDBStrUTF8Leng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBStrUTF8(sb->GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendStrW(const WChar *val)
{
	const UTF8Char *v = Text::StrToUTF8New(val);
	sb->AllocLeng(DB::DBUtil::SDBStrUTF8Leng(v, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBStrUTF8(sb->GetEndPtr(), v, this->svrType));
	Text::StrDelNew(v);
}

void DB::SQLBuilder::AppendDate(Data::DateTime *val)
{
	sb->AllocLeng(DB::DBUtil::SDBDateLeng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBDate(sb->GetEndPtr(), val, this->svrType, (Int8)this->tzQhr));
}

void DB::SQLBuilder::AppendDbl(Double val)
{
	sb->AllocLeng(DB::DBUtil::SDBDblLeng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBDbl(sb->GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendBool(Bool val)
{
	sb->AllocLeng(DB::DBUtil::SDBBoolLeng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBBool(sb->GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendVector(Math::Vector2D *vec)
{
	sb->AllocLeng(DB::DBUtil::SDBVectorLeng(vec, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBVector(sb->GetEndPtr(), vec, this->svrType));
}

void DB::SQLBuilder::AppendBinary(const UInt8 *buff, UOSInt buffSize)
{
	sb->AllocLeng(DB::DBUtil::SDBBinLeng(buff, buffSize, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBBin(sb->GetEndPtr(), buff, buffSize, this->svrType));
}

void DB::SQLBuilder::AppendTableName(DB::TableDef *table)
{
	const UTF8Char *name;
	if ((name = table->GetDatabaseName()) != 0)
	{
		this->AppendCol(name);
		sb->AppendChar('.', 1);
	}
	name = table->GetTableName();
	UOSInt i = Text::StrIndexOf(name, '.');
	if (i != INVALID_INDEX)
	{
		const UTF8Char *catalog = Text::StrCopyNewC(name, i);
		this->AppendCol(catalog);
		sb->AppendChar('.', 1);
		Text::StrDelNew(catalog);
		this->AppendCol(name + i + 1);
	}
	else
	{
		this->AppendCol(name);
	}
}

void DB::SQLBuilder::AppendCol(const UTF8Char *val)
{
	sb->AllocLeng(DB::DBUtil::SDBColUTF8Leng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBColUTF8(sb->GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::AppendCol(const WChar *val)
{
	const UTF8Char *v = Text::StrToUTF8New(val);
	sb->AllocLeng(DB::DBUtil::SDBColUTF8Leng(v, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBColUTF8(sb->GetEndPtr(), v, this->svrType));
	Text::StrDelNew(v);
}

void DB::SQLBuilder::AppendTrim(const UTF8Char *val)
{
	sb->AllocLeng(DB::DBUtil::SDBTrimLeng(val, this->svrType));
	sb->SetEndPtr(DB::DBUtil::SDBTrim(sb->GetEndPtr(), val, this->svrType));
}

void DB::SQLBuilder::Clear()
{
	sb->ClearStr();
}

const UTF8Char *DB::SQLBuilder::ToString()
{
	return sb->ToString();
}

UOSInt DB::SQLBuilder::GetLength()
{
	return sb->GetLength();
}

Text::String *DB::SQLBuilder::ToNewString()
{
	return Text::String::New(sb->ToString(), sb->GetLength());
}
