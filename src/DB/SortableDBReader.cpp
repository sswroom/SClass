#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/FieldComparator.h"
#include "DB/ColDef.h"
#include "DB/SortableDBReader.h"

Data::VariItem *DB::SortableDBReader::GetItem(UOSInt colIndex)
{
	Data::VariObject *obj = this->objList->GetItem(this->currIndex);
	if (obj == 0)
		return 0;
	return obj->GetItem(this->cols->GetItem(colIndex)->GetColName());
}

DB::SortableDBReader::SortableDBReader(DB::ReadingDB *db, const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *colNames, UOSInt dataOfst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	this->currIndex = INVALID_INDEX;
	this->objList = 0;
	this->cols = 0;
	UOSInt i;
	UOSInt j;
	DB::ColDef colDef(0);
	Data::VariObject *obj;
	if (colNames == 0 || colNames->GetCount() == 0)
	{
		DB::DBReader *r = db->GetTableData(tableName, 0, 0, 0, 0, 0);
		if (r == 0)
		{
			return;
		}

		NEW_CLASS(this->cols, Data::ArrayList<ColDef*>());
		NEW_CLASS(this->objList, Data::ArrayList<Data::VariObject*>());
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			if (r->GetColDef(i, &colDef))
			{
				this->cols->Add(colDef.Clone());
			}
			i++;
		}
		while (r->ReadNext())
		{
			obj = r->CreateVariObject();
			if (obj)
			{
				if (condition == 0 || condition->IsValid(obj))
				{
					this->objList->Add(obj);
				}
				else
				{
					DEL_CLASS(obj);
				}
			}
		}
		db->CloseReader(r);
	}
	else
	{
		Data::ArrayListStrUTF8 dbColNames;
		i = 0;
		j = colNames->GetCount();
		while (i < j)
		{
			if (dbColNames.SortedIndexOf(colNames->GetItem(i)) < 0)
			{
				dbColNames.SortedInsert(colNames->GetItem(i));
			}
			i++;
		}
		if (condition)
		{
			Data::ArrayList<const UTF8Char*> condColNames;
			condition->GetFieldList(&condColNames);
			i = 0;
			j = condColNames.GetCount();
			while (i < j)
			{
				if (dbColNames.SortedIndexOf(condColNames.GetItem(i)) < 0)
				{
					dbColNames.SortedInsert(condColNames.GetItem(i));
				}
				i++;
			}
		}
		DB::DBReader *r = db->GetTableData(tableName, &dbColNames, 0, 0, 0, 0);
		if (r == 0)
		{
			return;
		}

		NEW_CLASS(this->cols, Data::ArrayList<ColDef*>());
		NEW_CLASS(this->objList, Data::ArrayList<Data::VariObject*>());
		Data::StringUTF8Map<DB::ColDef*> tmpCols;
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			if (r->GetColDef(i, &colDef))
			{
				tmpCols.Put(colDef.GetColName(), colDef.Clone());
			}
			i++;
		}
		while (r->ReadNext())
		{
			obj = r->CreateVariObject();
			if (obj)
			{
				if (condition == 0 || condition->IsValid(obj))
				{
					this->objList->Add(obj);
				}
				else
				{
					DEL_CLASS(obj);
				}
			}
		}
		db->CloseReader(r);

		DB::ColDef *col;
		i = 0;
		j = colNames->GetCount();
		while (i < j)
		{
			col = tmpCols.Get(colNames->GetItem(i));
			if (col)
			{
				this->cols->Add(col->Clone());
			}
			i++;
		}

		Data::ArrayList<DB::ColDef *> *colList = tmpCols.GetValues();
		i = colList->GetCount();
		while (i-- > 0)
		{
			col = colList->GetItem(i);
			DEL_CLASS(col);
		}
		tmpCols.Clear();
	}
	if (ordering)
	{
		Data::FieldComparator comparator(ordering);
		Data::Sort::ArtificialQuickSort::Sort(this->objList->GetArray(&i), &comparator, 0, (OSInt)this->objList->GetCount() - 1);
	}
	if (dataOfst > 0)
	{
		if (dataOfst >= this->objList->GetCount())
		{
			dataOfst = this->objList->GetCount();
			while (dataOfst-- > 0)
			{
				obj = this->objList->GetItem(dataOfst);
				DEL_CLASS(obj);
			}
			this->objList->Clear();
		}
		else
		{
			i = dataOfst;
			while (i-- > 0)
			{
				obj = this->objList->GetItem(i);
				DEL_CLASS(obj);
			}
			this->objList->RemoveRange(0, dataOfst);
		}
	}
	if (maxCnt != 0 && maxCnt < this->objList->GetCount())
	{
		i = this->objList->GetCount();
		while (i-- > maxCnt)
		{
			obj = this->objList->RemoveAt(i);
			DEL_CLASS(obj);
		}
	}
}

