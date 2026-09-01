#include "Stdafx.h"
#include "DB/SQL/SQLCreateTableCommand.h"
#include "DB/SQL/SQLFileDB.h"
#include "DB/SQL/SQLFileDBReader.h"
#include "DB/SQL/SQLInsertCommand.h"



void __stdcall DB::SQL::SQLFileDB::FreeTableInfo(NN<TableInfo> tableInfo)
{
	tableInfo.Delete();
}

void __stdcall DB::SQL::SQLFileDB::FreeSchemaInfo(NN<SchemaInfo> schemaInfo)
{
	schemaInfo->tables.FreeAll(FreeTableInfo);
	schemaInfo.Delete();
}

Optional<DB::SQL::SQLFileDB::TableInfo> DB::SQL::SQLFileDB::GetTableInfo(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<SchemaInfo> schemaInfo;
	if (!this->schemaMap.GetC(schemaName.OrEmpty()).SetTo(schemaInfo))
	{
		return nullptr;
	}
	return schemaInfo->tables.GetC(tableName);
}

DB::SQL::SQLFileDB::SQLFileDB(NN<SQLFile> sqlFile) : DB::ReadingDB(sqlFile->GetSourceNameObj())
{
	this->sqlFile = sqlFile;
	this->tzQhr = Data::DateTimeUtil::GetLocalTzQhr();
	UIntOS i = 0;
	UIntOS j = sqlFile->GetCount();
	NN<SQLCommand> cmd;
	while (i < j)
	{
		if (sqlFile->GetSQL(i).SetTo(cmd))
		{
			CommandType cmdType = cmd->GetCommandType();
			if (cmdType == CommandType::CreateTable)
			{
				//(void)NN<SQLCreateTableCommand>::ConvertFrom(cmd);
			}
			else if (cmdType == CommandType::Insert)
			{
				NN<SQLInsertCommand> insertCmd = NN<SQLInsertCommand>::ConvertFrom(cmd);
				NN<SQLObjectPath> path = insertCmd->GetObjectPath();
				NN<Text::String> schemaName;
				NN<SQLObjectPath> parentPath;
				if (path->GetParentPath().SetTo(parentPath))
				{
					schemaName = parentPath->GetObjectName();
				}
				else
				{
					schemaName = Text::String::NewEmpty();
				}
				NN<SchemaInfo> schemaInfo;
				if (!this->schemaMap.GetNN(schemaName).SetTo(schemaInfo))
				{
					NEW_CLASSNN(schemaInfo, SchemaInfo());
					schemaInfo->name = schemaName;
					this->schemaMap.Put(schemaName, schemaInfo);
				}
				NN<Text::String> tableName = path->GetObjectName();
				NN<TableInfo> tableInfo;
				if (!schemaInfo->tables.GetNN(tableName).SetTo(tableInfo))
				{
					NEW_CLASSNN(tableInfo, TableInfo());
					tableInfo->path = path;
					tableInfo->createCmd = nullptr;
					schemaInfo->tables.Put(tableName, tableInfo);
				}
				tableInfo->insertCmds.Add(insertCmd);
			}
		}
		i++;
	}
}

DB::SQL::SQLFileDB::~SQLFileDB()
{
	this->sqlFile.Delete();
}
		
UIntOS DB::SQL::SQLFileDB::QuerySchemaNames(NN<Data::ArrayListStringNN> names)
{
	UIntOS i = 0;
	UIntOS j = this->schemaMap.GetCount();
	while (i < j)
	{
		names->Add(Text::String::OrEmpty(this->schemaMap.GetKey(i))->Clone());
		i++;
	}
	return j;
}

UIntOS DB::SQL::SQLFileDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	NN<SchemaInfo> schemaInfo;
	if (!this->schemaMap.GetC(schemaName.OrEmpty()).SetTo(schemaInfo))
	{
		return 0;
	}
	UIntOS i = 0;
	UIntOS j = schemaInfo->tables.GetCount();
	while (i < j)
	{
		names->Add(Text::String::OrEmpty(schemaInfo->tables.GetKey(i))->Clone());
		i++;
	}
	return j;
	return 0;
}

