#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBClassBuilder.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "Text/MyString.h"

DB::TableDef::TableDef(Text::CString schemaName, Text::CString tableName)
{
	this->databaseName = 0;
	this->schemaName = Text::String::NewOrNull(schemaName);
	this->tableName = Text::String::New(tableName);
	this->engine = 0;
	this->charset = 0;
	this->attr = 0;
	this->comments = 0;
	this->sqlType = DB::SQLType::Unknown;
}

DB::TableDef::~TableDef()
{
	UOSInt i;
	SDEL_STRING(this->databaseName);
	SDEL_STRING(this->schemaName);
	this->tableName->Release();
	SDEL_STRING(this->engine);
	SDEL_STRING(this->charset);
	SDEL_TEXT(this->attr);
	SDEL_TEXT(this->comments);
	i = this->cols.GetCount();
	while (i-- > 0)
	{
		DEL_CLASS(this->cols.RemoveAt(i));
	}
}

Text::String *DB::TableDef::GetDatabaseName() const
{
	return this->databaseName;
}

Text::String *DB::TableDef::GetSchemaName() const
{
	return this->schemaName;
}

Text::String *DB::TableDef::GetTableName() const
{
	return this->tableName.Ptr();
}

Text::String *DB::TableDef::GetEngine() const
{
	return this->engine;
}

Text::String *DB::TableDef::GetCharset() const
{
	return this->charset;
}

const UTF8Char *DB::TableDef::GetAttr() const
{
	return this->attr;
}

const UTF8Char *DB::TableDef::GetComments() const
{
	return this->comments;
}

DB::SQLType DB::TableDef::GetSQLType() const
{
	return this->sqlType;
}

UOSInt DB::TableDef::GetColCnt() const
{
	return this->cols.GetCount();
}

DB::ColDef *DB::TableDef::GetCol(UOSInt index) const
{
	return this->cols.GetItem(index);
}

DB::ColDef *DB::TableDef::GetSinglePKCol() const
{
	DB::ColDef *retCol = 0;
	DB::ColDef *col;
	UOSInt i = 0;
	UOSInt j = this->cols.GetCount();
	while (i < j)
	{
		col = this->cols.GetItem(i);
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

UOSInt DB::TableDef::CountPK() const
{
	UOSInt cnt = 0;
	UOSInt i = this->cols.GetCount();
	while (i-- > 0)
	{
		if (this->cols.GetItem(i)->IsPK())
			cnt++;
	}
	return cnt;
}

DB::TableDef *DB::TableDef::AddCol(NotNullPtr<DB::ColDef> col)
{
	this->cols.Add(col);
	return this;
}

DB::TableDef *DB::TableDef::SetDatabaseName(Text::CString databaseName)
{
	SDEL_STRING(this->databaseName);
	this->databaseName = Text::String::NewOrNull(databaseName);
	return this;
}

DB::TableDef *DB::TableDef::SetSchemaName(Text::CString schemaName)
{
	SDEL_STRING(this->schemaName);
	this->schemaName = Text::String::NewOrNull(schemaName);
	return this;
}

DB::TableDef *DB::TableDef::SetTableName(Text::CString tableName)
{
	this->tableName->Release();
	this->tableName = Text::String::New(tableName);
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

DB::TableDef *DB::TableDef::SetSQLType(DB::SQLType sqlType)
{
	this->sqlType = sqlType;
	return this;
}

void DB::TableDef::ColFromReader(NotNullPtr<DB::DBReader> r)
{
	UOSInt i = 0;
	UOSInt j = r->ColCount();
	NotNullPtr<DB::ColDef> col;
	while (i < j)
	{
		NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
		r->GetColDef(i, col);
		this->AddCol(col);
		i++;
	}
}

NotNullPtr<DB::TableDef> DB::TableDef::Clone() const
{
	NotNullPtr<DB::TableDef> newObj;
	NEW_CLASSNN(newObj, DB::TableDef(STR_CSTR(this->schemaName), this->tableName->ToCString()));
	newObj->SetDatabaseName(STR_CSTR(this->databaseName));
	newObj->SetEngine(STR_CSTR(this->engine));
	newObj->SetCharset(STR_CSTR(this->charset));
	newObj->SetAttr(this->attr);
	newObj->SetComments(this->comments);
	newObj->SetSQLType(this->sqlType);
	UOSInt i = 0;
	UOSInt j = this->cols.GetCount();
	while (i < j)
	{
		newObj->AddCol(this->cols.GetItem(i)->Clone());
		i++;
	}
	return newObj;
}

NotNullPtr<Data::Class> DB::TableDef::CreateTableClass() const
{
	DB::DBClassBuilder builder;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = this->cols.GetCount();
	while (i < j)
	{
		DB::ColDef *col = this->cols.GetItem(i);
		builder.AddItem(col->GetColName()->v, col->GetColType());
		i++;
	}
	return builder.GetResultClass();
}
