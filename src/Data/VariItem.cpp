#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/VariItem.h"
#include "Math/WKTWriter.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

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

Single Data::VariItem::GetAsF32()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return this->val.f32;
	case ItemType::F64:
		return (Single)this->val.f64;
	case ItemType::I8:
		return (Single)this->val.i8;
	case ItemType::U8:
		return (Single)this->val.u8;
	case ItemType::I16:
		return (Single)this->val.i16;
	case ItemType::U16:
		return (Single)this->val.u16;
	case ItemType::I32:
		return (Single)this->val.i32;
	case ItemType::U32:
		return (Single)this->val.u32;
	case ItemType::I64:
		return (Single)this->val.i64;
	case ItemType::U64:
		return (Single)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Double Data::VariItem::GetAsF64()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return (Double)this->val.f32;
	case ItemType::F64:
		return (Double)this->val.f64;
	case ItemType::I8:
		return (Double)this->val.i8;
	case ItemType::U8:
		return (Double)this->val.u8;
	case ItemType::I16:
		return (Double)this->val.i16;
	case ItemType::U16:
		return (Double)this->val.u16;
	case ItemType::I32:
		return (Double)this->val.i32;
	case ItemType::U32:
		return (Double)this->val.u32;
	case ItemType::I64:
		return (Double)this->val.i64;
	case ItemType::U64:
		return (Double)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Int8 Data::VariItem::GetAsI8()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return (Int8)this->val.f32;
	case ItemType::F64:
		return (Int8)this->val.f64;
	case ItemType::I8:
		return (Int8)this->val.i8;
	case ItemType::U8:
		return (Int8)this->val.u8;
	case ItemType::I16:
		return (Int8)this->val.i16;
	case ItemType::U16:
		return (Int8)this->val.u16;
	case ItemType::I32:
		return (Int8)this->val.i32;
	case ItemType::U32:
		return (Int8)this->val.u32;
	case ItemType::I64:
		return (Int8)this->val.i64;
	case ItemType::U64:
		return (Int8)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

UInt8 Data::VariItem::GetAsU8()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return (UInt8)this->val.f32;
	case ItemType::F64:
		return (UInt8)this->val.f64;
	case ItemType::I8:
		return (UInt8)this->val.i8;
	case ItemType::U8:
		return (UInt8)this->val.u8;
	case ItemType::I16:
		return (UInt8)this->val.i16;
	case ItemType::U16:
		return (UInt8)this->val.u16;
	case ItemType::I32:
		return (UInt8)this->val.i32;
	case ItemType::U32:
		return (UInt8)this->val.u32;
	case ItemType::I64:
		return (UInt8)this->val.i64;
	case ItemType::U64:
		return (UInt8)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Int16 Data::VariItem::GetAsI16()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return (Int16)this->val.f32;
	case ItemType::F64:
		return (Int16)this->val.f64;
	case ItemType::I8:
		return (Int16)this->val.i8;
	case ItemType::U8:
		return (Int16)this->val.u8;
	case ItemType::I16:
		return (Int16)this->val.i16;
	case ItemType::U16:
		return (Int16)this->val.u16;
	case ItemType::I32:
		return (Int16)this->val.i32;
	case ItemType::U32:
		return (Int16)this->val.u32;
	case ItemType::I64:
		return (Int16)this->val.i64;
	case ItemType::U64:
		return (Int16)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

UInt16 Data::VariItem::GetAsU16()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return (UInt16)this->val.f32;
	case ItemType::F64:
		return (UInt16)this->val.f64;
	case ItemType::I8:
		return (UInt16)this->val.i8;
	case ItemType::U8:
		return (UInt16)this->val.u8;
	case ItemType::I16:
		return (UInt16)this->val.i16;
	case ItemType::U16:
		return (UInt16)this->val.u16;
	case ItemType::I32:
		return (UInt16)this->val.i32;
	case ItemType::U32:
		return (UInt16)this->val.u32;
	case ItemType::I64:
		return (UInt16)this->val.i64;
	case ItemType::U64:
		return (UInt16)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Int32 Data::VariItem::GetAsI32()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return (Int32)this->val.f32;
	case ItemType::F64:
		return (Int32)this->val.f64;
	case ItemType::I8:
		return (Int32)this->val.i8;
	case ItemType::U8:
		return (Int32)this->val.u8;
	case ItemType::I16:
		return (Int32)this->val.i16;
	case ItemType::U16:
		return (Int32)this->val.u16;
	case ItemType::I32:
		return (Int32)this->val.i32;
	case ItemType::U32:
		return (Int32)this->val.u32;
	case ItemType::I64:
		return (Int32)this->val.i64;
	case ItemType::U64:
		return (Int32)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

