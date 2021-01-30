#include "Stdafx.h"
#include "Data/ICaseStringUTF8Map.h"
#include "DB/DBCache.h"
#include "DB/DBReader.h"

DB::DBCache::TableInfo *DB::DBCache::GetTableInfo(const UTF8Char *tableName)
{
	DB::DBCache::TableInfo *table;
	this->tableMut->Lock();
	table = this->tableMap->Get(tableName);
	this->tableMut->Unlock();
	if (table)
		return table;
	DB::TableDef *def = this->model->GetTable(tableName);
	if (def == 0)
	{
		return 0;
	}
	table = MemAlloc(DB::DBCache::TableInfo, 1);
	table->tableName = Text::StrCopyNew(tableName);
	table->def = def;
	table->dataCnt = 0;
	DB::SQLBuilder sql(this->db->GetSvrType());
	sql.AppendCmd((const UTF8Char*)"select count(*) from ");
	sql.AppendTableName(def);
	DB::DBReader *r = this->db->ExecuteReader(sql.ToString());
	if (r)
	{
		if (r->ReadNext())
		{
			table->dataCnt = r->GetInt32(0);
		}
		this->db->CloseReader(r);
	}
	this->tableMut->Lock();
	DB::DBCache::TableInfo *oldTable = this->tableMap->Put(table->tableName, table);
	this->tableMut->Unlock();
	if (oldTable)
	{
		Text::StrDelNew(oldTable->tableName);
		MemFree(oldTable);
	}
	return table;
}

DB::DBCache::DBCache(DB::DBModel *model, DB::DBTool *db)
{
	this->model = model;
	this->db = db;
	NEW_CLASS(this->tableMut, Sync::Mutex());
	NEW_CLASS(this->tableMap, Data::ICaseStringUTF8Map<DB::DBCache::TableInfo*>());
}

DB::DBCache::~DBCache()
{
	Data::ArrayList<DB::DBCache::TableInfo*> *tableList = this->tableMap->GetValues();
	DB::DBCache::TableInfo *table;
	UOSInt i = tableList->GetCount();
	while (i-- > 0)
	{
		table = tableList->GetItem(i);
		Text::StrDelNew(table->tableName);
		MemFree(table);
	}
	DEL_CLASS(this->tableMap);
	DEL_CLASS(this->tableMut);
}

OSInt DB::DBCache::GetRowCount(const UTF8Char *tableName)
{
	DB::DBCache::TableInfo *table = this->GetTableInfo(tableName);
	if (table)
	{
		return table->dataCnt;
	}
	else
	{
		return -1;
	}
}

UOSInt DB::DBCache::GetTableData(Data::ArrayList<DB::DBRow*> *outRows, const UTF8Char *tableName)
{

}

void DB::DBCache::FreeTableData(Data::ArrayList<DB::DBRow*> *rows)
{

}
