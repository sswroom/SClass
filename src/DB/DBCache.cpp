#include "Stdafx.h"
#include "Data/ICaseStringUTF8Map.h"
#include "DB/DBCache.h"
#include "DB/DBReader.h"
#include "Sync/MutexUsage.h"

DB::DBCache::TableInfo *DB::DBCache::GetTableInfo(const UTF8Char *tableName)
{
	DB::DBCache::TableInfo *table;
	Sync::MutexUsage mutUsage(this->tableMut);
	table = this->tableMap->Get(tableName);
	mutUsage.EndUse();
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
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("select count(*) from "));
	sql.AppendTableName(def);
	DB::DBReader *r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		if (r->ReadNext())
		{
			table->dataCnt = (UInt32)r->GetInt32(0);
		}
		this->db->CloseReader(r);
	}
	mutUsage.BeginUse();
	DB::DBCache::TableInfo *oldTable = this->tableMap->Put(table->tableName, table);
	mutUsage.EndUse();
	if (oldTable)
	{
		Text::StrDelNew(oldTable->tableName);
		MemFree(oldTable);
	}
	return table;
}

DB::DBCache::TableInfo *DB::DBCache::GetTableInfo(DB::TableDef *tableDef)
{
	DB::DBCache::TableInfo *table;
	UOSInt i;
	Sync::MutexUsage mutUsage(this->tableMut);
	Data::ArrayList<DB::DBCache::TableInfo*> *tableList = this->tableMap->GetValues();
	i = tableList->GetCount();
	while (i-- > 0)
	{
		table = tableList->GetItem(i);
		if (table->def == tableDef)
		{
			mutUsage.EndUse();
			return table;
		}
	}
	mutUsage.EndUse();
	return 0;
}

DB::DBCache::DBCache(DB::DBModel *model, DB::DBTool *db)
{
	this->model = model;
	this->db = db;
	this->cacheCnt = 4000;
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
		return (OSInt)table->dataCnt;
	}
	else
	{
		return -1;
	}
}

UOSInt DB::DBCache::GetTableData(Data::ArrayList<DB::DBRow*> *outRows, const UTF8Char *tableName, DB::PageRequest *page)
{
	DB::DBCache::TableInfo *tableInfo = this->GetTableInfo(tableName);
	if (tableInfo == 0)
		return 0;
	UOSInt ret = 0;
	DB::SQLBuilder sql(this->db);
	DB::DBTool::PageStatus status = this->db->GenSelectCmdPage(&sql, tableInfo->def, page);
	DB::DBReader *r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		DB::DBRow *row;
		UOSInt pageSkip = 0;
		UOSInt pageSize = page->GetPageSize();
		if (status != DB::DBTool::PS_SUCC)
		{
			pageSkip = page->GetPageNum() * page->GetPageSize();
		}
		while (r->ReadNext())
		{
			if (pageSkip == 0)
			{
				NEW_CLASS(row, DB::DBRow(tableInfo->def));
				row->SetByReader(r, true);
				outRows->Add(row);
				pageSize--;
				if (pageSize == 0)
				{
					break;
				}
			}
			else
			{
				pageSkip--;
			}
			ret++;
		}
		this->db->CloseReader(r);
	}
	return ret;
}

DB::DBRow *DB::DBCache::GetTableItem(const UTF8Char *tableName, Int64 pk)
{
	DB::DBCache::TableInfo *tableInfo = this->GetTableInfo(tableName);
	if (tableInfo == 0)
		return 0;
	DB::ColDef *col = tableInfo->def->GetSinglePKCol();
	if (col == 0)
	{
		return 0;
	}
	switch (col->GetColType())
	{
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Int64:
		break;
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_Bool:
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_DateTime2:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Binary:
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Unknown:
	case DB::DBUtil::CT_UUID:
	default:
		return 0;
	}
	DB::DBRow *row = 0;
	DB::SQLBuilder sql(this->db);
	this->db->GenSelectCmdPage(&sql, tableInfo->def, 0);
	sql.AppendCmdC(CSTR(" where "));
	sql.AppendCol(col->GetColName()->v);
	sql.AppendCmdC(CSTR(" = "));
	sql.AppendInt64(pk);
	DB::DBReader *r = this->db->ExecuteReaderC(sql.ToString(), sql.GetLength());
	if (r)
	{
		if (r->ReadNext())
		{
			NEW_CLASS(row, DB::DBRow(tableInfo->def));
			row->SetByReader(r, true);
		}
		this->db->CloseReader(r);
	}
	return row;
}

void DB::DBCache::FreeTableData(Data::ArrayList<DB::DBRow*> *rows)
{
	if (rows->GetCount() > 0)
	{
		DB::TableDef *table = rows->GetItem(0)->GetTableDef();
		DB::DBCache::TableInfo *tableInfo = this->GetTableInfo(table);
		if (tableInfo->dataCnt >= this->cacheCnt)
		{
			LIST_FREE_FUNC(rows, DEL_CLASS);
		}
		else
		{
			/////////////////////////////
			LIST_FREE_FUNC(rows, DEL_CLASS);
		}
	}
}

void DB::DBCache::FreeTableItem(DB::DBRow *row)
{
	DEL_CLASS(row);
}

Bool DB::DBCache::IsTableExist(const UTF8Char *tableName)
{
	return this->GetTableInfo(tableName) != 0;
}
