#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBClassBuilder.h"
#include "DB/TableDef.h"
#include "Text/MyString.h"

DB::TableDef::TableDef(const UTF8Char *tableName)
{
	this->databaseName = 0;
	this->tableName = Text::StrCopyNew(tableName);
	this->engine = 0;
	this->charset = 0;
	this->attr = 0;
	this->comments = 0;
	this->svrType = DB::DBUtil::ServerType::Unknown;
	NEW_CLASS(this->cols, Data::ArrayList<DB::ColDef*>());
}

DB::TableDef::~TableDef()
{
	UOSInt i;
	SDEL_TEXT(this->databaseName);
	SDEL_TEXT(this->tableName);
	SDEL_STRING(this->engine);
	SDEL_STRING(this->charset);
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

Text::String *DB::TableDef::GetEngine()
{
	return this->engine;
}

Text::String *DB::TableDef::GetCharset()
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

DB::TableDef *DB::TableDef::SetEngine(Text::CString engine)
{
	SDEL_STRING(this->engine);
	this->engine = Text::String::NewOrNull(engine);
	return this;
}

DB::TableDef *DB::TableDef::SetCharset(Text::CString charset)
{
	SDEL_STRING(this->charset);
	this->charset = Text::String::NewOrNull(charset);
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

DB::TableDef *DB::TableDef::Clone()
{
	DB::TableDef *newObj;
	NEW_CLASS(newObj, DB::TableDef(this->tableName));
	newObj->SetDatabaseName(this->databaseName);
	newObj->SetEngine(STR_CSTR(this->engine));
	newObj->SetCharset(STR_CSTR(this->charset));
	newObj->SetAttr(this->attr);
	newObj->SetComments(this->comments);
	newObj->SetSvrType(this->svrType);
	UOSInt i = 0;
	UOSInt j = this->cols->GetCount();
	while (i < j)
	{
		newObj->AddCol(this->cols->GetItem(i)->Clone());
		i++;
	}
	return newObj;
}

Data::Class *DB::TableDef::CreateTableClass()
{
	DB::DBClassBuilder builder;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->cols->GetCount();
	while (i < j)
	{
		DB::ColDef *col = this->cols->GetItem(i);
		builder.AddItem(col->GetColName()->v, col->GetColType());
		i++;
	}
	return builder.GetResultClass();
}
