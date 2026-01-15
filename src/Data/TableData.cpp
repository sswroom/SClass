#include "Stdafx.h"
#include "Data/TableData.h"
#include "DB/ColDef.h"

Data::TableData::TableData(NN<DB::ReadingDB> db, Bool needRelease, Text::CString schemaName, Text::CStringNN tableName)
{
	this->db = db;
	this->needRelease = needRelease;
	this->schemaName = Text::String::NewOrNull(schemaName);
	this->tableName = Text::String::New(tableName);
	this->cond = nullptr;
}

Data::TableData::~TableData()
{
	if (this->needRelease)
		this->db.Delete();
	OPTSTR_DEL(this->schemaName);
	this->tableName->Release();
	this->cond.Delete();
}

Optional<DB::DBReader> Data::TableData::GetTableData()
{
	return this->db->QueryTableData(OPTSTR_CSTR(this->schemaName), this->tableName->ToCString(), nullptr, 0, 0, nullptr, this->cond);
}

Optional<DB::DBReader> Data::TableData::GetTableData(NN<Data::QueryConditions> cond)
{
	NN<QueryConditions> tabCond;
	if (this->cond.SetTo(tabCond))
	{
		cond->And(NN<Conditions::BooleanObject>::ConvertFrom(tabCond->GetRootCond()->Clone()));
	}
	return this->db->QueryTableData(OPTSTR_CSTR(this->schemaName), this->tableName->ToCString(), nullptr, 0, 0, nullptr, cond);
}

Bool Data::TableData::GetColumnDataStr(Text::CStringNN columnName, NN<Data::ArrayListStringNN> str)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::DBReader> r;
	if (!this->GetTableData().SetTo(r))
		return false;
	UIntOS col = INVALID_INDEX;
	UIntOS i = 0;
	UIntOS colCnt = r->ColCount();
	while (i < colCnt)
	{
		if (r->GetName(i, sbuff).SetTo(sptr))
		{
			if (columnName.Equals(sbuff, (UIntOS)(sptr - sbuff)))
			{
				col = i;
				break;
			}
		}
		i++;
	}
	if (col == INVALID_INDEX)
	{
		this->CloseReader(r);
		return false;
	}
	while (r->ReadNext())
	{
		str->Add(r->GetNewStrNN(col));
	}
	this->CloseReader(r);
	return true;
}

Optional<Data::DataSet> Data::TableData::GetDataSet(Text::CStringNN columnName)
{
	NN<DB::DBReader> r;
	if (!this->GetTableData().SetTo(r))
	{
		return nullptr;
	}
	UIntOS keyCol = INVALID_INDEX;
	UIntOS valueCol = INVALID_INDEX;
	DB::ColDef colDef(CSTR(""));
	UIntOS colCnt = r->ColCount();
	UIntOS i = 0;
	while (i < colCnt)
	{
		if (r->GetColDef(i, colDef))
		{
			if (colDef.IsPK())
			{
				if (keyCol == INVALID_INDEX)
				{
					keyCol = i;
				}
				else
				{
					this->CloseReader(r);
					return nullptr;
				}
			}
			if (colDef.GetColName()->Equals(columnName))
			{
				valueCol = i;
			}
		}
		i++;
	}
	if (keyCol == INVALID_INDEX || valueCol == INVALID_INDEX)
	{
		this->CloseReader(r);
		return nullptr;
	}
	Data::VariItem keyItem;
	Data::VariItem valItem;
	NN<Data::DataSet> ds;
	NEW_CLASSNN(ds, Data::DataSet());
	while (r->ReadNext())
	{
		if (!r->GetVariItem(keyCol, keyItem))
		{
			printf("TableData: Error in getting key column value\r\n");
		}
		else if (!r->GetVariItem(valueCol, valItem))
		{
			printf("TableData: Error in getting value column value\r\n");
		}
		else
		{
			ds->AddItem(keyItem, valItem);
		}
	}
	this->CloseReader(r);
	return ds;
}

