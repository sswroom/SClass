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
		this->cols.RemoveAt(i).Delete();
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

Optional<DB::ColDef> DB::TableDef::GetCol(UOSInt index) const
{
	return this->cols.GetItem(index);
}

Optional<DB::ColDef> DB::TableDef::GetSinglePKCol() const
{
	Optional<DB::ColDef> retCol = 0;
	NotNullPtr<DB::ColDef> col;
	Data::ArrayIterator<NotNullPtr<DB::ColDef>> it = this->cols.Iterator();
	while (it.HasNext())
	{
		col = it.Next();
		if (col->IsPK())
		{
			if (!retCol.IsNull())
			{
				return 0;
			}
			retCol = col;
		}
	}
	return retCol;
}

UOSInt DB::TableDef::CountPK() const
{
	UOSInt cnt = 0;
	Data::ArrayIterator<NotNullPtr<DB::ColDef>> it = this->cols.Iterator();
	while (it.HasNext())
	{
		if (it.Next()->IsPK())
			cnt++;
	}
	return cnt;
}

Data::ArrayIterator<NotNullPtr<DB::ColDef>> DB::TableDef::ColIterator() const
{
	return this->cols.Iterator();
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
	Data::ArrayIterator<NotNullPtr<DB::ColDef>> it = this->cols.Iterator();
	while (it.HasNext())
	{
		newObj->AddCol(it.Next()->Clone());
	}
	return newObj;
}

NotNullPtr<Data::Class> DB::TableDef::CreateTableClass() const
{
	DB::DBClassBuilder builder;
	Data::ArrayIterator<NotNullPtr<DB::ColDef>> it = this->cols.Iterator();
	while (it.HasNext())
	{
		NotNullPtr<DB::ColDef> col = it.Next();
		builder.AddItem(col->GetColName()->v, col->GetColType());
	}
	return builder.GetResultClass();
}
