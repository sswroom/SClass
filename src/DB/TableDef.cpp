#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "DB/TableDef.h"

DB::TableDef::TableDef(const UTF8Char *tableName)
{
	this->databaseName = 0;
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
	SDEL_TEXT(this->databaseName);
	SDEL_TEXT(this->tableName);
	SDEL_TEXT(this->engine);
	SDEL_TEXT(this->charset);
	SDEL_TEXT(this->attr);
	SDEL_TEXT(this->comments);
	i = this->cols->GetCount();
	while (i-- > 0)
	{
		DEL_CLASS(this->cols->RemoveAt(i));
	}
	DEL_CLASS(this->cols);
	this->cols = 0;
}

const UTF8Char *DB::TableDef::GetDatabaseName()
{
	return this->databaseName;
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

DB::ColDef *DB::TableDef::GetSinglePKCol()
{
	DB::ColDef *retCol = 0;
	DB::ColDef *col;
	UOSInt i = 0;
	UOSInt j = this->cols->GetCount();
	while (i < j)
	{
		col = this->cols->GetItem(i);
		if (col->IsPK())
		{
			if (retCol != 0)
			{
				return 0;
			}
			retCol = col;
		}
		i++;
	}
	return retCol;
}

DB::TableDef *DB::TableDef::AddCol(DB::ColDef *col)
{
	this->cols->Add(col);
	return this;
}

DB::TableDef *DB::TableDef::SetDatabaseName(const UTF8Char *databaseName)
{
	SDEL_TEXT(this->databaseName);
	this->databaseName = SCOPY_TEXT(databaseName);
	return this;
}

DB::TableDef *DB::TableDef::SetTableName(const UTF8Char *tableName)
{
	SDEL_TEXT(this->tableName);
	this->tableName = SCOPY_TEXT(tableName);
	return this;
}

DB::TableDef *DB::TableDef::SetEngine(const UTF8Char *engine)
{
	SDEL_TEXT(this->engine);
	this->engine = SCOPY_TEXT(engine);
	return this;
}

DB::TableDef *DB::TableDef::SetCharset(const UTF8Char *charset)
{
	SDEL_TEXT(this->charset);
	this->charset = SCOPY_TEXT(charset);
	return this;
}

DB::TableDef *DB::TableDef::SetAttr(const UTF8Char *attr)
{
	SDEL_TEXT(this->attr);
	this->attr = SCOPY_TEXT(attr);
	return this;
}

DB::TableDef *DB::TableDef::SetComments(const UTF8Char *comments)
{
	SDEL_TEXT(this->comments)
	this->comments = SCOPY_TEXT(comments);
	return this;
}

DB::TableDef *DB::TableDef::SetSvrType(DB::DBUtil::ServerType svrType)
{
	this->svrType = svrType;
	return this;
}
