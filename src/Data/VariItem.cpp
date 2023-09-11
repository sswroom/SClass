#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/VariItem.h"
#include "Math/WKTReader.h"
#include "Math/WKTWriter.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"

Single Data::VariItem::GetAsF32() const
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
		return (Single)(this->val.boolean?1:0);
	case ItemType::Str:
		return (Single)Text::StrToDouble(this->val.str->v);
	case ItemType::CStr:
		return (Single)Text::StrToDouble(this->val.cstr.v);
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Double Data::VariItem::GetAsF64() const
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
		return Text::StrToDouble(this->val.str->v);
	case ItemType::CStr:
		return Text::CString(this->val.cstr.v, this->val.cstr.leng).ToDouble();
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Int8 Data::VariItem::GetAsI8() const
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
		return (Int8)Text::StrToInt32(this->val.str->v);
	case ItemType::CStr:
		return (Int8)Text::StrToInt32(this->val.cstr.v);
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

UInt8 Data::VariItem::GetAsU8() const
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
		return (UInt8)Text::StrToUInt32(this->val.str->v);
	case ItemType::CStr:
		return (UInt8)Text::StrToUInt32(this->val.cstr.v);
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Int16 Data::VariItem::GetAsI16() const
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
		return (Int16)Text::StrToInt32(this->val.str->v);
	case ItemType::CStr:
		return (Int16)Text::StrToInt32(this->val.cstr.v);
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

UInt16 Data::VariItem::GetAsU16() const
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
		return (UInt16)Text::StrToUInt32(this->val.str->v);
	case ItemType::CStr:
		return (UInt16)Text::StrToUInt32(this->val.cstr.v);
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Int32 Data::VariItem::GetAsI32() const
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
		return Text::StrToInt32(this->val.str->v);
	case ItemType::CStr:
		return Text::StrToInt32(this->val.cstr.v);
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

UInt32 Data::VariItem::GetAsU32() const
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
		return Text::StrToUInt32(this->val.str->v);
	case ItemType::CStr:
		return Text::StrToUInt32(this->val.cstr.v);
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Int64 Data::VariItem::GetAsI64() const
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
		return Text::StrToInt64(this->val.str->v);
	case ItemType::CStr:
		return Text::StrToInt64(this->val.cstr.v);
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

UInt64 Data::VariItem::GetAsU64() const
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
		return Text::StrToUInt64(this->val.str->v);
	case ItemType::CStr:
		return Text::StrToUInt64(this->val.cstr.v);
	case ItemType::Timestamp:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return 0;
	}
}

Bool Data::VariItem::GetAsBool() const
{
	return this->GetAsI64() != 0;
}

void Data::VariItem::GetAsString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	switch (this->itemType)
	{
	case ItemType::Unknown:
	case ItemType::Null:
		sb->AppendC(UTF8STRC("null"));
		return;
	case ItemType::F32:
		sb->AppendDouble(this->val.f32);
		return;
	case ItemType::F64:
		sb->AppendDouble(this->val.f64);
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
		if (this->val.boolean)
		{
			sb->AppendC(UTF8STRC("true"));
		}
		else
		{
			sb->AppendC(UTF8STRC("false"));
		}
		break;
	case ItemType::Str:
		sb->AppendC(this->val.str->v, this->val.str->leng);
		return;
	case ItemType::CStr:
		sb->AppendC(this->val.cstr.v, this->val.cstr.leng);
		return;
	case ItemType::Timestamp:
		sptr = this->val.ts.ToStringNoZone(sbuff);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		break;
	case ItemType::ByteArr:
		sb->AppendHexBuff(this->val.byteArr->GetArray(), this->val.byteArr->GetCount(), 0, Text::LineBreakType::None);
		return;
	case ItemType::Vector:
		{
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (vec.Set(this->val.vector))
			{
				Math::WKTWriter writer;
				writer.ToText(sb, vec);
			}
			else
			{
				sb->AppendC(UTF8STRC("null"));
			}
		}
		return;
	case ItemType::UUID:
		{
			sb->AppendUTF8Char('{');
			this->val.uuid->ToString(sb);
			sb->AppendUTF8Char('}');
			return;
		}
	}
}

