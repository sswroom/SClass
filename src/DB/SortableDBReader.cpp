#include "Stdafx.h"
#include "Data/ArrayListStringNN.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/FieldComparator.h"
#include "DB/ColDef.h"
#include "DB/SortableDBReader.h"
#include "Text/MyStringW.h"

Data::VariItem *DB::SortableDBReader::GetItem(UOSInt colIndex)
{
	NN<Data::VariObject> obj;
	if (!this->objList.GetItem(this->currIndex).SetTo(obj))
		return 0;
	NN<DB::ColDef> col;
	if (!this->cols.GetItem(colIndex).SetTo(col))
		return 0;
	return obj->GetItem(col->GetColName()->v);
}

DB::SortableDBReader::SortableDBReader(DB::ReadingDB *db, Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	this->currIndex = INVALID_INDEX;
	UOSInt i;
	UOSInt j;
	DB::ColDef colDef(CSTR_NULL);
	NN<Data::VariObject> obj;
	if (colNames == 0 || colNames->GetCount() == 0)
	{
		NN<DB::DBReader> r;
		if (!db->QueryTableData(schemaName, tableName, 0, 0, 0, CSTR_NULL, 0).SetTo(r))
		{
			return;
		}

		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			if (r->GetColDef(i, colDef))
			{
				this->cols.Add(colDef.Clone());
			}
			i++;
		}
		while (r->ReadNext())
		{
			obj = r->CreateVariObject();
			if (condition == 0 || condition->IsValid(obj))
			{
				this->objList.Add(obj);
			}
			else
			{
				obj.Delete();
			}
		}
		db->CloseReader(r);
	}
	else
	{
		Data::ArrayListStringNN dbColNames;
		i = 0;
		j = colNames->GetCount();
		while (i < j)
		{
			NN<Text::String> colName;
			if (colNames->GetItem(i).SetTo(colName) && dbColNames.SortedIndexOf(colName) < 0)
			{
				dbColNames.SortedInsert(colName);
			}
			i++;
		}
		if (condition)
		{
			Data::ArrayListStringNN condColNames;
			condition->GetFieldList(condColNames);
			i = 0;
			j = condColNames.GetCount();
			while (i < j)
			{
				NN<Text::String> colName;
				if (condColNames.GetItem(i).SetTo(colName) && dbColNames.SortedIndexOf(colName) < 0)
				{
					dbColNames.SortedInsert(colName);
				}
				i++;
			}
		}
		NN<DB::DBReader> r;
		if (!db->QueryTableData(schemaName, tableName, &dbColNames, 0, 0, CSTR_NULL, 0).SetTo(r))
		{
			return;
		}

		Data::StringMap<DB::ColDef*> tmpCols;
		i = 0;
		j = r->ColCount();
		while (i < j)
		{
			if (r->GetColDef(i, colDef))
			{
				tmpCols.PutNN(colDef.GetColName(), colDef.Clone().Ptr());
			}
			i++;
		}
		while (r->ReadNext())
		{
			obj = r->CreateVariObject();
			if (condition == 0 || condition->IsValid(obj))
			{
				this->objList.Add(obj);
			}
			else
			{
				obj.Delete();
			}
		}
		db->CloseReader(r);

		DB::ColDef *col;
		Data::ArrayIterator<NN<Text::String>> it = colNames->Iterator();
		while (it.HasNext())
		{
			col = tmpCols.GetNN(it.Next());
			if (col)
			{
				this->cols.Add(col->Clone());
			}
			i++;
		}

		NN<const Data::ArrayList<DB::ColDef *>> colList = tmpCols.GetValues();
		i = colList->GetCount();
		while (i-- > 0)
		{
			col = colList->GetItem(i);
			DEL_CLASS(col);
		}
		tmpCols.Clear();
	}
	if (ordering.leng > 0)
	{
		Data::FieldComparator comparator(ordering);
		Data::Sort::ArtificialQuickSort::Sort<NN<Data::VariObject>>(&this->objList, comparator);
	}
	if (dataOfst > 0)
	{
		if (dataOfst >= this->objList.GetCount())
		{
			dataOfst = this->objList.GetCount();
			while (dataOfst-- > 0)
			{
				this->objList.GetItem(dataOfst).Delete();
			}
			this->objList.Clear();
		}
		else
		{
			i = dataOfst;
			while (i-- > 0)
			{
				this->objList.GetItem(i).Delete();
			}
			this->objList.RemoveRange(0, dataOfst);
		}
	}
	if (maxCnt != 0 && maxCnt < this->objList.GetCount())
	{
		i = this->objList.GetCount();
		while (i-- > maxCnt)
		{
			if (this->objList.RemoveAt(i).SetTo(obj))
				obj.Delete();
		}
	}
}

