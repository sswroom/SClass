#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/VariItem.h"
#include "Text/MyString.h"

Data::VariItem::VariItem(ItemType itemType, ItemValue val)
{
	this->itemType = itemType;
	this->val = val;
}

Data::VariItem::~VariItem()
{
	switch (this->itemType)
	{
	case IT_NULL:
	case IT_UNKNOWN:
	case IT_F32:
	case IT_F64:
	case IT_I8:
	case IT_U8:
	case IT_I16:
	case IT_U16:
	case IT_I32:
	case IT_U32:
	case IT_I64:
	case IT_U64:
	case IT_BOOL:
		break;
	case IT_STR:
		Text::StrDelNew(this->val.str);
		break;
	case IT_DATE:
		DEL_CLASS(this->val.date);
		break;
	case IT_BYTEARR:
		DEL_CLASS(this->val.byteArr);
		break;
	case IT_VECTOR:
		DEL_CLASS(this->val.vector);
		break;
	}
}

Data::VariItem::ItemType Data::VariItem::GetItemType()
{
	return this->itemType;
}

const Data::VariItem::ItemValue Data::VariItem::GetItemValue()
{
	return this->val;
}

Data::VariItem *Data::VariItem::NewNull()
{
	ItemValue ival;
	ival.str = 0;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_NULL, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewStr(const UTF8Char *str)
{
	ItemValue ival;
	ival.str = Text::StrCopyNew(str);
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_STR, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewDate(Data::DateTime *dt)
{
	ItemValue ival;
	NEW_CLASS(ival.date, Data::DateTime(dt));
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_DATE, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewF32(Single val)
{
	ItemValue ival;
	ival.f32 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_F32, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewF64(Double val)
{
	ItemValue ival;
	ival.f64 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_F64, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewI8(Int8 val)
{
	ItemValue ival;
	ival.i8 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_I8, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewU8(UInt8 val)
{
	ItemValue ival;
	ival.u8 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_U8, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewI16(Int16 val)
{
	ItemValue ival;
	ival.i16 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_I16, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewU16(UInt16 val)
{
	ItemValue ival;
	ival.u16 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_U16, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewI32(Int32 val)
{
	ItemValue ival;
	ival.i32 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_I32, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewU32(UInt32 val)
{
	ItemValue ival;
	ival.u32 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_U32, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewI64(Int64 val)
{
	ItemValue ival;
	ival.i64 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_I64, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewU64(UInt64 val)
{
	ItemValue ival;
	ival.u64 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_U64, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewBool(Bool val)
{
	ItemValue ival;
	ival.boolean = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_BOOL, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewByteArr(const UInt8 *arr, UOSInt cnt)
{
	ItemValue ival;
	NEW_CLASS(ival.byteArr, Data::ReadonlyArray<UInt8>(arr, cnt));
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_BYTEARR, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewVector(Math::Vector2D *vec)
{
	ItemValue ival;
	ival.vector = vec->Clone();
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(IT_VECTOR, ival));
	return item;
}