UInt32 Data::VariItem::GetAsU32()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return (UInt32)this->val.f32;
	case ItemType::F64:
		return (UInt32)this->val.f64;
	case ItemType::I8:
		return (UInt32)this->val.i8;
	case ItemType::U8:
		return (UInt32)this->val.u8;
	case ItemType::I16:
		return (UInt32)this->val.i16;
	case ItemType::U16:
		return (UInt32)this->val.u16;
	case ItemType::I32:
		return (UInt32)this->val.i32;
	case ItemType::U32:
		return (UInt32)this->val.u32;
	case ItemType::I64:
		return (UInt32)this->val.i64;
	case ItemType::U64:
		return (UInt32)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Int64 Data::VariItem::GetAsI64()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return (Int64)this->val.f32;
	case ItemType::F64:
		return (Int64)this->val.f64;
	case ItemType::I8:
		return (Int64)this->val.i8;
	case ItemType::U8:
		return (Int64)this->val.u8;
	case ItemType::I16:
		return (Int64)this->val.i16;
	case ItemType::U16:
		return (Int64)this->val.u16;
	case ItemType::I32:
		return (Int64)this->val.i32;
	case ItemType::U32:
		return (Int64)this->val.u32;
	case ItemType::I64:
		return (Int64)this->val.i64;
	case ItemType::U64:
		return (Int64)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

