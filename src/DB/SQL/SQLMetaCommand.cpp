#include "Stdafx.h"
#include "DB/SQL/SQLMetaCommand.h"

DB::SQL::SQLMetaCommand::SQLMetaCommand(Text::CStringNN commandText)
{
	this->commandText = Text::String::New(commandText);
}

DB::SQL::SQLMetaCommand::~SQLMetaCommand()
{
	this->commandText->Release();
}

DB::SQL::CommandType DB::SQL::SQLMetaCommand::GetCommandType() const
{
	return CommandType::MetaCommand;
}

NN<Text::String> DB::SQL::SQLMetaCommand::GetCommandText() const
{
	return this->commandText;
}

