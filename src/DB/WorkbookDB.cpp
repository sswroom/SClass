#include "Stdafx.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "DB/WorkbookDB.h"
#include "Text/MyStringW.h"

class WorkbookReader : public DB::DBReader
{
private:
	NN<Text::SpreadSheet::Worksheet> sheet;
	DB::TableDef *tabDef;
	Text::SpreadSheet::Worksheet::RowData *row;
	UOSInt currIndex;
	UOSInt maxIndex;
public:
	WorkbookReader(NN<Text::SpreadSheet::Worksheet> sheet, DB::TableDef *tabDef, UOSInt initOfst, UOSInt maxOfst)
	{
		this->sheet = sheet;
		this->tabDef = tabDef;
		this->row = 0;
		this->currIndex = initOfst;
		this->maxIndex = maxOfst;
	}

	virtual ~WorkbookReader()
	{
		DEL_CLASS(this->tabDef);
	}

	virtual Bool ReadNext()
	{
		this->currIndex++;
		if (this->currIndex > maxIndex)
		{
			this->row = 0;
			return false;
		}
		this->row = this->sheet->GetItem(this->currIndex);
		return this->row != 0;
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
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return 0;
		}
		return cell->cellValue->ToInt32();
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return 0;
		}
		return cell->cellValue->ToInt64();
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return 0;
		}
		return Text::StrUTF8_WChar(buff, cell->cellValue->v, 0);
	}

	virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		return this->sheet->GetCellString(cell, sb);
	}

	virtual Optional<Text::String> GetNewStr(UOSInt colIndex)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return 0;
		}
		return cell->cellValue->Clone().Ptr();
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return 0;
		}
		return cell->cellValue->ConcatToS(buff, buffSize);
	}

	virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return 0;
		}
		return Data::Timestamp::FromStr(cell->cellValue->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return 0;
		}
		return cell->cellValue->ToDouble();
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return false;
		}
		return cell->cellValue->v[0] == 'T' || cell->cellValue->v[0] == 't' || cell->cellValue->ToInt32() != 0;
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return 0;
		}
		return cell->cellValue->leng;
	}

	virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return 0;
		}
		MemCopyNO(buff, cell->cellValue->v, cell->cellValue->leng);
		return cell->cellValue->leng;
	}

	virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex)
	{
		return 0;
	}

	virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
	{
		return false;
	}

	virtual Bool GetVariItem(UOSInt colIndex, NN<Data::VariItem> item)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		NN<Text::String> s;
		if (cell == 0 || !s.Set(cell->cellValue))
		{
			return false;
		}
		item->SetStr(s);
		return true;
	}

	virtual Bool IsNull(UOSInt colIndex)
	{
		const Text::SpreadSheet::Worksheet::CellData *cell = this->sheet->GetCellDataRead(this->currIndex, colIndex);
		if (cell == 0 || cell->cellValue == 0)
		{
			return true;
		}
		return false;
	}

	virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
	{
		NN<DB::ColDef> col;
		if (this->tabDef->GetCol(colIndex).SetTo(col))
		{
			return col->GetColName()->ConcatTo(buff);
		}
		return 0;
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

Optional<DB::DBReader> DB::WorkbookDB::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	NN<Text::SpreadSheet::Worksheet> sheet;
	if (!this->wb->GetWorksheetByName(tableName).SetTo(sheet))
	{
		return 0;
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
	NEW_CLASSNN(r, WorkbookReader(sheet, GetTableDef(schemaName, tableName), dataOfst, endOfst));
	return r;
}

DB::TableDef *DB::WorkbookDB::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	NN<Text::SpreadSheet::Worksheet> sheet;
	if (!this->wb->GetWorksheetByName(tableName).SetTo(sheet))
	{
		return 0;
	}
	DB::TableDef *tabDef;
	NN<DB::ColDef> col;
	NEW_CLASS(tabDef, DB::TableDef(schemaName, sheet->GetName()->ToCString()));
	Text::SpreadSheet::Worksheet::RowData *row = sheet->GetItem(0);
	Text::SpreadSheet::Worksheet::CellData *cell;
	UOSInt i = 0;
	UOSInt j = row->cells->GetCount();
	while (i < j)
	{
		cell = row->cells->GetItem(i);
		NEW_CLASSNN(col, DB::ColDef(Text::String::OrEmpty(cell->cellValue)));
		col->SetColType(DB::DBUtil::ColType::CT_VarUTF8Char);
		col->SetColSize(256);
		tabDef->AddCol(col);
		i++;
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
