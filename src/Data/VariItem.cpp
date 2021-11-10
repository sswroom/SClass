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
	case ItemType::Null:
	case ItemType::Unknown:
	case ItemType::F32:
	case ItemType::F64:
	case ItemType::I8:
	case ItemType::U8:
	case ItemType::I16:
	case ItemType::U16:
	case ItemType::I32:
	case ItemType::U32:
	case ItemType::I64:
	case ItemType::U64:
	case ItemType::BOOL:
		break;
	case ItemType::Str:
		Text::StrDelNew(this->val.str);
		break;
	case ItemType::Date:
		DEL_CLASS(this->val.date);
		break;
	case ItemType::ByteArr:
		DEL_CLASS(this->val.byteArr);
		break;
	case ItemType::Vector:
		DEL_CLASS(this->val.vector);
		break;
	case ItemType::UUID:
		DEL_CLASS(this->val.uuid);
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

Data::VariItem *Data::VariItem::Clone()
{
	ItemValue ival;
	switch (this->itemType)
	{
	case ItemType::Null:
		ival.str = 0;
		break;
	default:
	case ItemType::Unknown:
		ival.str = this->val.str;
		break;
	case ItemType::F32:
		ival.f32 = this->val.f32;
		break;
	case ItemType::F64:
		ival.f64 = this->val.f64;
		break;
	case ItemType::I8:
		ival.i8 = this->val.i8;
		break;
	case ItemType::U8:
		ival.u8 = this->val.u8;
		break;
	case ItemType::I16:
		ival.i16 = this->val.i16;
		break;
	case ItemType::U16:
		ival.u16 = this->val.u16;
		break;
	case ItemType::I32:
		ival.i32 = this->val.i32;
		break;
	case ItemType::U32:
		ival.u32 = this->val.u32;
		break;
	case ItemType::I64:
		ival.i64 = this->val.i64;
		break;
	case ItemType::U64:
		ival.u64 = this->val.u64;
		break;
	case ItemType::BOOL:
		ival.boolean = this->val.boolean;
		break;
	case ItemType::Str:
		ival.str = SCOPY_TEXT(this->val.str);
		break;
	case ItemType::Date:
		NEW_CLASS(ival.date, Data::DateTime(this->val.date));
		break;
	case ItemType::ByteArr:
		ival.byteArr = this->val.byteArr->Clone();
		break;
	case ItemType::Vector:
		ival.vector = this->val.vector->Clone();
		break;
	case ItemType::UUID:
		ival.uuid = this->val.uuid->Clone();
		break;
	}
	return NEW_CLASS_D(VariItem(this->itemType, ival));
}

void Data::VariItem::ToString(Text::StringBuilderUTF *sb)
{
	const UTF8Char *csptr;
	UTF8Char sbuff[64];
	switch (this->itemType)
	{
	case ItemType::Unknown:
	case ItemType::Null:
		sb->Append((const UTF8Char*)"null");
		return;
	case ItemType::F32:
		Text::SBAppendF32(sb, this->val.f32);
		return;
	case ItemType::F64:
		Text::SBAppendF64(sb, this->val.f64);
		return;
	case ItemType::I8:
		sb->AppendI16(this->val.i8);
		return;
	case ItemType::U8:
		sb->AppendU16(this->val.u8);
		return;
	case ItemType::I16:
		sb->AppendI16(this->val.i16);
		return;
	case ItemType::U16:
		sb->AppendU16(this->val.u16);
		return;
	case ItemType::I32:
		sb->AppendI32(this->val.i32);
		return;
	case ItemType::U32:
		sb->AppendU32(this->val.u32);
		return;
	case ItemType::I64:
		sb->AppendI64(this->val.i64);
		return;
	case ItemType::U64:
		sb->AppendU64(this->val.u64);
		return;
	case ItemType::BOOL:
		sb->Append(this->val.boolean?(const UTF8Char*)"true":(const UTF8Char*)"false");
		break;
	case ItemType::Str:
		csptr = Text::JSText::ToNewJSTextDQuote(this->val.str);
		sb->Append(csptr);
		Text::JSText::FreeNewText(csptr);
		return;
	case ItemType::Date:
		this->val.date->ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb->AppendChar('\"', 1);
		sb->Append(sbuff);
		sb->AppendChar('\"', 1);
		break;
	case ItemType::ByteArr:
		sb->AppendChar('\"', 1);
		sb->AppendHexBuff(this->val.byteArr->GetArray(), this->val.byteArr->GetCount(), 0, Text::LineBreakType::None);
		sb->AppendChar('\"', 1);
		return;
	case ItemType::Vector:
		{
			Math::WKTWriter writer;
			writer.GenerateWKT(sb, this->val.vector);
		}
		return;
	case ItemType::UUID:
		{
			sb->AppendChar('{', 1);
			this->val.uuid->ToString(sb);
			sb->AppendChar('}', 1);
			return;
		}
	}
}

Data::VariItem *Data::VariItem::NewNull()
{
	ItemValue ival;
	ival.str = 0;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Null, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewStr(const UTF8Char *str)
{
	ItemValue ival;
	ival.str = Text::StrCopyNew(str);
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Str, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewDate(Data::DateTime *dt)
{
	ItemValue ival;
	NEW_CLASS(ival.date, Data::DateTime(dt));
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Date, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewF32(Single val)
{
	ItemValue ival;
	ival.f32 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::F32, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewF64(Double val)
{
	ItemValue ival;
	ival.f64 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::F64, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewI8(Int8 val)
{
	ItemValue ival;
	ival.i8 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::I8, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewU8(UInt8 val)
{
	ItemValue ival;
	ival.u8 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::U8, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewI16(Int16 val)
{
	ItemValue ival;
	ival.i16 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::I16, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewU16(UInt16 val)
{
	ItemValue ival;
	ival.u16 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::U16, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewI32(Int32 val)
{
	ItemValue ival;
	ival.i32 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::I32, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewU32(UInt32 val)
{
	ItemValue ival;
	ival.u32 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::U32, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewI64(Int64 val)
{
	ItemValue ival;
	ival.i64 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::I64, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewU64(UInt64 val)
{
	ItemValue ival;
	ival.u64 = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::U64, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewBool(Bool val)
{
	ItemValue ival;
	ival.boolean = val;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::BOOL, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewByteArr(const UInt8 *arr, UOSInt cnt)
{
	ItemValue ival;
	NEW_CLASS(ival.byteArr, Data::ReadonlyArray<UInt8>(arr, cnt));
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::ByteArr, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewVector(Math::Vector2D *vec)
{
	ItemValue ival;
	ival.vector = vec->Clone();
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Vector, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewUUID(Data::UUID *uuid)
{
	ItemValue ival;
	ival.uuid = uuid->Clone();
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::UUID, ival));
	return item;
}