Optional<Data::DataSet> Data::TableData::GetKeyDataSet()
{
	NN<DB::DBReader> r;
	if (!this->GetTableData().SetTo(r))
	{
		return nullptr;
	}
	UIntOS keyCol = INVALID_INDEX;
	DB::ColDef colDef(CSTR(""));
	UIntOS colCnt = r->ColCount();
	UIntOS i = 0;
	while (i < colCnt)
	{
		if (r->GetColDef(i, colDef))
		{
			if (colDef.IsPK())
			{
				if (keyCol == INVALID_INDEX)
				{
					keyCol = i;
				}
				else
				{
					this->CloseReader(r);
					return nullptr;
				}
			}
		}
		i++;
	}
	if (keyCol == INVALID_INDEX)
	{
		this->CloseReader(r);
		return nullptr;
	}
	Data::VariItem keyItem;
	Data::VariItem valItem;
	NN<Data::DataSet> ds;
	NEW_CLASSNN(ds, Data::DataSet());
	while (r->ReadNext())
	{
		if (!r->GetVariItem(keyCol, keyItem))
		{
			printf("TableData: Error in getting key column value\r\n");
		}
		else
		{
			valItem.Set(keyItem);
			ds->AddItem(keyItem, valItem);
		}
	}
	this->CloseReader(r);
	return ds;
}

void Data::TableData::CloseReader(NN<DB::DBReader> r)
{
	this->db->CloseReader(r);
}

void Data::TableData::SetCondition(Optional<Data::QueryConditions> cond)
{
	this->cond.Delete();
	this->cond = cond;
}

NN<Data::TableData> Data::TableData::Clone() const
{
	NN<Data::TableData> data;
	NEW_CLASSNN(data, TableData(this->db, false, OPTSTR_CSTR(this->schemaName), this->tableName->ToCString()));
	return data;
};

NN<Data::TableData> Data::TableData::CreateSubTable(NN<Data::QueryConditions> cond) const
{
	NN<Data::TableData> data = this->Clone();
	data->SetCondition(cond);
	return data;
}

UIntOS Data::TableData::GetRowCount()
{
	UIntOS cnt = 0;
	NN<DB::DBReader> r;
	if (this->GetTableData().SetTo(r))
	{
		while (r->ReadNext())
		{
			cnt++;
		}
		this->db->CloseReader(r);
	}
	return cnt;
}

UIntOS Data::TableData::GetRowCount(NN<Data::QueryConditions> cond)
{
	UIntOS cnt = 0;
	NN<DB::DBReader> r;
	if (this->GetTableData(cond).SetTo(r))
	{
		while (r->ReadNext())
		{
			cnt++;
		}
		this->db->CloseReader(r);
	}
	cond.Delete();
	return cnt;
}

Bool Data::TableData::GetFirstData(Text::CStringNN columnName, Optional<Data::QueryConditions> cond, NN<VariItem> outItem)
{
	NN<Data::QueryConditions> nncond;
	NN<DB::DBReader> r;
	Bool succ = false;
	if (cond.SetTo(nncond))
	{
		if (!this->GetTableData(nncond).SetTo(r))
		{
			nncond.Delete();
			return false;
		}
	}
	else
	{
		if (!this->GetTableData().SetTo(r))
		{
			return false;
		}
	}
	if (r->ReadNext())
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		UIntOS i = r->ColCount();
		while (i-- > 0)
		{
			if (r->GetName(i, sbuff).SetTo(sptr))
			{
				if (CSTRP(sbuff, sptr).Equals(columnName))
				{
					succ = r->GetVariItem(i, outItem);
					if (succ && outItem->GetItemType() == Data::VariItem::ItemType::CStr)
					{
						Optional<Text::String> s = outItem->GetAsNewString();
						outItem->SetStr(s);
						OPTSTR_DEL(s);
					}
					break;
				}
			}
		}
	}
	this->db->CloseReader(r);
	cond.Delete();
	return succ;
}
