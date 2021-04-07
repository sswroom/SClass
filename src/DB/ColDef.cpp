#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "DB/ColDef.h"

DB::ColDef::ColDef(const UTF8Char *colName)
{
	if (colName)
	{
		this->colName = Text::StrCopyNew(colName);
	}
	else
	{
		this->colName = 0;
	}
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
	if (this->colName)
	{
		Text::StrDelNew(this->colName);
		this->colName = 0;
	}
	if (this->defVal)
	{
		Text::StrDelNew(this->defVal);
		this->defVal = 0;
	}
	if (this->attr)
	{
		Text::StrDelNew(this->attr);
		this->attr = 0;
	}
}

const UTF8Char *DB::ColDef::GetColName()
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

const UTF8Char *DB::ColDef::GetDefVal()
{
	return this->defVal;
}

const UTF8Char *DB::ColDef::GetAttr()
{
	return this->attr;
}

Bool DB::ColDef::GetDefVal(DB::SQLBuilder *sql)
{
	if (this->defVal == 0)
		return false;
	if (Text::StrStartsWith(this->defVal, (const UTF8Char*)"b'"))
	{
		sql->AppendBool(Text::StrEquals(this->defVal, (const UTF8Char*)"b'1'"));
	}
	else
	{
		sql->AppendStrUTF8(this->defVal);
	}
	return true;
}


void DB::ColDef::SetColName(const UTF8Char *colName)
{
	if (this->colName)
	{
		Text::StrDelNew(this->colName);
	}
	this->colName = SCOPY_TEXT(colName);
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
	SDEL_TEXT(this->defVal);
	this->defVal = SCOPY_TEXT(defVal);
}

void DB::ColDef::SetAttr(const UTF8Char *attr)
{
	SDEL_TEXT(this->attr);
	this->attr = SCOPY_TEXT(attr);
}

UTF8Char *DB::ColDef::ToColTypeStr(UTF8Char *sbuff)
{
	return DB::DBUtil::ColTypeGetString(sbuff, this->colType, this->colSize);
}

DB::ColDef *DB::ColDef::Clone()
{
	DB::ColDef *newObj;
	NEW_CLASS(newObj, DB::ColDef(this->colName));
	newObj->SetColType(this->colType);
	newObj->SetColSize(this->colSize);
	newObj->SetColDP(this->colDP);
	newObj->SetNotNull(this->notNull);
	newObj->SetPK(this->pk);
	newObj->SetAutoInc(this->autoInc);
	newObj->SetDefVal(this->defVal);
	newObj->SetAttr(this->attr);
	return newObj;
}
