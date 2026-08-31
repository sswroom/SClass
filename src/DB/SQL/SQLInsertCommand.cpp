#include "Stdafx.h"
#include "DB/SQL/SQLInsertCommand.h"

void __stdcall DB::SQL::SQLInsertCommand::FreeRow(NN<Data::ArrayListNN<SQLValue>> row)
{
	row->DeleteAll();
	row.Delete();
}

DB::SQL::SQLInsertCommand::SQLInsertCommand(NN<SQLObjectPath> objPath)
{
	this->objectPath = objPath;
}

DB::SQL::SQLInsertCommand::~SQLInsertCommand()
{
	this->objectPath.Delete();
	this->columnList.FreeAll();
	this->rowList.FreeAll(FreeRow);
}

DB::SQL::CommandType DB::SQL::SQLInsertCommand::GetCommandType() const
{
	return CommandType::Insert;
}

void DB::SQL::SQLInsertCommand::AddColumn(Text::CStringNN columnName)
{
	this->columnList.Add(Text::String::New(columnName));
}

void DB::SQL::SQLInsertCommand::AddRow(NN<Data::ArrayListNN<SQLValue>> row)
{
	NN<Data::ArrayListNN<SQLValue>> newRow;
	NEW_CLASSNN(newRow, Data::ArrayListNN<SQLValue>());
	newRow->AddAll(row);
	this->rowList.Add(newRow);
}

NN<DB::SQL::SQLObjectPath> DB::SQL::SQLInsertCommand::GetObjectPath() const
{
	return this->objectPath;
}

UIntOS DB::SQL::SQLInsertCommand::GetColumnCount() const
{
	return this->columnList.GetCount();
}

Optional<Text::String> DB::SQL::SQLInsertCommand::GetColumn(UIntOS index) const
{
	return this->columnList.GetItem(index);
}

UIntOS DB::SQL::SQLInsertCommand::GetRowCount() const
{
	return this->rowList.GetCount();
}

Optional<Data::ArrayListNN<DB::SQL::SQLValue>> DB::SQL::SQLInsertCommand::GetRow(UIntOS index) const
{
	return this->rowList.GetItem(index);
}