DB::SortableDBReader::~SortableDBReader()
{
	UOSInt i;
	Data::VariObject *obj;
	DB::ColDef *col;
	if (this->objList)
	{
		i = this->objList->GetCount();
		while (i-- > 0)
		{
			obj = this->objList->GetItem(i);
			DEL_CLASS(obj);
		}
		DEL_CLASS(this->objList);
	}
	if (this->cols)
	{
		i = this->cols->GetCount();
		while (i-- > 0)
		{
			col = this->cols->GetItem(i);
			DEL_CLASS(col);
		}
		DEL_CLASS(this->cols);
	}
}

Bool DB::SortableDBReader::ReadNext()
{
	if (this->objList == 0 || this->cols == 0)
	{
		return false;
	}
	if (this->currIndex == this->objList->GetCount())
	{
		return false;
	}
	this->currIndex++;
	if (this->currIndex == this->objList->GetCount())
	{
		return false;
	}
	return true;
}

UOSInt DB::SortableDBReader::ColCount()
{
	if (this->cols)
	{
		return this->cols->GetCount();
	}
	return 0;
}

OSInt DB::SortableDBReader::GetRowChanged()
{
	return 0;
}

Int32 DB::SortableDBReader::GetInt32(UOSInt colIndex)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item == 0)
	{
		return 0;
	}
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
		return (Int32)item->GetItemValue().f32;
	case Data::VariItem::ItemType::F64:
		return (Int32)item->GetItemValue().f64;
	case Data::VariItem::ItemType::I8:
		return (Int32)item->GetItemValue().i8;
	case Data::VariItem::ItemType::U8:
		return (Int32)item->GetItemValue().u8;
	case Data::VariItem::ItemType::I16:
		return (Int32)item->GetItemValue().i16;
	case Data::VariItem::ItemType::U16:
		return (Int32)item->GetItemValue().u16;
	case Data::VariItem::ItemType::I32:
		return (Int32)item->GetItemValue().i32;
	case Data::VariItem::ItemType::U32:
		return (Int32)item->GetItemValue().u32;
	case Data::VariItem::ItemType::I64:
		return (Int32)item->GetItemValue().i64;
	case Data::VariItem::ItemType::U64:
		return (Int32)item->GetItemValue().u64;
	case Data::VariItem::ItemType::BOOL:
		return item->GetItemValue().boolean?1:0;

	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return 0;
	}
}

Int64 DB::SortableDBReader::GetInt64(UOSInt colIndex)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item == 0)
	{
		return 0;
	}
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
		return (Int64)item->GetItemValue().f32;
	case Data::VariItem::ItemType::F64:
		return (Int64)item->GetItemValue().f64;
	case Data::VariItem::ItemType::I8:
		return (Int64)item->GetItemValue().i8;
	case Data::VariItem::ItemType::U8:
		return (Int64)item->GetItemValue().u8;
	case Data::VariItem::ItemType::I16:
		return (Int64)item->GetItemValue().i16;
	case Data::VariItem::ItemType::U16:
		return (Int64)item->GetItemValue().u16;
	case Data::VariItem::ItemType::I32:
		return (Int64)item->GetItemValue().i32;
	case Data::VariItem::ItemType::U32:
		return (Int64)item->GetItemValue().u32;
	case Data::VariItem::ItemType::I64:
		return (Int64)item->GetItemValue().i64;
	case Data::VariItem::ItemType::U64:
		return (Int64)item->GetItemValue().u64;
	case Data::VariItem::ItemType::BOOL:
		return item->GetItemValue().boolean?1:0;

	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return 0;
	}
}

WChar *DB::SortableDBReader::GetStr(UOSInt colIndex, WChar *buff)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, &sb))
	{
		return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
	}
	return 0;
}
Bool DB::SortableDBReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item == 0)
	{
		return false;
	}
	item->ToString(sb);
	return true;
}

