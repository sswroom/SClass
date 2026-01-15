#include "Stdafx.h"
#include "Data/ArrayListStringNN.h"
#include "Data/StringMapObj.hpp"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Data/FieldComparator.h"
#include "DB/ColDef.h"
#include "DB/SortableDBReader.h"
#include "Text/MyStringW.h"

Optional<Data::VariItem> DB::SortableDBReader::GetItem(UIntOS colIndex)
{
	NN<Data::VariObject> obj;
	if (!this->objList.GetItem(this->currIndex).SetTo(obj))
		return nullptr;
	NN<DB::ColDef> col;
	if (!this->cols.GetItem(colIndex).SetTo(col))
		return nullptr;
	return obj->GetItem(col->GetColName()->v);
}

DB::SortableDBReader::SortableDBReader(NN<DB::ReadingDB> db, Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	this->currIndex = INVALID_INDEX;
	UIntOS i;
	UIntOS j;
	DB::ColDef colDef(CSTR(""));
	NN<Data::VariObject> obj;
	NN<Data::ArrayListStringNN> nncolNames;
	NN<Data::QueryConditions> nncondition;
	if (!colNames.SetTo(nncolNames) || nncolNames->GetCount() == 0)
	{
		NN<DB::DBReader> r;
		if (!db->QueryTableData(schemaName, tableName, nullptr, 0, 0, nullptr, nullptr).SetTo(r))
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
			Bool valid;
			if (!condition.SetTo(nncondition) || !nncondition->IsValid(obj, valid) || valid)
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
		j = nncolNames->GetCount();
		while (i < j)
		{
			NN<Text::String> colName;
			if (nncolNames->GetItem(i).SetTo(colName) && dbColNames.SortedIndexOf(colName) < 0)
			{
				dbColNames.SortedInsert(colName);
			}
			i++;
		}
		if (condition.SetTo(nncondition))
		{
			Data::ArrayListStringNN condColNames;
			nncondition->GetFieldList(condColNames);
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
		if (!db->QueryTableData(schemaName, tableName, &dbColNames, 0, 0, nullptr, nullptr).SetTo(r))
		{
			return;
		}

		Data::StringMapObj<DB::ColDef*> tmpCols;
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
			Bool valid;
			if (!condition.SetTo(nncondition) || !nncondition->IsValid(obj, valid) || valid)
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
		Data::ArrayIterator<NN<Text::String>> it = nncolNames->Iterator();
		while (it.HasNext())
		{
			col = tmpCols.GetNN(it.Next());
			if (col)
			{
				this->cols.Add(col->Clone());
			}
			i++;
		}

		NN<const Data::ArrayListObj<DB::ColDef *>> colList = tmpCols.GetValues();
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
		Data::Sort::ArtificialQuickSort::Sort<NN<Data::VariObject>>(this->objList, comparator);
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
	UIntOS i;
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

UIntOS DB::SortableDBReader::ColCount()
{
	return this->cols.GetCount();
}

IntOS DB::SortableDBReader::GetRowChanged()
{
	return 0;
}

Int32 DB::SortableDBReader::GetInt32(UIntOS colIndex)
{
	NN<Data::VariItem> item;
	if (!this->GetItem(colIndex).SetTo(item))
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

Int64 DB::SortableDBReader::GetInt64(UIntOS colIndex)
{
	NN<Data::VariItem> item;
	if (!this->GetItem(colIndex).SetTo(item))
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

UnsafeArrayOpt<WChar> DB::SortableDBReader::GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, sb))
	{
		return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
	}
	return nullptr;
}
Bool DB::SortableDBReader::GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
{
	NN<Data::VariItem> item;
	if (!this->GetItem(colIndex).SetTo(item))
	{
		return false;
	}
	item->ToString(sb);
	return true;
}

Optional<Text::String> DB::SortableDBReader::GetNewStr(UIntOS colIndex)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, sb))
	{
		return Text::String::New(sb.ToCString());
	}
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> DB::SortableDBReader::GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
{
	Text::StringBuilderUTF8 sb;
	if (this->GetStr(colIndex, sb))
	{
		return Text::StrConcatS(buff, sb.ToString(), buffSize);
	}
	return nullptr;
}

