#include "Stdafx.h"
#include "DB/SQL/SQLFile.h"

DB::SQL::SQLFile::SQLFile(NN<Text::String> sourceName) : IO::ParsedObject(sourceName)
{
	this->sqlType = DB::SQLType::Unknown;
}

DB::SQL::SQLFile::SQLFile(const Text::CStringNN &sourceName) : IO::ParsedObject(sourceName)
{
	this->sqlType = DB::SQLType::Unknown;
}

DB::SQL::SQLFile::~SQLFile()
{
	this->sqlList.DeleteAll();
}

IO::ParserType DB::SQL::SQLFile::GetParserType() const
{
	return IO::ParserType::SQLFile;
}

void DB::SQL::SQLFile::SetSQLType(SQLType sqlType)
{
	this->sqlType = sqlType;
}

void DB::SQL::SQLFile::AddSQL(NN<SQLCommand> sql)
{
	this->sqlList.Add(sql);
}

DB::SQLType DB::SQL::SQLFile::GetSQLType() const
{
	return this->sqlType;
}
