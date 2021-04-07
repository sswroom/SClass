#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQL/CreateTableCommand.h"
#include "Text/MyString.h"

DB::SQL::CreateTableCommand::CreateTableCommand(const UTF8Char *tableName, TableDef *tableDef, Bool toRelease)
{
	this->tableName = Text::StrCopyNew(tableName);
	if (toRelease)
	{
		this->tableDef = tableDef;
	}
	else
	{
		this->tableDef = tableDef->Clone();
	}
}

DB::SQL::CreateTableCommand::~CreateTableCommand()
{
	Text::StrDelNew(this->tableName);
	DEL_CLASS(this->tableDef);
}

DB::SQL::SQLCommand::CommandType DB::SQL::CreateTableCommand::GetCommandType()
{
	return CT_CREATE_TABLE;
}

const UTF8Char *DB::SQL::CreateTableCommand::GetTableName()
{
	return this->tableName;
}

DB::TableDef *DB::SQL::CreateTableCommand::GetTableDef()
{
	return this->tableDef;
}
