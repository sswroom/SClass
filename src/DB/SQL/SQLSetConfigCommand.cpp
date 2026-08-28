#include "Stdafx.h"
#include "DB/SQL/SQLSetConfigCommand.h"
DB::SQL::SQLSetConfigCommand::SQLSetConfigCommand(ConfigLife configLife, NN<Text::String> configName, Text::CString configValue)
{
	this->configLife = configLife;
	this->configName = configName->Clone();
	this->configValue = Text::String::NewOrNull(configValue);
}

DB::SQL::SQLSetConfigCommand::~SQLSetConfigCommand()
{
	this->configName->Release();
	OPTSTR_DEL(this->configValue);
}

DB::SQL::CommandType DB::SQL::SQLSetConfigCommand::GetCommandType() const
{
	return CommandType::SetConfig;
}

DB::SQL::SQLSetConfigCommand::ConfigLife DB::SQL::SQLSetConfigCommand::GetConfigLife() const
{
	return configLife;
}

NN<Text::String> DB::SQL::SQLSetConfigCommand::GetConfigName() const
{
	return configName;
}

Optional<Text::String> DB::SQL::SQLSetConfigCommand::GetConfigValue() const
{
	return configValue;
}