Optional<DB::DBReader> DB::SQL::SQLFileDB::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	(void)ordering;
	NN<TableInfo> tableInfo;
	if (!this->GetTableInfo(schemaName, tableName).SetTo(tableInfo))
	{
		return nullptr;
	}
	NN<SQLFileDBReader> r;
	NEW_CLASSNN(r, SQLFileDBReader(tableInfo, colNames, dataOfst, maxCnt, condition, this->tzQhr));
	return r;
}

Optional<DB::TableDef> DB::SQL::SQLFileDB::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<TableInfo> tableInfo;
	if (!this->GetTableInfo(schemaName, tableName).SetTo(tableInfo))
	{
		return nullptr;
	}

	NN<SQLCreateTableCommand> createCmd;
	if (tableInfo->createCmd.SetTo(createCmd))
	{
		return createCmd->GetTableDef()->Clone();
	}
	NN<SQLInsertCommand> insertCmd;
	NN<Data::ArrayListNN<SQLValue>> row;
	if (tableInfo->insertCmds.GetCount() == 0 || !tableInfo->insertCmds.GetItem(0).SetTo(insertCmd) || !insertCmd->GetRow(0).SetTo(row))
	{
		return nullptr;
	}
	NN<TableDef> tableDef;
	NEW_CLASSNN(tableDef, DB::TableDef(schemaName, tableName));
	NN<Text::String> columnName;
	NN<SQLValue> sqlValue;
	NN<ColDef> colDef;
	UIntOS i = 0;
	UIntOS j = insertCmd->GetColumnCount();
	while (i < j)
	{
		if (insertCmd->GetColumn(i).SetTo(columnName) && row->GetItem(i).SetTo(sqlValue))
		{
			NEW_CLASSNN(colDef, DB::ColDef(columnName));
			switch (sqlValue->GetValueType())
			{
			case SQLValue::ValueType::String:
				colDef->SetColType(DB::DBUtil::ColType::CT_VarUTF8Char);
				colDef->SetColSize(0x7fffffff);
				break;
			case SQLValue::ValueType::I32:
				colDef->SetColType(DB::DBUtil::ColType::CT_Int32);
				colDef->SetColSize(11);
				break;
			case SQLValue::ValueType::I64:
				colDef->SetColType(DB::DBUtil::ColType::CT_Int64);
				colDef->SetColSize(21);
				break;
			case SQLValue::ValueType::F64:
				colDef->SetColType(DB::DBUtil::ColType::CT_Double);
				colDef->SetColSize(50);
				colDef->SetColDP(20);
				break;
			case SQLValue::ValueType::Bool:
				colDef->SetColType(DB::DBUtil::ColType::CT_Bool);
				colDef->SetColSize(5);
				break;
			case SQLValue::ValueType::Function:
			case SQLValue::ValueType::ObjectPath:
			case SQLValue::ValueType::Null:
			default:
				break;
			}
			tableDef->AddCol(colDef);
		}
		i++;
	}
	return tableDef;
}

void DB::SQL::SQLFileDB::CloseReader(NN<DBReader> r)
{
	DB::SQL::SQLFileDBReader *reader = (DB::SQL::SQLFileDBReader *)r.Ptr();
	DEL_CLASS(reader);
}

void DB::SQL::SQLFileDB::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void DB::SQL::SQLFileDB::Reconnect()
{
}

Int8 DB::SQL::SQLFileDB::GetTzQhr() const
{
	return this->tzQhr;
}

void DB::SQL::SQLFileDB::ForceTzQhr(Int8 tzQhr)
{
	this->tzQhr = tzQhr;
}

UIntOS DB::SQL::SQLFileDB::GetDatabaseNames(NN<Data::ArrayListStringNN> arr)
{
	return 0;
}

void DB::SQL::SQLFileDB::ReleaseDatabaseNames(NN<Data::ArrayListStringNN> arr)
{
}

Bool DB::SQL::SQLFileDB::ChangeDatabase(Text::CStringNN databaseName)
{
	return false;
}

Optional<Text::String> DB::SQL::SQLFileDB::GetCurrDBName()
{
	return nullptr;
}

Bool DB::SQL::SQLFileDB::IsFullConn() const
{
	return false;
}

Bool DB::SQL::SQLFileDB::IsDBTool() const
{
	return false;
}
