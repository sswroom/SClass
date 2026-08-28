#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQL/SQLCreateTableCommand.h"
#include "Text/MyString.h"

DB::SQL::SQLCreateTableCommand::SQLCreateTableCommand(NN<DB::TableDef> tableDef, Bool toRelease)
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

DB::SQL::SQLCreateTableCommand::~SQLCreateTableCommand()
{
	this->tableDef.Delete();
}

DB::SQL::CommandType DB::SQL::SQLCreateTableCommand::GetCommandType() const
{
	return DB::SQL::CommandType::CreateTable;
}

NN<DB::TableDef> DB::SQL::SQLCreateTableCommand::GetTableDef()
{
	return this->tableDef;
}
