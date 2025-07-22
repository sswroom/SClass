#include "Stdafx.h"
#include "Data/TableData.h"
#include "DB/ColDef.h"

Data::TableData::TableData(NN<DB::ReadingDB> db, Bool needRelease, Text::CString schemaName, Text::CStringNN tableName)
{
	this->db = db;
	this->needRelease = needRelease;
	this->schemaName = Text::String::NewOrNull(schemaName);
	this->tableName = Text::String::New(tableName);
}

Data::TableData::~TableData()
{
	if (this->needRelease)
		this->db.Delete();
	OPTSTR_DEL(this->schemaName);
	this->tableName->Release();
}

Optional<DB::DBReader> Data::TableData::GetTableData()
{
	return this->db->QueryTableData(OPTSTR_CSTR(this->schemaName), this->tableName->ToCString(), 0, 0, 0, 0, 0);
}

Bool Data::TableData::GetColumnDataStr(Text::CStringNN columnName, NN<Data::ArrayListStringNN> str)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::DBReader> r;
	if (!this->GetTableData().SetTo(r))
		return false;
	UOSInt col = INVALID_INDEX;
	UOSInt i = 0;
	UOSInt colCnt = r->ColCount();
	while (i < colCnt)
	{
		if (r->GetName(i, sbuff).SetTo(sptr))
		{
			if (columnName.Equals(sbuff, (UOSInt)(sptr - sbuff)))
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
		return 0;
	}
	UOSInt keyCol = INVALID_INDEX;
	UOSInt valueCol = INVALID_INDEX;
	DB::ColDef colDef(CSTR(""));
	UOSInt colCnt = r->ColCount();
	UOSInt i = 0;
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
					return 0;
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
		return 0;
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

void Data::TableData::CloseReader(NN<DB::DBReader> r)
{
	this->db->CloseReader(r);
}