UTF8Char *Data::VariItem::GetAsStringS(UTF8Char *sbuff, UOSInt buffSize) const
{
	switch (this->itemType)
	{
	case ItemType::Unknown:
	case ItemType::Null:
		return Text::StrConcatC(sbuff, UTF8STRC("null"));
	case ItemType::F32:
		return Text::StrDouble(sbuff, this->val.f32);
	case ItemType::F64:
		return Text::StrDouble(sbuff, this->val.f64);
	case ItemType::I8:
		return Text::StrInt16(sbuff, this->val.i8);
	case ItemType::U8:
		return Text::StrUInt16(sbuff, this->val.u8);
	case ItemType::I16:
		return Text::StrInt16(sbuff, this->val.i16);
	case ItemType::U16:
		return Text::StrUInt16(sbuff, this->val.u16);
	case ItemType::I32:
		return Text::StrInt32(sbuff, this->val.i32);
	case ItemType::U32:
		return Text::StrUInt32(sbuff, this->val.u32);
	case ItemType::I64:
		return Text::StrInt64(sbuff, this->val.i64);
	case ItemType::U64:
		return Text::StrUInt64(sbuff, this->val.u64);
	case ItemType::BOOL:
		if (this->val.boolean)
		{
			return Text::StrConcatC(sbuff, UTF8STRC("true"));
		}
		else
		{
			return Text::StrConcatC(sbuff, UTF8STRC("false"));
		}
		break;
	case ItemType::Str:
		return this->val.str->ConcatToS(sbuff, buffSize);
	case ItemType::CStr:
		return Text::StrConcatCS(sbuff, this->val.cstr.v, this->val.cstr.leng, buffSize);
	case ItemType::Timestamp:
		if (buffSize < 11)
		{
			return sbuff;
		}
		else if (buffSize < 17)
		{
			return this->val.ts.ToString(sbuff, "yyyy-MM-dd");
		}
		else if (buffSize < 20)
		{
			return this->val.ts.ToString(sbuff, "yyyy-MM-dd HH:mm");
		}
		else if (buffSize < 24)
		{
			return this->val.ts.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		}
		else if (buffSize < 27)
		{
			return this->val.ts.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		}
		else if (buffSize < 30)
		{
			return this->val.ts.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.ffffff");
		}
		else
		{
			return this->val.ts.ToStringNoZone(sbuff);
		}
	case ItemType::ByteArr:
		if (buffSize > (this->val.byteArr->GetCount() << 1))
		{
			return Text::StrHexBytes(sbuff, this->val.byteArr->GetArray(), this->val.byteArr->GetCount(), 0);
		}
		else
		{
			return Text::StrHexBytes(sbuff, this->val.byteArr->GetArray(), (buffSize - 1) >> 1, 0);
		}
	case ItemType::Vector:
		{
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (vec.Set(this->val.vector))
			{
				Text::StringBuilderUTF8 sb;
				Math::WKTWriter writer;
				writer.ToText(sb, vec);
				return sb.ConcatToS(sbuff, buffSize);
			}
			else
			{
				return Text::StrConcatCS(sbuff, UTF8STRC("null"), buffSize);
			}
		}
	case ItemType::UUID:
		{
			return Text::StrConcatC(this->val.uuid->ToString(Text::StrConcatC(sbuff, UTF8STRC("{"))), UTF8STRC("}"));
		}
	default:
		*sbuff = 0;
		return sbuff;
	}
}

