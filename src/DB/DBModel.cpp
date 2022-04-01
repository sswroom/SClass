#include "Stdafx.h"
#include "Data/ICaseStringMap.h"
#include "DB/DBModel.h"
#include "Text/StringBuilderUTF8.h"

DB::DBModel::DBModel()
{
	NEW_CLASS(this->tableMap, Data::ICaseStringMap<TableDef*>());
	NEW_CLASS(this->tables, Data::ArrayList<TableDef*>());
}

DB::DBModel::~DBModel()
{
	DEL_CLASS(this->tableMap);
	LIST_FREE_FUNC(this->tables, DEL_CLASS);
	DEL_CLASS(this->tables);
}

Bool DB::DBModel::LoadDatabase(DB::DBTool *db, Text::CString dbName)
{
	if (dbName.v && !db->ChangeDatabase(dbName.v))
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<Text::CString> tableNames;
	DB::TableDef *table;
	Text::String *tableName;
	UOSInt i;
	UOSInt j;
	db->QueryTableNames(&tableNames);
	i = tableNames.GetCount();
	while (i-- > 0)
	{
		table = db->GetTableDef(tableNames.GetItem(i));
		if (table)
		{
			table->SetDatabaseName(dbName);
			this->tables->Add(table);
			sb.ClearStr();
			if (dbName.v)
			{
				sb.Append(dbName);
				sb.AppendUTF8Char('.');
			}
			tableName = table->GetTableName();
			sb.Append(tableName);
			this->tableMap->Put(sb.ToCString(), table);
			j = tableName->IndexOf('.');
			this->tableMap->Put(tableName->ToCString().Substring(j + 1), table);
		}
	}
	db->ReleaseTableNames(&tableNames);
	return true;
}

DB::TableDef *DB::DBModel::GetTable(Text::CString tableName)
{
	return this->tableMap->Get(tableName);
}

UOSInt DB::DBModel::GetTableNames(Data::ArrayList<Text::CString> *tableNames)
{
	Data::ArrayList<Text::String*> *keys = this->tableMap->GetKeys();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		tableNames->Add(keys->GetItem(i)->ToCString());
		i++;
	}
	return j;
}
