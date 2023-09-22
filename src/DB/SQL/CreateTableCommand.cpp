#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQL/CreateTableCommand.h"
#include "Text/MyString.h"

DB::SQL::CreateTableCommand::CreateTableCommand(DB::TableDef *tableDef, Bool toRelease)
{
	if (toRelease)
	{
		this->tableDef = tableDef;
	}
	else
	{
		this->tableDef = tableDef->Clone().Ptr();
	}
}

DB::SQL::CreateTableCommand::~CreateTableCommand()
{
	DEL_CLASS(this->tableDef);
}

DB::SQL::SQLCommand::CommandType DB::SQL::CreateTableCommand::GetCommandType()
{
	return CT_CREATE_TABLE;
}

DB::TableDef *DB::SQL::CreateTableCommand::GetTableDef()
{
	return this->tableDef;
}
