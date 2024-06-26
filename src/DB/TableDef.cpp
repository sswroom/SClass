#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/DBClassBuilder.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "Text/MyString.h"

DB::TableDef::TableDef(Text::CString schemaName, Text::CStringNN tableName)
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
	OPTSTR_DEL(this->databaseName);
	OPTSTR_DEL(this->schemaName);
	this->tableName->Release();
	OPTSTR_DEL(this->engine);
	OPTSTR_DEL(this->charset);
	SDEL_TEXT(this->attr);
	SDEL_TEXT(this->comments);
	i = this->cols.GetCount();
	while (i-- > 0)
	{
		this->cols.RemoveAt(i).Delete();
	}
}

Optional<Text::String> DB::TableDef::GetDatabaseName() const
{
	return this->databaseName;
}

Optional<Text::String> DB::TableDef::GetSchemaName() const
{
	return this->schemaName;
}

NN<Text::String> DB::TableDef::GetTableName() const
{
	return this->tableName;
}

Optional<Text::String> DB::TableDef::GetEngine() const
{
	return this->engine;
}

Optional<Text::String> DB::TableDef::GetCharset() const
{
	return this->charset;
}

UnsafeArrayOpt<const UTF8Char> DB::TableDef::GetAttr() const
{
	return this->attr;
}

UnsafeArrayOpt<const UTF8Char> DB::TableDef::GetComments() const
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
	NN<DB::ColDef> col;
	Data::ArrayIterator<NN<DB::ColDef>> it = this->cols.Iterator();
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
	Data::ArrayIterator<NN<DB::ColDef>> it = this->cols.Iterator();
	while (it.HasNext())
	{
		if (it.Next()->IsPK())
			cnt++;
	}
	return cnt;
}

Data::ArrayIterator<NN<DB::ColDef>> DB::TableDef::ColIterator() const
{
	return this->cols.Iterator();
}

NN<DB::TableDef> DB::TableDef::AddCol(NN<DB::ColDef> col)
{
	this->cols.Add(col);
	return *this;
}

NN<DB::TableDef> DB::TableDef::SetDatabaseName(Text::CString databaseName)
{
	OPTSTR_DEL(this->databaseName);
	this->databaseName = Text::String::NewOrNull(databaseName);
	return *this;
}

NN<DB::TableDef> DB::TableDef::SetSchemaName(Text::CString schemaName)
{
	OPTSTR_DEL(this->schemaName);
	this->schemaName = Text::String::NewOrNull(schemaName);
	return *this;
}

NN<DB::TableDef> DB::TableDef::SetTableName(Text::CStringNN tableName)
{
	this->tableName->Release();
	this->tableName = Text::String::New(tableName);
	return *this;
}

NN<DB::TableDef> DB::TableDef::SetEngine(Text::CString engine)
{
	OPTSTR_DEL(this->engine);
	this->engine = Text::String::NewOrNull(engine);
	return *this;
}

NN<DB::TableDef> DB::TableDef::SetCharset(Text::CString charset)
{
	OPTSTR_DEL(this->charset);
	this->charset = Text::String::NewOrNull(charset);
	return *this;
}

NN<DB::TableDef> DB::TableDef::SetAttr(UnsafeArrayOpt<const UTF8Char> attr)
{
	SDEL_TEXT(this->attr);
	this->attr = Text::StrSCopyNew(attr);
	return *this;
}

NN<DB::TableDef> DB::TableDef::SetComments(UnsafeArrayOpt<const UTF8Char> comments)
{
	SDEL_TEXT(this->comments)
	this->comments = Text::StrSCopyNew(comments);
	return *this;
}

NN<DB::TableDef> DB::TableDef::SetSQLType(DB::SQLType sqlType)
{
	this->sqlType = sqlType;
	return *this;
}

void DB::TableDef::ColFromReader(NN<DB::DBReader> r)
{
	UOSInt i = 0;
	UOSInt j = r->ColCount();
	NN<DB::ColDef> col;
	while (i < j)
	{
		NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
		r->GetColDef(i, col);
		this->AddCol(col);
		i++;
	}
}

NN<DB::TableDef> DB::TableDef::Clone() const
{
	NN<DB::TableDef> newObj;
	NEW_CLASSNN(newObj, DB::TableDef(OPTSTR_CSTR(this->schemaName), this->tableName->ToCString()));
	newObj->SetDatabaseName(OPTSTR_CSTR(this->databaseName));
	newObj->SetEngine(OPTSTR_CSTR(this->engine));
	newObj->SetCharset(OPTSTR_CSTR(this->charset));
	newObj->SetAttr(this->attr);
	newObj->SetComments(this->comments);
	newObj->SetSQLType(this->sqlType);
	Data::ArrayIterator<NN<DB::ColDef>> it = this->cols.Iterator();
	while (it.HasNext())
	{
		newObj->AddCol(it.Next()->Clone());
	}
	return newObj;
}

NN<Data::Class> DB::TableDef::CreateTableClass() const
{
	DB::DBClassBuilder builder;
	Data::ArrayIterator<NN<DB::ColDef>> it = this->cols.Iterator();
	while (it.HasNext())
	{
		NN<DB::ColDef> col = it.Next();
		builder.AddItem(col->GetColName()->v, col->GetColType(), col->IsNotNull());
	}
	return builder.GetResultClass();
}
