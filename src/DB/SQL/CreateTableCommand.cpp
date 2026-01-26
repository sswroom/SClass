#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQL/CreateTableCommand.h"
#include "Text/MyString.h"

DB::SQL::CreateTableCommand::CreateTableCommand(NN<DB::TableDef> tableDef, Bool toRelease)
{
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
	this->tableDef.Delete();
}

DB::SQL::SQLCommand::CommandType DB::SQL::CreateTableCommand::GetCommandType()
{
	return CT_CREATE_TABLE;
}

NN<DB::TableDef> DB::SQL::CreateTableCommand::GetTableDef()
{
	return this->tableDef;
}
