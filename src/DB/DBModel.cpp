#include "Stdafx.h"
#include "Data/ICaseStringUTF8Map.h"
#include "DB/DBModel.h"
#include "Text/StringBuilderUTF8.h"

DB::DBModel::DBModel()
{
	NEW_CLASS(this->tableMap, Data::ICaseStringUTF8Map<TableDef*>());
	NEW_CLASS(this->tables, Data::ArrayList<TableDef*>());
}

DB::DBModel::~DBModel()
{
	DEL_CLASS(this->tableMap);
	LIST_FREE_FUNC(this->tables, DEL_CLASS);
	DEL_CLASS(this->tables);
}

Bool DB::DBModel::LoadDatabase(DB::DBTool *db, const UTF8Char *dbName)
{
	if (dbName && !db->ChangeDatabase(dbName))
	{
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Data::ArrayList<const UTF8Char *> tableNames;
	DB::TableDef *table;
	const UTF8Char *tableName;
	UOSInt i;
	UOSInt j;
	db->GetTableNames(&tableNames);
	i = tableNames.GetCount();
	while (i-- > 0)
	{
		table = db->GetTableDef(tableNames.GetItem(i));
		if (table)
		{
			table->SetDatabaseName(dbName);
			this->tables->Add(table);
			sb.ClearStr();
			if (dbName)
			{
				sb.Append(dbName);
				sb.AppendChar('.', 1);
			}
			sb.Append(tableName = table->GetTableName());
			this->tableMap->Put(sb.ToString(), table);
			j = Text::StrIndexOf(tableName, '.');
			this->tableMap->Put(tableName + j + 1, table);
		}
	}
	db->ReleaseDatabaseNames(&tableNames);
	return true;
}

DB::TableDef *DB::DBModel::GetTable(const UTF8Char *tableName)
{
	return this->tableMap->Get(tableName);
}

UOSInt DB::DBModel::GetTableNames(Data::ArrayList<const UTF8Char*> *tableNames)
{
	return tableNames->AddAll(this->tableMap->GetKeys());
}
