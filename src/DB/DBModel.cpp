#include "Stdafx.h"
#include "Data/ICaseStringMap.h"
#include "DB/DBModel.h"
#include "Text/StringBuilderUTF8.h"

DB::DBModel::DBModel()
{
}

DB::DBModel::~DBModel()
{
	this->tables.DeleteAll();
}

Bool DB::DBModel::LoadDatabase(NN<DB::DBTool> db, Text::CString dbName, Text::CString schemaName)
{
	if (dbName.v && !db->ChangeDatabase(dbName))
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayListStringNN tableNames;
	NN<DB::TableDef> table;
	NN<Text::String> tableName;
	UOSInt j;
	db->QueryTableNames(schemaName, tableNames);
	Data::ArrayIterator<NN<Text::String>> it = tableNames.Iterator();
	while (it.HasNext())
	{
		if (table.Set(db->GetTableDef(schemaName, it.Next()->ToCString())))
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

Optional<DB::TableDef> DB::DBModel::GetTable(Text::CStringNN tableName)
{
	return this->tableMap.Get(tableName);
}

UOSInt DB::DBModel::GetTableNames(NN<Data::ArrayList<Text::CString>> tableNames)
{
	NN<Data::ArrayList<Text::String*>> keys = this->tableMap.GetKeys();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		tableNames->Add(keys->GetItem(i)->ToCString());
		i++;
	}
	return j;
}
