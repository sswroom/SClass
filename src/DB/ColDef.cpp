#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "DB/ColDef.h"

DB::ColDef::ColDef(Text::CString colName)
{
	this->colName = Text::String::NewOrNull(colName);
	this->colType = DB::DBUtil::CT_Unknown;
	this->nativeType = 0;
	this->colSize = 0;
	this->notNull = false;
	this->pk = false;
	this->autoInc = false;
	this->defVal = 0;
	this->attr = 0;
}

DB::ColDef::ColDef(Text::String *colName)
{
	this->colName = SCOPY_STRING(colName);
	this->colType = DB::DBUtil::CT_Unknown;
	this->nativeType = 0;
	this->colSize = 0;
	this->notNull = false;
	this->pk = false;
	this->autoInc = false;
	this->defVal = 0;
	this->attr = 0;
}

DB::ColDef::~ColDef()
{
	SDEL_STRING(this->colName);
	SDEL_STRING(this->nativeType);
	SDEL_STRING(this->defVal);
	SDEL_STRING(this->attr);
}

Text::String *DB::ColDef::GetColName() const
{
	return this->colName;
}

DB::DBUtil::ColType DB::ColDef::GetColType() const
{
	return this->colType;
}

Text::String *DB::ColDef::GetNativeType() const
{
	return this->nativeType;
}

UOSInt DB::ColDef::GetColSize() const
{
	return this->colSize;
}

UOSInt DB::ColDef::GetColDP() const
{
	return this->colDP;
}

Bool DB::ColDef::IsNotNull() const
{
	return this->notNull;
}

Bool DB::ColDef::IsPK() const
{
	return this->pk;
}

Bool DB::ColDef::IsAutoInc() const
{
	return this->autoInc;
}

Text::String *DB::ColDef::GetDefVal() const
{
	return this->defVal;
}

Text::String *DB::ColDef::GetAttr() const
{
	return this->attr;
}

Bool DB::ColDef::GetDefVal(DB::SQLBuilder *sql) const
{
	if (this->defVal == 0)
		return false;
	if (this->defVal->StartsWith(UTF8STRC("b'")))
	{
		sql->AppendBool(this->defVal->Equals(UTF8STRC("b'1'")));
	}
	else
	{
		sql->AppendStr(this->defVal);
	}
	return true;
}


void DB::ColDef::SetColName(const UTF8Char *colName)
{
	SDEL_STRING(this->colName);
	if (colName) this->colName = Text::String::NewOrNullSlow(colName);
}

void DB::ColDef::SetColName(Text::CString colName)
{
	SDEL_STRING(this->colName);
	if (colName.v) this->colName = Text::String::New(colName.v, colName.leng);
}

void DB::ColDef::SetColName(Text::String *colName)
{
	SDEL_STRING(this->colName);
	if (colName) this->colName = colName->Clone();
}

void DB::ColDef::SetColType(DB::DBUtil::ColType colType)
{
	this->colType = colType;
}

void DB::ColDef::SetNativeType(Text::String *nativeType)
{
	SDEL_STRING(this->nativeType);
	this->nativeType = SCOPY_STRING(nativeType);
}

void DB::ColDef::SetNativeType(Text::CString nativeType)
{
	SDEL_STRING(this->nativeType);
	this->nativeType = Text::String::NewOrNull(nativeType);
}

void DB::ColDef::SetColSize(UOSInt colSize)
{
	this->colSize = colSize;
}

void DB::ColDef::SetColDP(UOSInt colDP)
{
	this->colDP = colDP;
}

void DB::ColDef::SetNotNull(Bool notNull)
{
	this->notNull = notNull;
}

void DB::ColDef::SetPK(Bool pk)
{
	this->pk = pk;
}

void DB::ColDef::SetAutoInc(Bool autoInc)
{
	this->autoInc = autoInc;
}

void DB::ColDef::SetDefVal(Text::CString defVal)
{
	SDEL_STRING(this->defVal);
	this->defVal = Text::String::NewOrNull(defVal);
}

void DB::ColDef::SetDefVal(Text::String *defVal)
{
	SDEL_STRING(this->defVal);
	if (defVal) this->defVal = defVal->Clone();
}

void DB::ColDef::SetAttr(Text::CString attr)
{
	SDEL_STRING(this->attr);
	this->attr = Text::String::NewOrNull(attr);
}

void DB::ColDef::SetAttr(Text::String *attr)
{
	SDEL_STRING(this->attr);
	if (attr) this->attr = attr->Clone();
}

void DB::ColDef::Set(const ColDef *colDef)
{
	this->SetColName(colDef->colName);
	this->SetColType(colDef->colType);
	this->SetNativeType(colDef->nativeType);
	this->SetColSize(colDef->colSize);
	this->SetColDP(colDef->colDP);
	this->SetNotNull(colDef->notNull);
	this->SetPK(colDef->pk);
	this->SetAutoInc(colDef->autoInc);
	this->SetDefVal(colDef->defVal);
	this->SetAttr(colDef->attr);
}

UTF8Char *DB::ColDef::ToColTypeStr(UTF8Char *sbuff) const
{
	return DB::DBUtil::ColTypeGetString(sbuff, this->colType, this->colSize);
}

DB::ColDef *DB::ColDef::Clone() const
{
	DB::ColDef *newObj;
	NEW_CLASS(newObj, DB::ColDef(this->colName));
	newObj->Set(this);
	return newObj;
}
