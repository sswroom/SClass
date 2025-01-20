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
	Text::CStringNN nndbName;
	if (dbName.SetTo(nndbName) && !db->ChangeDatabase(nndbName))
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
		if (db->GetTableDef(schemaName, it.Next()->ToCString()).SetTo(table))
		{
			table->SetDatabaseName(dbName);
			this->tables.Add(table);
			sb.ClearStr();
			if (dbName.SetTo(nndbName))
			{
				sb.Append(nndbName);
				sb.AppendUTF8Char('.');
			}
			tableName = table->GetTableName();
			sb.Append(tableName);
			this->tableMap.PutC(sb.ToCString(), table);
			j = tableName->IndexOf('.');
			this->tableMap.PutC(tableName->ToCString().Substring(j + 1), table);
		}
	}
	tableNames.FreeAll();
	return true;
}

Optional<DB::TableDef> DB::DBModel::GetTable(Text::CStringNN tableName)
{
	return this->tableMap.GetC(tableName);
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