Data::Timestamp DB::SortableDBReader::GetTimestamp(UIntOS colIndex)
{
	NN<Data::VariItem> item;
	if (this->GetItem(colIndex).SetTo(item) && item->GetItemType() == Data::VariItem::ItemType::Timestamp)
	{
		return item->GetItemValue().ts;
	}
	else
	{
		return Data::Timestamp(0);
	}
}

Double DB::SortableDBReader::GetDblOrNAN(UIntOS colIndex)
{
	NN<Data::VariItem> item;
	if (!this->GetItem(colIndex).SetTo(item))
	{
		return NAN;
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
		return NAN;
	}
}

Bool DB::SortableDBReader::GetBool(UIntOS colIndex)
{
	return this->GetInt32(colIndex) != 0;
}

UIntOS DB::SortableDBReader::GetBinarySize(UIntOS colIndex)
{
	NN<Data::VariItem> item;
	if (this->GetItem(colIndex).SetTo(item) && item->GetItemType() == Data::VariItem::ItemType::ByteArr)
	{
		return item->GetItemValue().byteArr->GetCount();
	}
	return 0;
}

UIntOS DB::SortableDBReader::GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
{
	NN<Data::VariItem> item;
	if (this->GetItem(colIndex).SetTo(item) && item->GetItemType() == Data::VariItem::ItemType::ByteArr)
	{
		Data::ReadonlyArray<UInt8> *arr = item->GetItemValue().byteArr;
		MemCopyNO(buff.Ptr(), arr->GetArray(), arr->GetCount());
		return arr->GetCount();
	}
	return 0;
}

Optional<Math::Geometry::Vector2D> DB::SortableDBReader::GetVector(UIntOS colIndex)
{
	NN<Data::VariItem> item;
	if (this->GetItem(colIndex).SetTo(item) && item->GetItemType() == Data::VariItem::ItemType::Vector)
	{
		return item->GetItemValue().vector->Clone();
	}
	return nullptr;
}

Bool DB::SortableDBReader::GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
{
	NN<Data::VariItem> item;
	if (this->GetItem(colIndex).SetTo(item) && item->GetItemType() == Data::VariItem::ItemType::UUID)
	{
		uuid->SetValue(item->GetItemValue().uuid);
		return true;
	}
	return false;
}

Bool DB::SortableDBReader::GetVariItem(UIntOS colIndex, NN<Data::VariItem> item)
{
	NN<Data::VariItem> dataItem;
	if (this->GetItem(colIndex).SetTo(dataItem))
	{
		item->Set(dataItem);
		return true;
	}
	return false;
}

Bool DB::SortableDBReader::IsNull(UIntOS colIndex)
{
	NN<Data::VariItem> item;
	if (!this->GetItem(colIndex).SetTo(item))
	{
		return true;
	}
	return item->GetItemType() == Data::VariItem::ItemType::Null;
}

UnsafeArrayOpt<UTF8Char> DB::SortableDBReader::GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
{
	NN<DB::ColDef> col;
	if (this->cols.GetItem(colIndex).SetTo(col))
	{
		NN<Text::String> colName = col->GetColName();
		return Text::StrConcatC(buff, colName->v, colName->leng);
	}
	return nullptr;
}

DB::DBUtil::ColType DB::SortableDBReader::GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
{
	NN<DB::ColDef> col;
	if (this->cols.GetItem(colIndex).SetTo(col))
	{
		colSize.Set(col->GetColSize());
		return col->GetColType();
	}
	return DB::DBUtil::CT_Unknown;
}

Bool DB::SortableDBReader::GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
{
	NN<DB::ColDef> c;
	if (!this->cols.GetItem(colIndex).SetTo(c))
	{
		return false;
	}
	colDef->Set(c);
	return true;
}
