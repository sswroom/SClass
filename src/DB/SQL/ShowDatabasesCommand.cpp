#include "Stdafx.h"
#include "DB/SQL/ShowDatabasesCommand.h"

DB::SQL::ShowDatabasesCommand::ShowDatabasesCommand()
{
}

DB::SQL::ShowDatabasesCommand::~ShowDatabasesCommand()
{
}

DB::SQL::CommandType DB::SQL::ShowDatabasesCommand::GetCommandType()
{
	return DB::SQL::CommandType::ShowDatabases;
}