const UTF8Char *DB::SortableDBReader::GetNewStr(UOSInt colIndex)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, &sb))
	{
		return Text::StrCopyNew(sb.ToString());
	}
	return 0;
}

UTF8Char *DB::SortableDBReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, &sb))
	{
		return Text::StrConcatS(buff, sb.ToString(), buffSize);
	}
	return 0;
}

DB::DBReader::DateErrType DB::SortableDBReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item && item->GetItemType() == Data::VariItem::ItemType::Date)
	{
		outVal->SetValue(item->GetItemValue().date);
		return DB::DBReader::DET_OK;
	}
	else if (item && item->GetItemType() == Data::VariItem::ItemType::Null)
	{
		return DB::DBReader::DET_NULL;
	}
	else
	{
		return DB::DBReader::DET_ERROR;
	}
}

Double DB::SortableDBReader::GetDbl(UOSInt colIndex)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item == 0)
	{
		return 0;
	}
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::F32:
		return item->GetItemValue().f32;
	case Data::VariItem::ItemType::F64:
		return item->GetItemValue().f64;
	case Data::VariItem::ItemType::I8:
		return (Double)item->GetItemValue().i8;
	case Data::VariItem::ItemType::U8:
		return (Double)item->GetItemValue().u8;
	case Data::VariItem::ItemType::I16:
		return (Double)item->GetItemValue().i16;
	case Data::VariItem::ItemType::U16:
		return (Double)item->GetItemValue().u16;
	case Data::VariItem::ItemType::I32:
		return (Double)item->GetItemValue().i32;
	case Data::VariItem::ItemType::U32:
		return (Double)item->GetItemValue().u32;
	case Data::VariItem::ItemType::I64:
		return (Double)item->GetItemValue().i64;
	case Data::VariItem::ItemType::U64:
		return (Double)item->GetItemValue().u64;
	case Data::VariItem::ItemType::BOOL:
		return item->GetItemValue().boolean?1:0;

	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return 0;
	}
}

Bool DB::SortableDBReader::GetBool(UOSInt colIndex)
{
	return this->GetInt32(colIndex) != 0;
}

UOSInt DB::SortableDBReader::GetBinarySize(UOSInt colIndex)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item && item->GetItemType() == Data::VariItem::ItemType::ByteArr)
	{
		return item->GetItemValue().byteArr->GetCount();
	}
	return 0;
}

UOSInt DB::SortableDBReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item && item->GetItemType() == Data::VariItem::ItemType::ByteArr)
	{
		Data::ReadonlyArray<UInt8> *arr = item->GetItemValue().byteArr;
		MemCopyNO(buff, arr->GetArray(), arr->GetCount());
		return arr->GetCount();
	}
	return 0;
}

Math::Vector2D *DB::SortableDBReader::GetVector(UOSInt colIndex)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item && item->GetItemType() == Data::VariItem::ItemType::Vector)
	{
		return item->GetItemValue().vector->Clone();
	}
	return 0;
}

Bool DB::SortableDBReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item && item->GetItemType() == Data::VariItem::ItemType::UUID)
	{
		uuid->SetValue(item->GetItemValue().uuid);
		return true;
	}
	return false;
}

Bool DB::SortableDBReader::IsNull(UOSInt colIndex)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item == 0)
	{
		return true;
	}
	return item->GetItemType() == Data::VariItem::ItemType::Null;
}

UTF8Char *DB::SortableDBReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	DB::ColDef *col = this->cols->GetItem(colIndex);
	if (col)
	{
		return Text::StrConcat(buff, col->GetColName());
	}
	return 0;
}

DB::DBUtil::ColType DB::SortableDBReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	DB::ColDef *col = this->cols->GetItem(colIndex);
	if (col)
	{
		return col->GetColType();
	}
	return DB::DBUtil::CT_Unknown;
}

Bool DB::SortableDBReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	DB::ColDef *c = this->cols->GetItem(colIndex);
	if (c == 0)
	{
		return false;
	}
	colDef->Set(c);
	return true;
}

void DB::SortableDBReader::DelNewStr(const UTF8Char *s)
{
	Text::StrDelNew(s);
}
