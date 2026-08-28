#include "Stdafx.h"
#include "DB/SQL/SQLShowDatabasesCommand.h"

DB::SQL::SQLShowDatabasesCommand::SQLShowDatabasesCommand()
{
}

DB::SQL::SQLShowDatabasesCommand::~SQLShowDatabasesCommand()
{
}

DB::SQL::CommandType DB::SQL::SQLShowDatabasesCommand::GetCommandType() const
{
	return DB::SQL::CommandType::ShowDatabases;
}
