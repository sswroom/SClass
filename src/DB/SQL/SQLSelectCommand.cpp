#include "Stdafx.h"
#include "DB/SQL/SQLSelectCommand.h"

void __stdcall DB::SQL::SQLSelectCommand::FreeColumnInfo(NN<ColumnInfo> v)
{
	v->value.Delete();
	OPTSTR_DEL(v->alias);
	MemFreeNN(v);
}

DB::SQL::SQLSelectCommand::SQLSelectCommand()
{
}

DB::SQL::SQLSelectCommand::~SQLSelectCommand()
{
	this->columnList.FreeAll(FreeColumnInfo);
}

DB::SQL::CommandType DB::SQL::SQLSelectCommand::GetCommandType() const
{
	return CommandType::Select;
}

void DB::SQL::SQLSelectCommand::AddColumn(NN<SQLValue> value, Text::CString alias)
{
	NN<ColumnInfo> col = MemAllocNN(ColumnInfo);
	col->value = value;
	col->alias = Text::String::NewOrNull(alias);
	columnList.Add(col);
}

UIntOS DB::SQL::SQLSelectCommand::GetColumnCount() const
{
	return columnList.GetCount();
}

Optional<DB::SQL::SQLValue> DB::SQL::SQLSelectCommand::GetColumn(UIntOS index) const
{
	NN<ColumnInfo> col;
	if (!columnList.GetItem(index).SetTo(col))
	{
		return nullptr;
	}
	return col->value;
}

Optional<Text::String> DB::SQL::SQLSelectCommand::GetColumnAlias(UIntOS index) const
{
	NN<ColumnInfo> col;
	if (!columnList.GetItem(index).SetTo(col))
	{
		return nullptr;
	}
	return col->alias;
}
