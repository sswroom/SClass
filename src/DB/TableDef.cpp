#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "DB/TableDef.h"

DB::TableDef::TableDef(const UTF8Char *tableName)
{
	this->tableName = Text::StrCopyNew(tableName);
	this->engine = 0;
	this->charset = 0;
	this->attr = 0;
	this->comments = 0;
	this->svrType = DB::DBUtil::SVR_TYPE_UNKNOWN;
	NEW_CLASS(this->cols, Data::ArrayList<DB::ColDef*>());
}

DB::TableDef::~TableDef()
{
	OSInt i;
	if (this->tableName)
	{
		Text::StrDelNew(this->tableName);
		this->tableName = 0;
	}
	if (this->engine)
	{
		Text::StrDelNew(this->engine);
		this->engine = 0;
	}
	if (this->charset)
	{
		Text::StrDelNew(this->charset);
		this->charset = 0;
	}
	if (this->attr)
	{
		Text::StrDelNew(this->attr);
		this->attr = 0;
	}
	if (this->comments)
	{
		Text::StrDelNew(this->comments);
		this->comments = 0;
	}
	i = this->cols->GetCount();
	while (i-- > 0)
	{
		DEL_CLASS(this->cols->RemoveAt(i));
	}
	DEL_CLASS(this->cols);
	this->cols = 0;
}

const UTF8Char *DB::TableDef::GetTableName()
{
	return this->tableName;
}

const UTF8Char *DB::TableDef::GetEngine()
{
	return this->engine;
}

const UTF8Char *DB::TableDef::GetCharset()
{
	return this->charset;
}

const UTF8Char *DB::TableDef::GetAttr()
{
	return this->attr;
}

const UTF8Char *DB::TableDef::GetComments()
{
	return this->comments;
}

DB::DBUtil::ServerType DB::TableDef::GetSvrType()
{
	return this->svrType;
}

UOSInt DB::TableDef::GetColCnt()
{
	return this->cols->GetCount();
}

DB::ColDef *DB::TableDef::GetCol(UOSInt index)
{
	return this->cols->GetItem(index);
}

void DB::TableDef::AddCol(DB::ColDef *col)
{
	this->cols->Add(col);
}

void DB::TableDef::SetTableName(const UTF8Char *tableName)
{
	if (this->tableName)
	{
		Text::StrDelNew(this->tableName);
	}
	if (tableName)
	{
		this->tableName = Text::StrCopyNew(tableName);
	}
	else
	{
		this->tableName = 0;
	}
}

void DB::TableDef::SetEngine(const UTF8Char *engine)
{
	if (this->engine)
	{
		Text::StrDelNew(this->engine);
	}
	if (engine)
	{
		this->engine = Text::StrCopyNew(engine);
	}
	else
	{
		this->engine = 0;
	}
}

void DB::TableDef::SetCharset(const UTF8Char *charset)
{
	if (this->charset)
	{
		Text::StrDelNew(this->charset);
	}
	if (charset)
	{
		this->charset = Text::StrCopyNew(charset);
	}
	else
	{
		this->charset = 0;
	}
}

void DB::TableDef::SetAttr(const UTF8Char *attr)
{
	if (this->attr)
	{
		Text::StrDelNew(this->attr);
	}
	if (attr)
	{
		this->attr = Text::StrCopyNew(attr);
	}
	else
	{
		this->attr = 0;
	}
}

void DB::TableDef::SetComments(const UTF8Char *comments)
{
	if (this->comments)
	{
		Text::StrDelNew(this->comments);
	}
	if (comments)
	{
		this->comments = Text::StrCopyNew(comments);
	}
	else
	{
		this->comments = 0;
	}
}

void DB::TableDef::SetSvrType(DB::DBUtil::ServerType svrType)
{
	this->svrType = svrType;
}
