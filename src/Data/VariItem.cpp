#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/VariItem.h"
#include "Math/WKTWriter.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

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

void Data::VariItem::ToString(Text::StringBuilderUTF *sb)
{
	const UTF8Char *csptr;
	UTF8Char sbuff[64];
	switch (this->itemType)
	{
	case IT_UNKNOWN:
	case IT_NULL:
		sb->Append((const UTF8Char*)"null");
		return;
	case IT_F32:
		Text::SBAppendF32(sb, this->val.f32);
		return;
	case IT_F64:
		Text::SBAppendF64(sb, this->val.f64);
		return;
	case IT_I8:
		sb->AppendI16(this->val.i8);
		return;
	case IT_U8:
		sb->AppendU16(this->val.u8);
		return;
	case IT_I16:
		sb->AppendI16(this->val.i16);
		return;
	case IT_U16:
		sb->AppendU16(this->val.u16);
		return;
	case IT_I32:
		sb->AppendI32(this->val.i32);
		return;
	case IT_U32:
		sb->AppendU32(this->val.u32);
		return;
	case IT_I64:
		sb->AppendI64(this->val.i64);
		return;
	case IT_U64:
		sb->AppendU64(this->val.u64);
		return;
	case IT_BOOL:
		sb->Append(this->val.boolean?(const UTF8Char*)"true":(const UTF8Char*)"false");
		break;
	case IT_STR:
		csptr = Text::JSText::ToNewJSTextDQuote(this->val.str);
		sb->Append(csptr);
		Text::JSText::FreeNewText(csptr);
		return;
	case IT_DATE:
		this->val.date->ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb->AppendChar('\"', 1);
		sb->Append(sbuff);
		sb->AppendChar('\"', 1);
		break;
	case IT_BYTEARR:
		sb->AppendChar('\"', 1);
		sb->AppendHexBuff(this->val.byteArr->GetArray(), this->val.byteArr->GetCount(), 0, Text::LineBreakType::None);
		sb->AppendChar('\"', 1);
		return;
	case IT_VECTOR:
		{
			Math::WKTWriter writer;
			writer.GenerateWKT(sb, this->val.vector);
		}
		return;
	}
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