Text::String *Data::VariItem::GetAsNewString() const
{
	NotNullPtr<Text::String> s;
	switch (this->itemType)
	{
	case ItemType::Unknown:
	case ItemType::Null:
		return 0;
	case ItemType::F32:
		s = Text::String::New(32);
		s->leng = (UOSInt)(Text::StrDouble(s->v, this->val.f32) - s->v);
		return s.Ptr();
	case ItemType::F64:
		s = Text::String::New(32);
		s->leng = (UOSInt)(Text::StrDouble(s->v, this->val.f64) - s->v);
		return s.Ptr();
	case ItemType::I8:
		s = Text::String::New(4);
		s->leng = (UOSInt)(Text::StrInt16(s->v, this->val.i8) - s->v);
		return s.Ptr();
	case ItemType::U8:
		s = Text::String::New(3);
		s->leng = (UOSInt)(Text::StrUInt16(s->v, this->val.u8) - s->v);
		return s.Ptr();
	case ItemType::I16:
		s = Text::String::New(6);
		s->leng = (UOSInt)(Text::StrInt16(s->v, this->val.i16) - s->v);
		return s.Ptr();
	case ItemType::U16:
		s = Text::String::New(5);
		s->leng = (UOSInt)(Text::StrUInt16(s->v, this->val.u16) - s->v);
		return s.Ptr();
	case ItemType::I32:
		s = Text::String::New(11);
		s->leng = (UOSInt)(Text::StrInt32(s->v, this->val.i32) - s->v);
		return s.Ptr();
	case ItemType::U32:
		s = Text::String::New(10);
		s->leng = (UOSInt)(Text::StrUInt32(s->v, this->val.u32) - s->v);
		return s.Ptr();
	case ItemType::I64:
		s = Text::String::New(21);
		s->leng = (UOSInt)(Text::StrInt64(s->v, this->val.i64) - s->v);
		return s.Ptr();
	case ItemType::U64:
		s = Text::String::New(20);
		s->leng = (UOSInt)(Text::StrUInt64(s->v, this->val.u64) - s->v);
		return s.Ptr();
	case ItemType::BOOL:
		if (this->val.boolean)
		{
			return Text::String::New(UTF8STRC("true")).Ptr();
		}
		else
		{
			return Text::String::New(UTF8STRC("false")).Ptr();
		}
		break;
	case ItemType::Str:
		return this->val.str->Clone().Ptr();
	case ItemType::CStr:
		return Text::String::New(this->val.cstr.v, this->val.cstr.leng).Ptr();
	case ItemType::Timestamp:
		s = Text::String::New(30);
		s->leng = (UOSInt)(this->val.ts.ToStringNoZone(s->v) - s->v);
		return s.Ptr();
	case ItemType::ByteArr:
		s = Text::String::New(this->val.byteArr->GetCount() * 2);
		s->leng = (UOSInt)(Text::StrHexBytes(s->v, this->val.byteArr->GetArray(), this->val.byteArr->GetCount(), 0) - s->v);
		return s.Ptr();
	case ItemType::Vector:
		{
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (vec.Set(this->val.vector))
			{
				Text::StringBuilderUTF8 sb;
				Math::WKTWriter writer;
				writer.ToText(sb, vec);
				return Text::String::New(sb.ToCString()).Ptr();
			}
			else
			{
				return 0;
			}
		}
	case ItemType::UUID:
		s = Text::String::New(48);
		s->leng = (UOSInt)(Text::StrConcatC(this->val.uuid->ToString(Text::StrConcatC(s->v, UTF8STRC("{"))), UTF8STRC("}")) - s->v);
		return s.Ptr();
	default:
		return 0;
	}
}

Data::DateTime *Data::VariItem::GetAsNewDate() const
{
	Data::DateTime *date;
	if (this->itemType == ItemType::Timestamp)
	{
		NEW_CLASS(date, Data::DateTime(this->val.ts.ToTicks(), this->val.ts.tzQhr));
		return date;
	}
	else if (this->itemType == ItemType::CStr)
	{
		NEW_CLASS(date, Data::DateTime());
		date->ToLocalTime();
		if (date->SetValue(Text::CString(this->val.cstr.v, this->val.cstr.leng)))
		{
			return date;
		}
		DEL_CLASS(date);
		return 0;
	}
	else if (this->itemType == ItemType::Str)
	{
		NEW_CLASS(date, Data::DateTime());
		date->ToLocalTime();
		if (date->SetValue(this->val.str->ToCString()))
		{
			return date;
		}
		DEL_CLASS(date);
		return 0;
	}
	else
	{
		return 0;
	}
}

Data::Timestamp Data::VariItem::GetAsTimestamp() const
{
	if (this->itemType == ItemType::Timestamp)
	{
		return this->val.ts;
	}
	else if (this->itemType == ItemType::CStr)
	{
		Data::DateTime dt;
		dt.ToLocalTime();
		if (dt.SetValue(Text::CString(this->val.cstr.v, this->val.cstr.leng)))
		{
			return Data::Timestamp(dt.ToTicks(), dt.GetTimeZoneQHR());
		}
		return Data::Timestamp(0);
	}
	else if (this->itemType == ItemType::Str)
	{
		Data::DateTime dt;
		dt.ToLocalTime();
		if (dt.SetValue(this->val.str->ToCString()))
		{
			return Data::Timestamp(dt.ToTicks(), dt.GetTimeZoneQHR());
		}
		return Data::Timestamp(0);
	}
	else
	{
		return Data::Timestamp(0);
	}
}


Data::ReadonlyArray<UInt8> *Data::VariItem::GetAsNewByteArr() const
{
	if (this->itemType != ItemType::ByteArr)
		return 0;
	return this->val.byteArr->Clone().Ptr();
}

Math::Geometry::Vector2D *Data::VariItem::GetAsNewVector() const
{
	if (this->itemType == ItemType::Vector)
	{
		return this->val.vector->Clone().Ptr();
	}
	else if (this->itemType == ItemType::CStr)
	{
		Math::WKTReader reader(0);
		return reader.ParseWKT(this->val.cstr.v);
	}
	else if (this->itemType == ItemType::Str)
	{
		Math::WKTReader reader(0);
		return reader.ParseWKT(this->val.str->v);
	}
	return 0;
}

Data::UUID *Data::VariItem::GetAsNewUUID() const
{
	if (this->itemType != ItemType::UUID)
		return 0;
	return this->val.uuid->Clone();
}