UInt64 Data::VariItem::GetAsU64()
{
	switch (this->itemType)
	{
	case ItemType::F32:
		return (UInt64)this->val.f32;
	case ItemType::F64:
		return (UInt64)this->val.f64;
	case ItemType::I8:
		return (UInt64)this->val.i8;
	case ItemType::U8:
		return (UInt64)this->val.u8;
	case ItemType::I16:
		return (UInt64)this->val.i16;
	case ItemType::U16:
		return (UInt64)this->val.u16;
	case ItemType::I32:
		return (UInt64)this->val.i32;
	case ItemType::U32:
		return (UInt64)this->val.u32;
	case ItemType::I64:
		return (UInt64)this->val.i64;
	case ItemType::U64:
		return (UInt64)this->val.u64;
	case ItemType::BOOL:
		return this->val.boolean?1:0;
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Bool Data::VariItem::GetAsBool()
{
	return this->GetAsI64() != 0;
}

void Data::VariItem::GetAsString(Text::StringBuilderUTF *sb)
{
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
		sb->Append(this->val.str);
		return;
	case ItemType::Date:
		this->val.date->ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		sb->Append(sbuff);
		break;
	case ItemType::ByteArr:
		sb->AppendHexBuff(this->val.byteArr->GetArray(), this->val.byteArr->GetCount(), 0, Text::LineBreakType::None);
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

Data::DateTime *Data::VariItem::GetAsDate()
{
	if (this->itemType != ItemType::Date)
		return 0;
	return this->val.date;
}

Data::ReadonlyArray<UInt8> *Data::VariItem::GetAsByteArr()
{
	if (this->itemType != ItemType::ByteArr)
		return 0;
	return this->val.byteArr;
}

Math::Vector2D *Data::VariItem::GetAsVector()
{
	if (this->itemType != ItemType::Vector)
		return 0;
	return this->val.vector;
}

Data::UUID *Data::VariItem::GetAsUUID()
{
	if (this->itemType != ItemType::UUID)
		return 0;
	return this->val.uuid;
}

void *Data::VariItem::GetAsUnk()
{
	return (void*)this->val.str;
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
	if (str == 0) return NewNull();
	ItemValue ival;
	ival.str = Text::StrCopyNew(str);
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Str, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewDate(Data::DateTime *dt)
{
	if (dt == 0) return NewNull();
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
	if (arr == 0) return NewNull();
	ItemValue ival;
	NEW_CLASS(ival.byteArr, Data::ReadonlyArray<UInt8>(arr, cnt));
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::ByteArr, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewByteArr(Data::ReadonlyArray<UInt8> *arr)
{
	if (arr == 0) return NewNull();
	ItemValue ival;
	ival.byteArr = arr->Clone();
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::ByteArr, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewVector(Math::Vector2D *vec)
{
	if (vec == 0) return NewNull();
	ItemValue ival;
	ival.vector = vec->Clone();
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Vector, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewUUID(Data::UUID *uuid)
{
	if (uuid == 0) return NewNull();
	ItemValue ival;
	ival.uuid = uuid->Clone();
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::UUID, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewFromPtr(void *ptr, ItemType itemType)
{
	switch (itemType)
	{
	case ItemType::F32:
		return NewF32(*(Single*)ptr);
	case ItemType::F64:
		return NewF64(*(Double*)ptr);
	case ItemType::I8:
		return NewI8(*(Int8*)ptr);
	case ItemType::U8:
		return NewU8(*(UInt8*)ptr);
	case ItemType::I16:
		return NewI16(*(Int16*)ptr);
	case ItemType::U16:
		return NewU16(*(UInt16*)ptr);
	case ItemType::I32:
		return NewI32(*(Int32*)ptr);
	case ItemType::U32:
		return NewU32(*(UInt32*)ptr);
	case ItemType::I64:
		return NewI64(*(Int64*)ptr);
	case ItemType::U64:
		return NewU64(*(UInt64*)ptr);
	case ItemType::BOOL:
		return NewBool(*(Bool*)ptr);
	case ItemType::Null:
		return NewNull();
	case ItemType::Str:
		return NewStr(*(const UTF8Char**)ptr);
	case ItemType::Date:
		return NewDate(*(Data::DateTime**)ptr);
	case ItemType::ByteArr:
		return NewByteArr(*(Data::ReadonlyArray<UInt8>**)ptr);
	case ItemType::Vector:
		return NewVector(*(Math::Vector2D**)ptr);
	case ItemType::UUID:
		return NewUUID(*(Data::UUID**)ptr);
	case ItemType::Unknown:
	default:
		return 0;
	}
}

void Data::VariItem::SetPtr(void *ptr, ItemType itemType, VariItem *item)
{
	switch (itemType)
	{
	case ItemType::F32:
		*(Single*)ptr = item->GetAsF32();
		break;
	case ItemType::F64:
		*(Double*)ptr = item->GetAsF64();
		break;
	case ItemType::I8:
		*(Int8*)ptr = item->GetAsI8();
		break;
	case ItemType::U8:
		*(UInt8*)ptr = item->GetAsU8();
		break;
	case ItemType::I16:
		*(Int16*)ptr = item->GetAsI16();
		break;
	case ItemType::U16:
		*(UInt16*)ptr = item->GetAsU16();
		break;
	case ItemType::I32:
		*(Int32*)ptr = item->GetAsI32();
		break;
	case ItemType::U32:
		*(UInt32*)ptr = item->GetAsU32();
		break;
	case ItemType::I64:
		*(Int64*)ptr = item->GetAsI64();
		break;
	case ItemType::U64:
		*(UInt64*)ptr = item->GetAsU64();
		break;
	case ItemType::BOOL:
		*(Bool*)ptr = item->GetAsBool();
		break;
	case ItemType::Null:
		*(void**)ptr = 0;
	case ItemType::Str:
		{
			Text::StringBuilderUTF8 sb;
			item->GetAsString(&sb);
			*(const UTF8Char**)ptr = Text::StrCopyNew(sb.ToString());
		}
		break;
	case ItemType::Date:
		{
			Data::DateTime *date = item->GetAsDate();
			if (date)
			{
				*(Data::DateTime**)ptr = NEW_CLASS_D(Data::DateTime(date));
			}
			else
			{
				*(Data::DateTime**)ptr = 0;
			}
			break;
		}
	case ItemType::ByteArr:
		{
			Data::ReadonlyArray<UInt8> *byteArr = item->GetAsByteArr();
			if (byteArr)
			{
				*(Data::ReadonlyArray<UInt8>**)ptr = byteArr->Clone();
			}
			else
			{
				*(Data::ReadonlyArray<UInt8>**)ptr = 0;
			}
			break;
		}
	case ItemType::Vector:
		{
			Math::Vector2D *vec = item->GetAsVector();
			if (vec)
			{
				*(Math::Vector2D**)ptr = vec->Clone();
			}
			else
			{
				*(Math::Vector2D**)ptr = 0;
			}
			break;
		}
	case ItemType::UUID:
		{
			Data::UUID *uuid = item->GetAsUUID();
			if (uuid)
			{
				*(Data::UUID**)ptr = uuid->Clone();
			}
			else
			{
				*(Data::UUID**)ptr = 0;
			}
			break;
		}
	case ItemType::Unknown:
	default:
		*(void**)ptr = item->GetAsUnk();
		break;
	}
}

UOSInt Data::VariItem::GetItemSize(ItemType itemType)
{
	switch (itemType)
	{
	case ItemType::F32:
		return sizeof(Single);
	case ItemType::F64:
		return sizeof(Double);
	case ItemType::I8:
	case ItemType::U8:
		return sizeof(Int8);
	case ItemType::I16:
	case ItemType::U16:
		return sizeof(Int16);
	case ItemType::I32:
	case ItemType::U32:
		return sizeof(Int32);
	case ItemType::I64:
	case ItemType::U64:
		return sizeof(Int64);
	case ItemType::BOOL:
		return sizeof(Bool);
	case ItemType::Unknown:
	case ItemType::Null:
	case ItemType::Str:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	default:
		return sizeof(void*);
	}
}