DB::SortableDBReader::~SortableDBReader()
{
	UOSInt i;
	i = this->objList.GetCount();
	while (i-- > 0)
	{
		this->objList.GetItem(i).Delete();
	}
	i = this->cols.GetCount();
	while (i-- > 0)
	{
		this->cols.GetItem(i).Delete();
	}
}

Bool DB::SortableDBReader::ReadNext()
{
	if (this->currIndex == this->objList.GetCount())
	{
		return false;
	}
	this->currIndex++;
	if (this->currIndex == this->objList.GetCount())
	{
		return false;
	}
	return true;
}

UOSInt DB::SortableDBReader::ColCount()
{
	return this->cols.GetCount();
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
	case Data::VariItem::ItemType::NI32:
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
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Timestamp:
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
	case Data::VariItem::ItemType::NI32:
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
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Timestamp:
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
	if (this->GetStr(colIndex, sb))
	{
		return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
	}
	return 0;
}
Bool DB::SortableDBReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item == 0)
	{
		return false;
	}
	item->ToString(sb);
	return true;
}

Optional<Text::String> DB::SortableDBReader::GetNewStr(UOSInt colIndex)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, sb))
	{
		return Text::String::New(sb.ToCString());
	}
	return 0;
}

UTF8Char *DB::SortableDBReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, sb))
	{
		return Text::StrConcatS(buff, sb.ToString(), buffSize);
	}
	return 0;
}

Data::Timestamp DB::SortableDBReader::GetTimestamp(UOSInt colIndex)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item && item->GetItemType() == Data::VariItem::ItemType::Timestamp)
	{
		return item->GetItemValue().ts;
	}
	else
	{
		return Data::Timestamp(0);
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
	case Data::VariItem::ItemType::NI32:
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
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Timestamp:
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

Optional<Math::Geometry::Vector2D> DB::SortableDBReader::GetVector(UOSInt colIndex)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item && item->GetItemType() == Data::VariItem::ItemType::Vector)
	{
		return item->GetItemValue().vector->Clone();
	}
	return 0;
}

Bool DB::SortableDBReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	Data::VariItem *item = this->GetItem(colIndex);
	if (item && item->GetItemType() == Data::VariItem::ItemType::UUID)
	{
		uuid->SetValue(item->GetItemValue().uuid);
		return true;
	}
	return false;
}

Bool DB::SortableDBReader::GetVariItem(UOSInt colIndex, NN<Data::VariItem> item)
{
	Data::VariItem *dataItem = this->GetItem(colIndex);
	if (dataItem)
	{
		item->Set(item);
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
	NN<DB::ColDef> col;
	if (this->cols.GetItem(colIndex).SetTo(col))
	{
		NN<Text::String> colName = col->GetColName();
		return Text::StrConcatC(buff, colName->v, colName->leng);
	}
	return 0;
}

DB::DBUtil::ColType DB::SortableDBReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	NN<DB::ColDef> col;
	if (this->cols.GetItem(colIndex).SetTo(col))
	{
		colSize.Set(col->GetColSize());
		return col->GetColType();
	}
	return DB::DBUtil::CT_Unknown;
}

Bool DB::SortableDBReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	NN<DB::ColDef> c;
	if (!this->cols.GetItem(colIndex).SetTo(c))
	{
		return false;
	}
	colDef->Set(c);
	return true;
}
