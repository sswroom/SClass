#include "Stdafx.h"
#include "DB/SQL/SQLEngineDBTable.h"

DB::SQL::SQLEngineDBTable::SQLEngineDBTable(NN<DB::SharedReadingDB> db, Text::CString schemaName, Text::CStringNN tableName)
{
	this->db = db;
	db->UseObject();
	this->schemaName = Text::String::NewOrNull(schemaName);
	this->tableName = Text::String::New(tableName);
}

DB::SQL::SQLEngineDBTable::~SQLEngineDBTable()
{
	OPTSTR_DEL(this->schemaName);
	this->tableName->Release();
	this->db->UnuseObject();
}

Optional<DB::TableDef> DB::SQL::SQLEngineDBTable::GetTableDef()
{
	Sync::MutexUsage mutUsage;
	NN<DB::ReadingDB> db =this->db->UseDB(mutUsage);
	return db->GetTableDef(OPTSTR_CSTR(this->schemaName), this->tableName->ToCString());
}

Optional<DB::DBReader> DB::SQL::SQLEngineDBTable::QueryTableData(Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	Sync::MutexUsage mutUsage;
	NN<DB::ReadingDB> db = this->db->UseDB(mutUsage);
	return db->QueryTableData(OPTSTR_CSTR(this->schemaName), this->tableName->ToCString(), colNames, dataOfst, maxCnt, ordering, condition);
}

void DB::SQL::SQLEngineDBTable::CloseReader(NN<DBReader> r)
{
	Sync::MutexUsage mutUsage;
	NN<DB::ReadingDB> db = this->db->UseDB(mutUsage);
	db->CloseReader(r);
}
