#include "Stdafx.h"
#include "Data/ICaseStringMap.h"
#include "DB/DBCache.h"
#include "DB/DBReader.h"
#include "DB/SQLGenerator.h"
#include "Sync/MutexUsage.h"

Optional<DB::DBCache::TableInfo> DB::DBCache::GetTableInfo(Text::CStringNN tableName)
{
	NN<DB::DBCache::TableInfo> table;
	Sync::MutexUsage mutUsage(this->tableMut);
	if (this->tableMap.GetC(tableName).SetTo(table))
		return table;
	mutUsage.EndUse();
	NN<DB::TableDef> def;
	if (!this->model->GetTable(tableName).SetTo(def))
	{
		return 0;
	}
	table = MemAllocNN(DB::DBCache::TableInfo);
	table->tableName = Text::String::New(tableName);
	table->def = def;
	table->dataCnt = 0;
	DB::SQLBuilder sql(this->db);
	sql.AppendCmdC(CSTR("select count(*) from "));
	sql.AppendTableName(def);
	NN<DB::DBReader> r;
	if (this->db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		if (r->ReadNext())
		{
			table->dataCnt = (UInt32)r->GetInt32(0);
		}
		this->db->CloseReader(r);
	}
	mutUsage.BeginUse();
	NN<DB::DBCache::TableInfo> oldTable;
	if (this->tableMap.PutNN(table->tableName, table).SetTo(oldTable))
	{
		oldTable->tableName->Release();
		MemFreeNN(oldTable);
	}
	mutUsage.EndUse();
	return table;
}

Optional<DB::DBCache::TableInfo> DB::DBCache::GetTableInfo(NN<DB::TableDef> tableDef)
{
	NN<DB::DBCache::TableInfo> table;
	UOSInt i;
	Sync::MutexUsage mutUsage(this->tableMut);
	NN<const Data::ArrayListNN<DB::DBCache::TableInfo>> tableList = this->tableMap.GetValues();
	i = tableList->GetCount();
	while (i-- > 0)
	{
		table = tableList->GetItemNoCheck(i);
		if (table->def == tableDef)
		{
			return table;
		}
	}
	return 0;
}

DB::DBCache::DBCache(NN<DB::DBModel> model, NN<DB::DBTool> db)
{
	this->model = model;
	this->db = db;
	this->cacheCnt = 4000;
}

DB::DBCache::~DBCache()
{
	NN<const Data::ArrayListNN<DB::DBCache::TableInfo>> tableList = this->tableMap.GetValues();
	NN<DB::DBCache::TableInfo> table;
	UOSInt i = tableList->GetCount();
	while (i-- > 0)
	{
		table = tableList->GetItemNoCheck(i);
		table->tableName->Release();
		MemFreeNN(table);
	}
}

OSInt DB::DBCache::GetRowCount(Text::CStringNN tableName)
{
	NN<DB::DBCache::TableInfo> table;
	if (this->GetTableInfo(tableName).SetTo(table))
	{
		return (OSInt)table->dataCnt;
	}
	else
	{
		return -1;
	}
}

UOSInt DB::DBCache::QueryTableData(NN<Data::ArrayListNN<DB::DBRow>> outRows, Text::CStringNN tableName, DB::PageRequest *page)
{
	NN<DB::DBCache::TableInfo> tableInfo;
	if (!this->GetTableInfo(tableName).SetTo(tableInfo))
		return 0;
	UOSInt ret = 0;
	DB::SQLBuilder sql(this->db);
	DB::SQLGenerator::PageStatus status = DB::SQLGenerator::GenSelectCmdPage(sql, tableInfo->def, page);
	NN<DB::DBReader> r;
	if (this->db->ExecuteReader(sql.ToCString()).SetTo(r))
	{
		NN<DB::DBRow> row;
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
				NEW_CLASSNN(row, DB::DBRow(tableInfo->def));
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

DB::DBRow *DB::DBCache::GetTableItem(Text::CStringNN tableName, Int64 pk)
{
	NN<DB::DBCache::TableInfo> tableInfo;
	if (!this->GetTableInfo(tableName).SetTo(tableInfo))
		return 0;
	NN<DB::ColDef> col;
	if (!tableInfo->def->GetSinglePKCol().SetTo(col))
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
	DB::SQLGenerator::GenSelectCmdPage(sql, tableInfo->def, 0);
	sql.AppendCmdC(CSTR(" where "));
	sql.AppendCol(col->GetColName()->v);
	sql.AppendCmdC(CSTR(" = "));
	sql.AppendInt64(pk);
	NN<DB::DBReader> r;
	if (this->db->ExecuteReader(sql.ToCString()).SetTo(r))
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

void DB::DBCache::FreeTableData(NN<Data::ArrayListNN<DB::DBRow>> rows)
{
	NN<DB::DBRow> row;
	if (rows->GetCount() > 0 && rows->GetItem(0).SetTo(row))
	{
		NN<DB::TableDef> table = row->GetTableDef();
		NN<DB::DBCache::TableInfo> tableInfo;
		if (this->GetTableInfo(table).SetTo(tableInfo))
		{
			if (tableInfo->dataCnt >= this->cacheCnt)
			{
				rows->DeleteAll();
			}
			else
			{
				/////////////////////////////
				rows->DeleteAll();
			}
		}
		else
		{
			rows->DeleteAll();
		}
	}
}

void DB::DBCache::FreeTableItem(NN<DB::DBRow> row)
{
	row.Delete();
}

Bool DB::DBCache::IsTableExist(Text::CStringNN tableName)
{
	return this->GetTableInfo(tableName).NotNull();
}
