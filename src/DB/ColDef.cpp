#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "DB/ColDef.h"

DB::ColDef::ColDef(const UTF8Char *colName)
{
	this->colName = Text::String::NewOrNull(colName);
	this->colType = DB::DBUtil::CT_Unknown;
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
	SDEL_STRING(this->defVal);
	SDEL_STRING(this->attr);
}

Text::String *DB::ColDef::GetColName()
{
	return this->colName;
}

DB::DBUtil::ColType DB::ColDef::GetColType()
{
	return this->colType;
}

UOSInt DB::ColDef::GetColSize()
{
	return this->colSize;
}

UOSInt DB::ColDef::GetColDP()
{
	return this->colDP;
}

Bool DB::ColDef::IsNotNull()
{
	return this->notNull;
}

Bool DB::ColDef::IsPK()
{
	return this->pk;
}

Bool DB::ColDef::IsAutoInc()
{
	return this->autoInc;
}

Text::String *DB::ColDef::GetDefVal()
{
	return this->defVal;
}

Text::String *DB::ColDef::GetAttr()
{
	return this->attr;
}

Bool DB::ColDef::GetDefVal(DB::SQLBuilder *sql)
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
	if (colName) this->colName = Text::String::NewOrNull(colName);
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

void DB::ColDef::SetDefVal(const UTF8Char *defVal)
{
	SDEL_STRING(this->defVal);
	if (defVal) this->defVal = Text::String::NewOrNull(defVal);
}

void DB::ColDef::SetDefVal(Text::String *defVal)
{
	SDEL_STRING(this->defVal);
	if (defVal) this->defVal = defVal->Clone();
}

void DB::ColDef::SetAttr(const UTF8Char *attr)
{
	SDEL_STRING(this->attr);
	if (attr) this->attr = Text::String::NewOrNull(attr);
}

void DB::ColDef::SetAttr(Text::String *attr)
{
	SDEL_STRING(this->attr);
	if (attr) this->attr = attr->Clone();
}

void DB::ColDef::Set(ColDef *colDef)
{
	this->SetColName(colDef->colName);
	this->SetColType(colDef->colType);
	this->SetColSize(colDef->colSize);
	this->SetColDP(colDef->colDP);
	this->SetNotNull(colDef->notNull);
	this->SetPK(colDef->pk);
	this->SetAutoInc(colDef->autoInc);
	this->SetDefVal(colDef->defVal);
	this->SetAttr(colDef->attr);
}

UTF8Char *DB::ColDef::ToColTypeStr(UTF8Char *sbuff)
{
	return DB::DBUtil::ColTypeGetString(sbuff, this->colType, this->colSize);
}

DB::ColDef *DB::ColDef::Clone()
{
	DB::ColDef *newObj;
	NEW_CLASS(newObj, DB::ColDef(this->colName));
	newObj->Set(this);
	return newObj;
}
