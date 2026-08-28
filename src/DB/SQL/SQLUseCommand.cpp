#include "Stdafx.h"
#include "DB/SQL/SQLUseCommand.h"

DB::SQL::SQLUseCommand::SQLUseCommand(Text::CStringNN dbName)
{
	this->dbName = Text::String::New(dbName);
}

DB::SQL::SQLUseCommand::~SQLUseCommand()
{
	this->dbName->Release();
}

DB::SQL::CommandType DB::SQL::SQLUseCommand::GetCommandType() const
{
	return DB::SQL::CommandType::Use;
}
