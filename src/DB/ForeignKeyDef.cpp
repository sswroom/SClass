#include "Stdafx.h"
#include "DB/ForeignKeyDef.h"

void DB::ForeignKeyDef::FreeColMap(NN<ColMap> colMap)
{
	colMap->localCol->Release();
	colMap->foreignCol->Release();
	MemFreeNN(colMap);
}

DB::ForeignKeyDef::ForeignKeyDef(Text::CString name, Text::CString foreignSchema, Text::CStringNN foreignTable)
{
	this->name = Text::String::NewOrNull(name);
	this->foreignSchema = Text::String::NewOrNull(foreignSchema);
	this->foreignTable = Text::String::New(foreignTable);
}

DB::ForeignKeyDef::ForeignKeyDef(Optional<Text::String> name, Optional<Text::String> foreignSchema, NN<Text::String> foreignTable)
{
	this->name = Text::String::CopyOrNull(name);
	this->foreignSchema = Text::String::CopyOrNull(foreignSchema);
	this->foreignTable = foreignTable->Clone();
}

DB::ForeignKeyDef::~ForeignKeyDef()
{
	OPTSTR_DEL(this->name);
	OPTSTR_DEL(this->foreignSchema);
	this->foreignTable->Release();
	this->cols.FreeAll(FreeColMap);
}

Optional<Text::String> DB::ForeignKeyDef::GetName() const
{
	return this->name;
}

Optional<Text::String> DB::ForeignKeyDef::GetForeignSchema() const
{
	return this->foreignSchema;
}

NN<Text::String> DB::ForeignKeyDef::GetForeignTable() const
{
	return this->foreignTable;
}

void DB::ForeignKeyDef::AddCol(Text::CStringNN localCol, Text::CStringNN foreignCol)
{
	NN<ColMap> colMap = MemAllocNN(ColMap);
	colMap->localCol = Text::String::New(localCol);
	colMap->foreignCol = Text::String::New(foreignCol);
	this->cols.Add(colMap);
}

void DB::ForeignKeyDef::AddCol(NN<Text::String> localCol, NN<Text::String> foreignCol)
{
	NN<ColMap> colMap = MemAllocNN(ColMap);
	colMap->localCol = localCol->Clone();
	colMap->foreignCol = foreignCol->Clone();
	this->cols.Add(colMap);
}

UIntOS DB::ForeignKeyDef::GetColCnt() const
{
	return this->cols.GetCount();
}

Optional<DB::ForeignKeyDef::ColMap> DB::ForeignKeyDef::GetCol(UIntOS index) const
{
	return this->cols.GetItem(index);
}
