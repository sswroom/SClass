#include "Stdafx.h"
#include "DB/SQL/UseCommand.h"

DB::SQL::UseCommand::UseCommand(Text::CStringNN dbName)
{
	this->dbName = Text::String::New(dbName);
}

DB::SQL::UseCommand::~UseCommand()
{
	this->dbName->Release();
}

DB::SQL::CommandType DB::SQL::UseCommand::GetCommandType()
{
	return DB::SQL::CommandType::Use;
}