Data::ReadonlyArray<UInt8> *Data::VariItem::GetAndRemoveByteArr()
{
	if (this->itemType != ItemType::ByteArr)
		return 0;
	this->itemType = ItemType::Null;
	return this->val.byteArr;
}

Math::Geometry::Vector2D *Data::VariItem::GetAndRemoveVector()
{
	if (this->itemType != ItemType::Vector)
		return 0;
	this->itemType = ItemType::Null;
	return this->val.vector;
}

Data::UUID *Data::VariItem::GetAndRemoveUUID()
{
	if (this->itemType != ItemType::UUID)
		return 0;
	this->itemType = ItemType::Null;
	return this->val.uuid;
}

void *Data::VariItem::GetAsUnk() const
{
	return (void*)this->val.str;
}

void Data::VariItem::SetNull()
{
	this->FreeItem();
	this->itemType = ItemType::Null;
}

void Data::VariItem::SetStrSlow(const UTF8Char *str)
{
	this->FreeItem();
	if (str)
	{
		this->val.cstr.v = str;
		this->val.cstr.leng = Text::StrCharCnt(str);
		this->itemType = ItemType::CStr;
	}
	else
	{
		this->val.str = 0;
		this->itemType = ItemType::Null;
	}
}

void Data::VariItem::SetStr(const UTF8Char *str, UOSInt strLen)
{
	this->FreeItem();
	if (str)
	{
		this->val.cstr.v = str;
		this->val.cstr.leng = strLen;
		this->itemType = ItemType::CStr;
	}
	else
	{
		this->val.str = 0;
		this->itemType = ItemType::Null;
	}
}

void Data::VariItem::SetStrCopy(const UTF8Char *str, UOSInt strLen)
{
	this->FreeItem();
	if (str)
	{
		this->val.str = Text::String::New(str, strLen).Ptr();
		this->itemType = ItemType::Str;
	}
	else
	{
		this->val.str = 0;
		this->itemType = ItemType::Null;
	}
}

void Data::VariItem::SetStr(Text::String *str)
{
	this->FreeItem();
	if (str)
	{
		this->val.str = str->Clone().Ptr();
		this->itemType = ItemType::Str;
	}
	else
	{
		this->itemType = ItemType::Null;
	}
}

void Data::VariItem::SetStr(NotNullPtr<Text::String> str)
{
	this->FreeItem();
	this->val.str = str->Clone().Ptr();
	this->itemType = ItemType::Str;
}

void Data::VariItem::SetDate(Data::DateTime *dt)
{
	if (dt == 0)
	{
		this->FreeItem();
		this->itemType = ItemType::Null;
	}
	else if (this->itemType == ItemType::Timestamp)
	{
		this->val.ts = Timestamp(dt->ToTicks(), dt->GetTimeZoneQHR());
	}
	else
	{
		this->FreeItem();
		this->val.ts = Timestamp(dt->ToTicks(), dt->GetTimeZoneQHR());
		this->itemType = ItemType::Timestamp;
	}
}

void Data::VariItem::SetDate(const Data::Timestamp &ts)
{
	if (this->itemType == ItemType::Timestamp)
	{
		this->val.ts = ts;
	}
	else
	{
		this->FreeItem();
		this->val.ts = ts;
		this->itemType = ItemType::Timestamp;
	}
}

void Data::VariItem::SetF32(Single val)
{
	this->FreeItem();
	this->val.f32 = val;
	this->itemType = ItemType::F32;
}

void Data::VariItem::SetF64(Double val)
{
	this->FreeItem();
	this->val.f64 = val;
	this->itemType = ItemType::F64;
}

void Data::VariItem::SetI8(Int8 val)
{
	this->FreeItem();
	this->val.i8 = val;
	this->itemType = ItemType::I8;
}

void Data::VariItem::SetU8(UInt8 val)
{
	this->FreeItem();
	this->val.u8 = val;
	this->itemType = ItemType::U8;
}

void Data::VariItem::SetI16(Int16 val)
{
	this->FreeItem();
	this->val.i16 = val;
	this->itemType = ItemType::I16;
}

void Data::VariItem::SetU16(UInt16 val)
{
	this->FreeItem();
	this->val.u16 = val;
	this->itemType = ItemType::U16;
}

void Data::VariItem::SetI32(Int32 val)
{
	this->FreeItem();
	this->val.i32 = val;
	this->itemType = ItemType::I32;
}

void Data::VariItem::SetU32(UInt32 val)
{
	this->FreeItem();
	this->val.u32 = val;
	this->itemType = ItemType::U32;
}

void Data::VariItem::SetI64(Int64 val)
{
	this->FreeItem();
	this->val.i64 = val;
	this->itemType = ItemType::I64;
}

