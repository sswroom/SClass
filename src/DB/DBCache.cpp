#include "Stdafx.h"
#include "Data/ICaseStringMap.h"
#include "DB/DBCache.h"
#include "DB/DBReader.h"
#include "DB/SQLGenerator.h"
#include "Sync/MutexUsage.h"

DB::DBCache::TableInfo *DB::DBCache::GetTableInfo(Text::CString tableName)
{
	DB::DBCache::TableInfo *table;
	Sync::MutexUsage mutUsage(this->tableMut);
	table = this->tableMap.Get(tableName);
	mutUsage.EndUse();
	if (table)
		return table;
	DB::TableDef *def = this->model->GetTable(tableName);
	if (def == 0)
	{
		return 0;
	}
	table = MemAlloc(DB::DBCache::TableInfo, 1);
	table->tableName = Text::String::New(tableName);
	table->def = def;
	table->dataCnt = 0;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("select count(*) from "));
	sql.AppendTableName(def);
	DB::DBReader *r = this->db->ExecuteReader(sql.ToCString());
	if (r)
	{
		if (r->ReadNext())
		{
			table->dataCnt = (UInt32)r->GetInt32(0);
		}
		this->db->CloseReader(r);
	}
	mutUsage.BeginUse();
	DB::DBCache::TableInfo *oldTable = this->tableMap.PutNN(table->tableName, table);
	mutUsage.EndUse();
	if (oldTable)
	{
		oldTable->tableName->Release();
		MemFree(oldTable);
	}
	return table;
}

DB::DBCache::TableInfo *DB::DBCache::GetTableInfo(DB::TableDef *tableDef)
{
	DB::DBCache::TableInfo *table;
	UOSInt i;
	Sync::MutexUsage mutUsage(this->tableMut);
	NotNullPtr<const Data::ArrayList<DB::DBCache::TableInfo*>> tableList = this->tableMap.GetValues();
	i = tableList->GetCount();
	while (i-- > 0)
	{
		table = tableList->GetItem(i);
		if (table->def == tableDef)
		{
			return table;
		}
	}
	return 0;
}

DB::DBCache::DBCache(NotNullPtr<DB::DBModel> model, NotNullPtr<DB::DBTool> db)
{
	this->model = model;
	this->db = db;
	this->cacheCnt = 4000;
}

DB::DBCache::~DBCache()
{
	NotNullPtr<const Data::ArrayList<DB::DBCache::TableInfo*>> tableList = this->tableMap.GetValues();
	DB::DBCache::TableInfo *table;
	UOSInt i = tableList->GetCount();
	while (i-- > 0)
	{
		table = tableList->GetItem(i);
		table->tableName->Release();
		MemFree(table);
	}
}

OSInt DB::DBCache::GetRowCount(Text::CString tableName)
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

UOSInt DB::DBCache::QueryTableData(Data::ArrayList<DB::DBRow*> *outRows, Text::CString tableName, DB::PageRequest *page)
{
	DB::DBCache::TableInfo *tableInfo = this->GetTableInfo(tableName);
	if (tableInfo == 0)
		return 0;
	UOSInt ret = 0;
	DB::SQLBuilder sql(this->db);
	DB::SQLGenerator::PageStatus status = DB::SQLGenerator::GenSelectCmdPage(&sql, tableInfo->def, page);
	DB::DBReader *r = this->db->ExecuteReader(sql.ToCString());
	if (r)
	{
		DB::DBRow *row;
		UOSInt pageSkip = 0;
		UOSInt pageSize = page->GetPageSize();
		if (status != DB::SQLGenerator::PageStatus::Succ)
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

DB::DBRow *DB::DBCache::GetTableItem(Text::CString tableName, Int64 pk)
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
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_Bool:
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
	case DB::DBUtil::CT_Date:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_Binary:
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Unknown:
	case DB::DBUtil::CT_UUID:
	default:
		return 0;
	}
	DB::DBRow *row = 0;
	DB::SQLBuilder sql(this->db);
	DB::SQLGenerator::GenSelectCmdPage(&sql, tableInfo->def, 0);
	sql.AppendCmdC(CSTR(" where "));
	sql.AppendCol(col->GetColName()->v);
	sql.AppendCmdC(CSTR(" = "));
	sql.AppendInt64(pk);
	DB::DBReader *r = this->db->ExecuteReader(sql.ToCString());
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

Bool DB::DBCache::IsTableExist(Text::CString tableName)
{
	return this->GetTableInfo(tableName) != 0;
}
