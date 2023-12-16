#include "Stdafx.h"
#include "Data/ICaseStringMap.h"
#include "DB/DBModel.h"
#include "Text/StringBuilderUTF8.h"

DB::DBModel::DBModel()
{
}

DB::DBModel::~DBModel()
{
	LIST_FREE_FUNC(&this->tables, DEL_CLASS);
}

Bool DB::DBModel::LoadDatabase(DB::DBTool *db, Text::CString dbName, Text::CString schemaName)
{
	if (dbName.v && !db->ChangeDatabase(dbName))
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayListStringNN tableNames;
	DB::TableDef *table;
	Text::String *tableName;
	UOSInt j;
	db->QueryTableNames(schemaName, tableNames);
	Data::ArrayIterator<NotNullPtr<Text::String>> it = tableNames.Iterator();
	while (it.HasNext())
	{
		table = db->GetTableDef(schemaName, it.Next()->ToCString());
		if (table)
		{
			table->SetDatabaseName(dbName);
			this->tables.Add(table);
			sb.ClearStr();
			if (dbName.v)
			{
				sb.Append(dbName);
				sb.AppendUTF8Char('.');
			}
			tableName = table->GetTableName();
			sb.Append(tableName);
			this->tableMap.Put(sb.ToCString(), table);
			j = tableName->IndexOf('.');
			this->tableMap.Put(tableName->ToCString().Substring(j + 1), table);
		}
	}
	tableNames.FreeAll();
	return true;
}

DB::TableDef *DB::DBModel::GetTable(Text::CString tableName)
{
	return this->tableMap.Get(tableName);
}

UOSInt DB::DBModel::GetTableNames(Data::ArrayList<Text::CString> *tableNames)
{
	NotNullPtr<Data::ArrayList<Text::String*>> keys = this->tableMap.GetKeys();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		tableNames->Add(keys->GetItem(i)->ToCString());
		i++;
	}
	return j;
}