void Data::VariItem::SetU64(UInt64 val)
{
	this->FreeItem();
	this->val.u64 = val;
	this->itemType = ItemType::U64;
}

void Data::VariItem::SetBool(Bool val)
{
	this->FreeItem();
	this->val.boolean = val;
	this->itemType = ItemType::BOOL;
}

void Data::VariItem::SetByteArr(const UInt8 *arr, UOSInt cnt)
{
	this->FreeItem();
	this->val.byteArr = NEW_CLASS_D(Data::ReadonlyArray<UInt8>(arr, cnt));
	this->itemType = ItemType::ByteArr;
}

void Data::VariItem::SetByteArr(Data::ReadonlyArray<UInt8> *arr)
{
	this->FreeItem();
	this->val.byteArr = arr->Clone().Ptr();
	this->itemType = ItemType::ByteArr;
}

void Data::VariItem::SetVector(Math::Geometry::Vector2D *vec)
{
	this->FreeItem();
	this->val.vector = vec->Clone().Ptr();
	this->itemType = ItemType::Vector;
}

void Data::VariItem::SetUUID(Data::UUID *uuid)
{
	this->FreeItem();
	this->val.uuid = uuid->Clone();
	this->itemType = ItemType::UUID;
}

void Data::VariItem::SetVectorDirect(Math::Geometry::Vector2D *vec)
{
	this->FreeItem();
	this->val.vector = vec;
	this->itemType = ItemType::Vector;
}

void Data::VariItem::SetUUIDDirect(Data::UUID *uuid)
{
	this->FreeItem();
	this->val.uuid = uuid;
	this->itemType = ItemType::UUID;
}

void Data::VariItem::Set(VariItem *item)
{
	this->FreeItem();
	this->itemType = item->itemType;
	switch (this->itemType)
	{
	case ItemType::Null:
		this->val.str = 0;
		break;
	default:
	case ItemType::Unknown:
		this->val.str = item->val.str;
		break;
	case ItemType::F32:
		this->val.f32 = item->val.f32;
		break;
	case ItemType::F64:
		this->val.f64 = item->val.f64;
		break;
	case ItemType::I8:
		this->val.i8 = item->val.i8;
		break;
	case ItemType::U8:
		this->val.u8 = item->val.u8;
		break;
	case ItemType::I16:
		this->val.i16 = item->val.i16;
		break;
	case ItemType::U16:
		this->val.u16 = item->val.u16;
		break;
	case ItemType::I32:
		this->val.i32 = item->val.i32;
		break;
	case ItemType::U32:
		this->val.u32 = item->val.u32;
		break;
	case ItemType::I64:
		this->val.i64 = item->val.i64;
		break;
	case ItemType::U64:
		this->val.u64 = item->val.u64;
		break;
	case ItemType::BOOL:
		this->val.boolean = item->val.boolean;
		break;
	case ItemType::CStr:
		this->val.cstr.v = item->val.cstr.v;
		this->val.cstr.leng = item->val.cstr.leng;
		break;
	case ItemType::Str:
		this->val.str = item->val.str->Clone().Ptr();
		break;
	case ItemType::Timestamp:
		this->val.ts = item->val.ts;
		break;
	case ItemType::ByteArr:
		this->val.byteArr = item->val.byteArr->Clone().Ptr();
		break;
	case ItemType::Vector:
		this->val.vector = item->val.vector->Clone().Ptr();
		break;
	case ItemType::UUID:
		this->val.uuid = item->val.uuid->Clone();
		break;
	}
}

Data::VariItem *Data::VariItem::Clone() const
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
		ival.str = this->val.str->Clone().Ptr();
		break;
	case ItemType::CStr:
		ival.cstr.v = this->val.cstr.v;
		ival.cstr.leng = this->val.cstr.leng;
		break;
	case ItemType::Timestamp:
		ival.ts = this->val.ts;
		break;
	case ItemType::ByteArr:
		ival.byteArr = this->val.byteArr->Clone().Ptr();
		break;
	case ItemType::Vector:
		ival.vector = this->val.vector->Clone().Ptr();
		break;
	case ItemType::UUID:
		ival.uuid = this->val.uuid->Clone();
		break;
	}
	return NEW_CLASS_D(VariItem(this->itemType, ival));
}

