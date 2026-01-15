#include "Stdafx.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "DB/WorkbookDB.h"
#include "Text/MyStringW.h"

class WorkbookReader : public DB::DBReader
{
private:
	NN<Text::SpreadSheet::Worksheet> sheet;
	NN<DB::TableDef> tabDef;
	Optional<Text::SpreadSheet::Worksheet::RowData> row;
	UOSInt currIndex;
	UOSInt maxIndex;
public:
	WorkbookReader(NN<Text::SpreadSheet::Worksheet> sheet, NN<DB::TableDef> tabDef, UOSInt initOfst, UOSInt maxOfst)
	{
		this->sheet = sheet;
		this->tabDef = tabDef;
		this->row = nullptr;
		this->currIndex = initOfst;
		this->maxIndex = maxOfst;
	}

	virtual ~WorkbookReader()
	{
		this->tabDef.Delete();
	}

	virtual Bool ReadNext()
	{
		this->currIndex++;
		if (this->currIndex > maxIndex)
		{
			this->row = nullptr;
			return false;
		}
		this->row = this->sheet->GetItem(this->currIndex);
		return this->row.NotNull();
	}

	virtual UOSInt ColCount()
	{
		return this->tabDef->GetColCnt();
	}

	virtual OSInt GetRowChanged()
	{
		return 0;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return 0;
		}
		return cellValue->ToInt32();
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return 0;
		}
		return cellValue->ToInt64();
	}

	virtual UnsafeArrayOpt<WChar> GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return nullptr;
		}
		return Text::StrUTF8_WChar(buff, cellValue->v, 0);
	}

	virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return false;
		}
		sb->Append(cellValue);
		return true;
	}

	virtual Optional<Text::String> GetNewStr(UOSInt colIndex)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return nullptr;
		}
		return cellValue->Clone();
	}

	virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return nullptr;
		}
		return cellValue->ConcatToS(buff, buffSize);
	}

	virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return 0;
		}
		return Data::Timestamp::FromStr(cellValue->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	}

	virtual Double GetDblOrNAN(UOSInt colIndex)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return NAN;
		}
		return cellValue->ToDoubleOrNAN();
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return 0;
		}
		return cellValue->v[0] == 'T' || cellValue->v[0] == 't' || cellValue->ToInt32() != 0;
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return 0;
		}
		return cellValue->leng;
	}

	virtual UOSInt GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return 0;
		}
		MemCopyNO(buff.Ptr(), cellValue->v.Ptr(), cellValue->leng);
		return cellValue->leng;
	}

	virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex)
	{
		return nullptr;
	}

	virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
	{
		return false;
	}

	virtual Bool GetVariItem(UOSInt colIndex, NN<Data::VariItem> item)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return 0;
		}
		item->SetStr(cellValue);
		return true;
	}

	virtual Bool IsNull(UOSInt colIndex)
	{
		NN<const Text::SpreadSheet::Worksheet::CellData> cell;
		NN<Text::String> cellValue;
		if (!this->sheet->GetCellDataRead(this->currIndex, colIndex).SetTo(cell) || !cell->cellValue.SetTo(cellValue))
		{
			return true;
		}
		return false;
	}

	virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
	{
		NN<DB::ColDef> col;
		if (this->tabDef->GetCol(colIndex).SetTo(col))
		{
			return col->GetColName()->ConcatTo(buff);
		}
		return nullptr;
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
	{
		NN<DB::ColDef> col;
		if (this->tabDef->GetCol(colIndex).SetTo(col))
		{
			colSize.Set(col->GetColSize());
			return col->GetColType();
		}
		return DB::DBUtil::ColType::CT_Unknown;
	}

	virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
	{
		NN<DB::ColDef> col;
		if (this->tabDef->GetCol(colIndex).SetTo(col))
		{
			colDef->Set(col);
			return true;
		}
		return false;
		
	}
};

DB::WorkbookDB::WorkbookDB(NN<Text::SpreadSheet::Workbook> wb) : DB::ReadingDB(wb->GetSourceNameObj())
{
	this->wb = wb;
}

DB::WorkbookDB::~WorkbookDB()
{
	this->wb.Delete();
}

UOSInt DB::WorkbookDB::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
	{
		return 0;
	}
	Data::ArrayIterator<NN<Text::SpreadSheet::Worksheet>> it = this->wb->Iterator();
	while (it.HasNext())
	{
		names->Add(it.Next()->GetName()->Clone());
	}
	return this->wb->GetCount();
}

Optional<DB::DBReader> DB::WorkbookDB::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<Text::SpreadSheet::Worksheet> sheet;
	NN<DB::TableDef> tabDef;
	if (!this->wb->GetWorksheetByName(tableName).SetTo(sheet))
	{
		return nullptr;
	}
	if (!GetTableDef(schemaName, tableName).SetTo(tabDef))
	{
		return nullptr;
	}
	NN<WorkbookReader> r;
	UOSInt endOfst;
	if (maxCnt == 0)
	{
		endOfst = sheet->GetCount();
	}
	else
	{
		endOfst = dataOfst + maxCnt;
		if (endOfst > sheet->GetCount())
		{
			endOfst = sheet->GetCount();
		}
	}
	NEW_CLASSNN(r, WorkbookReader(sheet, tabDef, dataOfst, endOfst));
	return r;
}

Optional<DB::TableDef> DB::WorkbookDB::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<Text::SpreadSheet::Worksheet> sheet;
	if (!this->wb->GetWorksheetByName(tableName).SetTo(sheet))
	{
		return nullptr;
	}
	DB::TableDef *tabDef;
	NN<DB::ColDef> col;
	NEW_CLASS(tabDef, DB::TableDef(schemaName, sheet->GetName()->ToCString()));
	NN<Text::SpreadSheet::Worksheet::RowData> row;
	NN<Text::SpreadSheet::Worksheet::CellData> cell;
	if (sheet->GetItem(0).SetTo(row))
	{
		UOSInt i = 0;
		UOSInt j = row->cells.GetCount();
		while (i < j)
		{
			if (row->cells.GetItem(i).SetTo(cell))
			{
				NEW_CLASSNN(col, DB::ColDef(Text::String::OrEmpty(cell->cellValue)));
				col->SetColType(DB::DBUtil::ColType::CT_VarUTF8Char);
				col->SetColSize(256);
				tabDef->AddCol(col);
			}
			else
			{
				NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
				col->SetColType(DB::DBUtil::ColType::CT_VarUTF8Char);
				col->SetColSize(256);
				tabDef->AddCol(col);
			}
			i++;
		}
	}
	return tabDef;
}

void DB::WorkbookDB::CloseReader(NN<DBReader> r)
{
	WorkbookReader *reader = (WorkbookReader*)r.Ptr();
	DEL_CLASS(reader);
}

void DB::WorkbookDB::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{

}

void DB::WorkbookDB::Reconnect()
{

}