void Data::VariItem::ToString(NotNullPtr<Text::StringBuilderUTF8> sb) const
{
	NotNullPtr<Text::String> s;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	switch (this->itemType)
	{
	case ItemType::Unknown:
	case ItemType::Null:
		sb->AppendC(UTF8STRC("null"));
		return;
	case ItemType::F32:
		sb->AppendDouble(this->val.f32);
		return;
	case ItemType::F64:
		sb->AppendDouble(this->val.f64);
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
		if (this->val.boolean)
			sb->AppendC(UTF8STRC("true"));
		else
			sb->AppendC(UTF8STRC("false"));
		break;
	case ItemType::Str:
		s = Text::JSText::ToNewJSTextDQuote(this->val.str->v);
		sb->Append(s);
		s->Release();
		return;
	case ItemType::CStr:
		s = Text::JSText::ToNewJSTextDQuote(this->val.cstr.v);
		sb->Append(s);
		s->Release();
		return;
	case ItemType::Timestamp:
		sptr = this->val.ts.ToStringNoZone(sbuff);
		sb->AppendUTF8Char('\"');
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendUTF8Char('\"');
		break;
	case ItemType::ByteArr:
		sb->AppendUTF8Char('\"');
		sb->AppendHexBuff(this->val.byteArr->GetArray(), this->val.byteArr->GetCount(), 0, Text::LineBreakType::None);
		sb->AppendUTF8Char('\"');
		return;
	case ItemType::Vector:
		{
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (vec.Set(this->val.vector))
			{
				Math::WKTWriter writer;
				writer.ToText(sb, vec);
			}
			else
			{
				sb->AppendC(UTF8STRC("null"));
			}
		}
		return;
	case ItemType::UUID:
		{
			sb->AppendUTF8Char('{');
			this->val.uuid->ToString(sb);
			sb->AppendUTF8Char('}');
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

Data::VariItem *Data::VariItem::NewStrSlow(const UTF8Char *str)
{
	if (str == 0) return NewNull();
	ItemValue ival;
	ival.cstr.v = str;
	ival.cstr.leng = Text::StrCharCnt(str);
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::CStr, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewStr(Text::CString str)
{
	if (str.v == 0) return NewNull();
	ItemValue ival;
	ival.cstr.v = str.v;
	ival.cstr.leng = str.leng;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::CStr, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewStr(Text::String *str)
{
	if (str == 0) return NewNull();
	ItemValue ival;
	ival.str = str->Clone().Ptr();
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Str, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewDate(Data::DateTime *dt)
{
	if (dt == 0) return NewNull();
	ItemValue ival;
	ival.ts = Data::Timestamp(dt->ToTicks(), dt->GetTimeZoneQHR());
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Timestamp, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewTS(const Data::Timestamp &ts)
{
	ItemValue ival;
	ival.ts = ts;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Timestamp, ival));
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
	ival.byteArr = arr->Clone().Ptr();
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::ByteArr, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewVector(Math::Geometry::Vector2D *vec)
{
	if (vec == 0) return NewNull();
	ItemValue ival;
	ival.vector = vec->Clone().Ptr();
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

Data::VariItem *Data::VariItem::NewVectorDirect(Math::Geometry::Vector2D *vec)
{
	if (vec == 0) return NewNull();
	ItemValue ival;
	ival.vector = vec;
	Data::VariItem *item;
	NEW_CLASS(item, Data::VariItem(ItemType::Vector, ival));
	return item;
}

Data::VariItem *Data::VariItem::NewUUIDDirect(Data::UUID *uuid)
{
	if (uuid == 0) return NewNull();
	ItemValue ival;
	ival.uuid = uuid;
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
		return NewStr(*(Text::String**)ptr);
	case ItemType::CStr:
		return NewStr(*(Text::CString*)ptr);
	case ItemType::Timestamp:
		return NewTS(*(Data::Timestamp*)ptr);
	case ItemType::ByteArr:
		return NewByteArr(*(Data::ReadonlyArray<UInt8>**)ptr);
	case ItemType::Vector:
		return NewVector(*(Math::Geometry::Vector2D**)ptr);
	case ItemType::UUID:
		return NewUUID(*(Data::UUID**)ptr);
	case ItemType::Unknown:
	default:
		return 0;
	}
}

void Data::VariItem::SetFromPtr(Data::VariItem *item, void *ptr, ItemType itemType)
{
	switch (itemType)
	{
	case ItemType::F32:
		item->SetF32(*(Single*)ptr);
		return;
	case ItemType::F64:
		item->SetF64(*(Double*)ptr);
		return;
	case ItemType::I8:
		item->SetI8(*(Int8*)ptr);
		return;
	case ItemType::U8:
		item->SetU8(*(UInt8*)ptr);
		return;
	case ItemType::I16:
		item->SetI16(*(Int16*)ptr);
		return;
	case ItemType::U16:
		item->SetU16(*(UInt16*)ptr);
		return;
	case ItemType::I32:
		item->SetI32(*(Int32*)ptr);
		return;
	case ItemType::U32:
		item->SetU32(*(UInt32*)ptr);
		return;
	case ItemType::I64:
		item->SetI64(*(Int64*)ptr);
		return;
	case ItemType::U64:
		item->SetU64(*(UInt64*)ptr);
		return;
	case ItemType::BOOL:
		item->SetBool(*(Bool*)ptr);
		return;
	case ItemType::Null:
		item->SetNull();
		return;
	case ItemType::Str:
		item->SetStr(*(Text::String**)ptr);
		return;
	case ItemType::CStr:
		{
			Text::CString cstr = *(Text::CString*)ptr;
			item->SetStr(cstr.v, cstr.leng);
		}
		return;
	case ItemType::Timestamp:
		item->SetDate(*(Data::Timestamp*)ptr);
		return;
	case ItemType::ByteArr:
		item->SetByteArr(*(Data::ReadonlyArray<UInt8>**)ptr);
		return;
	case ItemType::Vector:
		item->SetVector(*(Math::Geometry::Vector2D**)ptr);
		return;
	case ItemType::UUID:
		item->SetUUID(*(Data::UUID**)ptr);
		return;
	case ItemType::Unknown:
	default:
		return;
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
			ItemType itemType = item->GetItemType();
			if (itemType == ItemType::Null)
			{
				*(Text::String**)ptr = 0;
			}
			else if (itemType == ItemType::Str)
			{
				*(Text::String**)ptr = item->GetItemValue().str->Clone().Ptr();
			}
			else if (itemType == ItemType::CStr)
			{
				const ItemValue ival = item->GetItemValue();
				*(Text::String**)ptr = Text::String::New(ival.cstr.v, ival.cstr.leng).Ptr();
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				item->GetAsString(sb);
				*(Text::String**)ptr = Text::String::New(sb.ToCString()).Ptr();
			}
		}
		break;
	case ItemType::CStr:
		{
			ItemType itemType = item->GetItemType();
			if (itemType == ItemType::Null)
			{
				*(Text::CString*)ptr = 0;
			}
			else if (itemType == ItemType::Str)
			{
				*(Text::CString*)ptr = item->GetItemValue().str->ToCString();
			}
			else if (itemType == ItemType::CStr)
			{
				const ItemValue ival = item->GetItemValue();
				*(Text::CString*)ptr = Text::CString(ival.cstr.v, ival.cstr.leng);
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				item->GetAsString(sb);
				*(Text::CString*)ptr = sb.ToCString();
			}
		}
		break;
	case ItemType::Timestamp:
		*(Data::Timestamp*)ptr = item->GetAsTimestamp();
		break;
	case ItemType::ByteArr:
		*(Data::ReadonlyArray<UInt8>**)ptr = item->GetAsNewByteArr();
		break;
	case ItemType::Vector:
		*(Math::Geometry::Vector2D**)ptr = item->GetAsNewVector();
		break;
	case ItemType::UUID:
		*(Data::UUID**)ptr = item->GetAsNewUUID();
		break;
	case ItemType::Unknown:
	default:
		*(void**)ptr = item->GetAsUnk();
		break;
	}
}


void Data::VariItem::SetPtrAndNotKeep(void *ptr, ItemType itemType, VariItem *item)
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
			ItemType itemType = item->GetItemType();
			if (itemType == ItemType::Null)
			{
				*(Text::String**)ptr = 0;
			}
			else if (itemType == ItemType::Str)
			{
				*(Text::String**)ptr = item->GetItemValue().str->Clone().Ptr();
			}
			else if (itemType == ItemType::CStr)
			{
				const ItemValue ival = item->GetItemValue();
				*(Text::String**)ptr = Text::String::New(ival.cstr.v, ival.cstr.leng).Ptr();
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				item->GetAsString(sb);
				*(Text::String**)ptr = Text::String::New(sb.ToCString()).Ptr();
			}
		}
		break;
	case ItemType::CStr:
		{
			ItemType itemType = item->GetItemType();
			if (itemType == ItemType::Null)
			{
				*(Text::CString*)ptr = 0;
			}
			else if (itemType == ItemType::Str)
			{
				*(Text::CString*)ptr = item->GetItemValue().str->ToCString();
			}
			else if (itemType == ItemType::CStr)
			{
				const ItemValue ival = item->GetItemValue();
				*(Text::CString*)ptr = Text::CString(ival.cstr.v, ival.cstr.leng);
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				item->GetAsString(sb);
				*(Text::CString*)ptr = sb.ToCString();
			}
		}
		break;
	case ItemType::Timestamp:
		*(Data::Timestamp*)ptr = item->GetAsTimestamp();
		break;
	case ItemType::ByteArr:
		if (item->GetItemType() == ItemType::ByteArr)
		{
			*(Data::ReadonlyArray<UInt8>**)ptr = item->GetAndRemoveByteArr();
		}
		else
		{
			*(Data::ReadonlyArray<UInt8>**)ptr = item->GetAsNewByteArr();
		}
		break;
	case ItemType::Vector:
		if (item->GetItemType() == ItemType::Vector)
		{
			*(Math::Geometry::Vector2D**)ptr = item->GetAndRemoveVector();
		}
		else
		{
			*(Math::Geometry::Vector2D**)ptr = item->GetAsNewVector();
		}
		break;
	case ItemType::UUID:
		if (item->GetItemType() == ItemType::UUID)
		{
			*(Data::UUID**)ptr = item->GetAndRemoveUUID();
		}
		else
		{
			*(Data::UUID**)ptr = item->GetAsNewUUID();
		}
		break;
	case ItemType::Unknown:
	default:
		*(void**)ptr = item->GetAsUnk();
		break;
	}
}

Bool Data::VariItem::PtrEquals(void *ptr1, void *ptr2, ItemType itemType)
{
	switch (itemType)
	{
	case ItemType::F32:
		return *(Single*)ptr1 == *(Single*)ptr2;
	case ItemType::F64:
		return *(Double*)ptr1 == *(Double*)ptr2;
	case ItemType::I8:
		return *(Int8*)ptr1 == *(Int8*)ptr2;
	case ItemType::U8:
		return *(UInt8*)ptr1 == *(UInt8*)ptr2;
	case ItemType::I16:
		return *(Int16*)ptr1 == *(Int16*)ptr2;
	case ItemType::U16:
		return *(UInt16*)ptr1 == *(UInt16*)ptr2;
	case ItemType::I32:
		return *(Int32*)ptr1 == *(Int32*)ptr2;
	case ItemType::U32:
		return *(UInt32*)ptr1 == *(UInt32*)ptr2;
	case ItemType::I64:
		return *(Int64*)ptr1 == *(Int64*)ptr2;
	case ItemType::U64:
		return *(UInt64*)ptr1 == *(UInt64*)ptr2;
	case ItemType::BOOL:
		return *(Bool*)ptr1 == *(Bool*)ptr2;
	case ItemType::Null:
		return true;
	case ItemType::Str:
		{
			Text::String *val1 = *(Text::String**)ptr1;
			Text::String *val2 = *(Text::String**)ptr2;
			if (val1 == val2)
			{
				return true;
			}
			if (val1 == 0 || val2 == 0)
			{
				return false;
			}
			return Text::StrEquals(val1->v, val2->v);
		}
		break;
	case ItemType::CStr:
		{
			Text::CString val1 = *(Text::CString*)ptr1;
			Text::CString val2 = *(Text::CString*)ptr2;
			return val1.Equals(val2.v, val2.leng);
		}
		break;
	case ItemType::Timestamp:
		{
			Data::Timestamp val1 = *(Data::Timestamp*)ptr1;
			Data::Timestamp val2 = *(Data::Timestamp*)ptr2;
			return (val1 == val2);
		}
	case ItemType::ByteArr:
		{
			Data::ReadonlyArray<UInt8> *val1 = *(Data::ReadonlyArray<UInt8>**)ptr1;
			Data::ReadonlyArray<UInt8> *val2 = *(Data::ReadonlyArray<UInt8>**)ptr2;
			if (val1 == val2)
			{
				return true;
			}
			if (val1 == 0 || val2 == 0)
			{
				return false;
			}
			return val1->Equals(val2);
		}
	case ItemType::Vector:
		{
			if (ptr1 == ptr2)
			{
				return true;
			}
			NotNullPtr<Math::Geometry::Vector2D> val1;
			NotNullPtr<Math::Geometry::Vector2D> val2;
			if (!val1.Set(*(Math::Geometry::Vector2D**)ptr1) || !val2.Set(*(Math::Geometry::Vector2D**)ptr2))
			{
				return false;
			}
			return val1->Equals(val2);
		}
	case ItemType::UUID:
		{
			Data::UUID *val1 = *(Data::UUID**)ptr1;
			Data::UUID *val2 = *(Data::UUID**)ptr2;
			if (val1 == val2)
			{
				return true;
			}
			if (val1 == 0 || val2 == 0)
			{
				return false;
			}
			return val1->Equals(val2);
		}
	case ItemType::Unknown:
	default:
		return *(void**)ptr1 == *(void**)ptr2;
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
	case ItemType::CStr:
		return sizeof(Text::CString);
	case ItemType::Timestamp:
		return sizeof(Data::Timestamp);
	case ItemType::Unknown:
	case ItemType::Null:
	case ItemType::Str:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	default:
		return sizeof(void*);
	}
}
