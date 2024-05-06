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
	case ItemType::NI32:
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
	case ItemType::NI32:
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
	case ItemType::NI32:
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
	case ItemType::NI32:
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
	case ItemType::NI32:
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
	case ItemType::NI32:
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
	case ItemType::NI32:
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
	case ItemType::NI32:
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

NInt32 Data::VariItem::GetAsNI32() const
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
	case ItemType::NI32:
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
		return this->val.str->ToNInt32();
	case ItemType::CStr:
		{
			Int32 v;
			if (Text::StrToInt32(this->val.cstr.v, v))
				return v;
			else
				return nullptr;
		}
	case ItemType::Timestamp:
	case ItemType::Date:
	case ItemType::ByteArr:
	case ItemType::Vector:
	case ItemType::UUID:
	case ItemType::Unknown:
	case ItemType::Null:
	default:
		return nullptr;
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
	case ItemType::NI32:
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
	case ItemType::NI32:
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

Bool Data::VariItem::GetAsBool() const
{
	return this->GetAsI64() != 0;
}

void Data::VariItem::GetAsString(NN<Text::StringBuilderUTF8> sb) const
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
	case ItemType::NI32:
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
	case ItemType::Date:
		sptr = this->val.date.ToString(sbuff);
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		break;
	case ItemType::ByteArr:
		sb->AppendHexBuff(this->val.byteArr->GetArray(), this->val.byteArr->GetCount(), 0, Text::LineBreakType::None);
		return;
	case ItemType::Vector:
		{
			Math::WKTWriter writer;
			writer.ToText(sb, this->val.vector);
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
	case ItemType::NI32:
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
	case ItemType::Date:
		if (buffSize < 11)
		{
			return sbuff;
		}
		else
		{
			return this->val.date.ToString(sbuff, "yyyy-MM-dd");
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
			Text::StringBuilderUTF8 sb;
			Math::WKTWriter writer;
			writer.ToText(sb, this->val.vector);
			return sb.ConcatToS(sbuff, buffSize);
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
	NN<Text::String> s;
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
	case ItemType::NI32:
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
	case ItemType::Date:
		s = Text::String::New(10);
		s->leng = (UOSInt)(this->val.date.ToString(s->v) - s->v);
		return s.Ptr();
	case ItemType::ByteArr:
		s = Text::String::New(this->val.byteArr->GetCount() * 2);
		s->leng = (UOSInt)(Text::StrHexBytes(s->v, this->val.byteArr->GetArray(), this->val.byteArr->GetCount(), 0) - s->v);
		return s.Ptr();
	case ItemType::Vector:
		{
			Text::StringBuilderUTF8 sb;
			Math::WKTWriter writer;
			writer.ToText(sb, this->val.vector);
			return Text::String::New(sb.ToCString()).Ptr();
		}
	case ItemType::UUID:
		s = Text::String::New(48);
		s->leng = (UOSInt)(Text::StrConcatC(this->val.uuid->ToString(Text::StrConcatC(s->v, UTF8STRC("{"))), UTF8STRC("}")) - s->v);
		return s.Ptr();
	default:
		return 0;
	}
}

Data::DateTime *Data::VariItem::GetAsNewDateTime() const
{
	Data::DateTime *date;
	if (this->itemType == ItemType::Timestamp)
	{
		NEW_CLASS(date, Data::DateTime(this->val.ts.ToTicks(), this->val.ts.tzQhr));
		return date;
	}
	else if (this->itemType == ItemType::Date)
	{
		NEW_CLASS(date, Data::DateTime(this->val.date.ToTicks(), Data::DateTimeUtil::GetLocalTzQhr()));
		return date;
	}
	else if (this->itemType == ItemType::CStr)
	{
		NEW_CLASS(date, Data::DateTime());
		date->ToLocalTime();
		if (date->SetValue(Text::CStringNN(this->val.cstr.v, this->val.cstr.leng)))
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
	else if (this->itemType == ItemType::Date)
	{
		return Data::Timestamp(this->val.date.ToTicks(), 0);
	}
	else if (this->itemType == ItemType::CStr)
	{
		return Data::Timestamp::FromStr(Text::CStringNN(this->val.cstr.v, this->val.cstr.leng), Data::DateTimeUtil::GetLocalTzQhr());
	}
	else if (this->itemType == ItemType::Str)
	{
		return Data::Timestamp::FromStr(this->val.str->ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
	}
	else
	{
		return Data::Timestamp(0);
	}
}

Data::Date Data::VariItem::GetAsDate() const
{
	if (this->itemType == ItemType::Timestamp)
	{
		return this->val.ts.ToDate();
	}
	else if (this->itemType == ItemType::Date)
	{
		return this->val.date;
	}
	else if (this->itemType == ItemType::CStr)
	{
		return Data::Timestamp::FromStr(Text::CStringNN(this->val.cstr.v, this->val.cstr.leng), Data::DateTimeUtil::GetLocalTzQhr()).ToDate();
	}
	else if (this->itemType == ItemType::Str)
	{
		return Data::Timestamp::FromStr(this->val.str->ToCString(), Data::DateTimeUtil::GetLocalTzQhr()).ToDate();
	}
	else
	{
		return Data::Date(nullptr);
	}
}

Data::ReadonlyArray<UInt8> *Data::VariItem::GetAsNewByteArr() const
{
	if (this->itemType != ItemType::ByteArr)
		return 0;
	return this->val.byteArr->Clone().Ptr();
}

Optional<Math::Geometry::Vector2D> Data::VariItem::GetAsNewVector() const
{
	if (this->itemType == ItemType::Vector)
	{
		return this->val.vector->Clone();
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
	return this->val.uuid->Clone().Ptr();
}

Data::ReadonlyArray<UInt8> *Data::VariItem::GetAndRemoveByteArr()
{
	if (this->itemType != ItemType::ByteArr)
		return 0;
	this->itemType = ItemType::Null;
	return this->val.byteArr;
}

Optional<Math::Geometry::Vector2D> Data::VariItem::GetAndRemoveVector()
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
	return this->val.uuid.Ptr();
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

void Data::VariItem::SetStr(Optional<Text::String> str)
{
	this->FreeItem();
	NN<Text::String> s;
	if (str.SetTo(s))
	{
		this->val.str = s->Clone().Ptr();
		this->itemType = ItemType::Str;
	}
	else
	{
		this->itemType = ItemType::Null;
	}
}

void Data::VariItem::SetStr(NN<Text::String> str)
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

void Data::VariItem::SetDate(const Data::Date &date)
{
	this->FreeItem();
	this->val.date = date;
	this->itemType = ItemType::Date;
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

void Data::VariItem::SetNI32(NInt32 val)
{
	this->FreeItem();
	if (val.IsNull())
	{
		this->val.i32 = 0;
		this->itemType = ItemType::Null;
	}
	else
	{
		this->val.i32 = val.IntVal();
		this->itemType = ItemType::I32;
	}
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

void Data::VariItem::SetByteArr(UnsafeArray<const UInt8> arr, UOSInt cnt)
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

void Data::VariItem::SetVector(NN<Math::Geometry::Vector2D> vec)
{
	this->FreeItem();
	this->val.vector = vec->Clone();
	this->itemType = ItemType::Vector;
}

void Data::VariItem::SetUUID(NN<Data::UUID> uuid)
{
	this->FreeItem();
	this->val.uuid = uuid->Clone();
	this->itemType = ItemType::UUID;
}

void Data::VariItem::SetVectorDirect(NN<Math::Geometry::Vector2D> vec)
{
	this->FreeItem();
	this->val.vector = vec;
	this->itemType = ItemType::Vector;
}

void Data::VariItem::SetUUIDDirect(NN<Data::UUID> uuid)
{
	this->FreeItem();
	this->val.uuid = uuid;
	this->itemType = ItemType::UUID;
}

void Data::VariItem::Set(NN<VariItem> item)
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
	case ItemType::NI32:
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
	case ItemType::Date:
		this->val.date = item->val.date;
		break;
	case ItemType::ByteArr:
		this->val.byteArr = item->val.byteArr->Clone().Ptr();
		break;
	case ItemType::Vector:
		this->val.vector = item->val.vector->Clone();
		break;
	case ItemType::UUID:
		this->val.uuid = item->val.uuid->Clone();
		break;
	}
}

NN<Data::VariItem> Data::VariItem::Clone() const
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
	case ItemType::NI32:
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
	case ItemType::Date:
		ival.date = this->val.date;
		break;
	case ItemType::ByteArr:
		ival.byteArr = this->val.byteArr->Clone().Ptr();
		break;
	case ItemType::Vector:
		ival.vector = this->val.vector->Clone();
		break;
	case ItemType::UUID:
		ival.uuid = this->val.uuid->Clone();
		break;
	}
	NN<VariItem> ret;
	NEW_CLASSNN(ret, VariItem(this->itemType, ival));
	return ret;
}

void Data::VariItem::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	NN<Text::String> s;
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
	case ItemType::NI32:
		if (this->val.i32 == NINT32_NULL)
			sb->Append(CSTR("null"));
		else
			sb->AppendI32(this->val.i32);
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
	case ItemType::Date:
		sptr = this->val.date.ToString(sbuff);
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
			Math::WKTWriter writer;
			writer.ToText(sb, this->val.vector);
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

NN<Data::VariItem> Data::VariItem::NewNull()
{
	ItemValue ival;
	ival.str = 0;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::Null, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewStrSlow(const UTF8Char *str)
{
	if (str == 0) return NewNull();
	ItemValue ival;
	ival.cstr.v = str;
	ival.cstr.leng = Text::StrCharCnt(str);
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::CStr, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewStr(Text::CString str)
{
	if (str.v == 0) return NewNull();
	ItemValue ival;
	ival.cstr.v = str.v;
	ival.cstr.leng = str.leng;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::CStr, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewStr(Optional<Text::String> str)
{
	NN<Text::String> s;
	if (!str.SetTo(s)) return NewNull();
	ItemValue ival;
	ival.str = s->Clone().Ptr();
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::Str, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewDateTime(Data::DateTime *dt)
{
	if (dt == 0) return NewNull();
	ItemValue ival;
	ival.ts = Data::Timestamp(dt->ToTicks(), dt->GetTimeZoneQHR());
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::Timestamp, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewTS(const Data::Timestamp &ts)
{
	ItemValue ival;
	ival.ts = ts;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::Timestamp, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewDate(const Data::Date &date)
{
	ItemValue ival;
	ival.date = date;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::Date, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewF32(Single val)
{
	ItemValue ival;
	ival.f32 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::F32, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewF64(Double val)
{
	ItemValue ival;
	ival.f64 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::F64, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewI8(Int8 val)
{
	ItemValue ival;
	ival.i8 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::I8, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewU8(UInt8 val)
{
	ItemValue ival;
	ival.u8 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::U8, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewI16(Int16 val)
{
	ItemValue ival;
	ival.i16 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::I16, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewU16(UInt16 val)
{
	ItemValue ival;
	ival.u16 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::U16, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewI32(Int32 val)
{
	ItemValue ival;
	ival.i32 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::I32, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewU32(UInt32 val)
{
	ItemValue ival;
	ival.u32 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::U32, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewNI32(NInt32 val)
{
	ItemValue ival;
	NN<Data::VariItem> item;
	if (val.IsNull())
	{
		ival.i32 = 0;		
		NEW_CLASSNN(item, Data::VariItem(ItemType::Null, ival));
	}
	else
	{
		ival.i32 = val.IntVal();
		NEW_CLASSNN(item, Data::VariItem(ItemType::I32, ival));
	}
	return item;
}

NN<Data::VariItem> Data::VariItem::NewI64(Int64 val)
{
	ItemValue ival;
	ival.i64 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::I64, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewU64(UInt64 val)
{
	ItemValue ival;
	ival.u64 = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::U64, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewBool(Bool val)
{
	ItemValue ival;
	ival.boolean = val;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::BOOL, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewByteArr(const UInt8 *arr, UOSInt cnt)
{
	if (arr == 0) return NewNull();
	ItemValue ival;
	NEW_CLASS(ival.byteArr, Data::ReadonlyArray<UInt8>(arr, cnt));
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::ByteArr, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewByteArr(Data::ReadonlyArray<UInt8> *arr)
{
	if (arr == 0) return NewNull();
	ItemValue ival;
	ival.byteArr = arr->Clone().Ptr();
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::ByteArr, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewVector(Optional<Math::Geometry::Vector2D> vec)
{
	NN<Math::Geometry::Vector2D> nnvec;
	if (!vec.SetTo(nnvec)) return NewNull();
	ItemValue ival;
	ival.vector = nnvec->Clone();
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::Vector, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewUUID(Data::UUID *uuid)
{
	if (uuid == 0) return NewNull();
	ItemValue ival;
	ival.uuid = uuid->Clone();
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::UUID, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewVectorDirect(Optional<Math::Geometry::Vector2D> vec)
{
	NN<Math::Geometry::Vector2D> nnvec;
	if (!vec.SetTo(nnvec)) return NewNull();
	ItemValue ival;
	ival.vector = nnvec;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::Vector, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewUUIDDirect(Data::UUID *uuid)
{
	NN<Data::UUID> nnuuid;
	if (!nnuuid.Set(uuid)) return NewNull();
	ItemValue ival;
	ival.uuid = nnuuid;
	NN<Data::VariItem> item;
	NEW_CLASSNN(item, Data::VariItem(ItemType::UUID, ival));
	return item;
}

NN<Data::VariItem> Data::VariItem::NewFromPtr(void *ptr, ItemType itemType)
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
	case ItemType::NI32:
		return NewNI32(*(NInt32*)ptr);
	case ItemType::I64:
		return NewI64(*(Int64*)ptr);
	case ItemType::U64:
		return NewU64(*(UInt64*)ptr);
	case ItemType::BOOL:
		return NewBool(*(Bool*)ptr);
	case ItemType::Null:
		return NewNull();
	case ItemType::Str:
		return NewStr(*(Optional<Text::String>*)ptr);
	case ItemType::CStr:
		return NewStr(*(Text::CString*)ptr);
	case ItemType::Timestamp:
		return NewTS(*(Data::Timestamp*)ptr);
	case ItemType::Date:
		return NewDate(*(Data::Date*)ptr);
	case ItemType::ByteArr:
		return NewByteArr(*(Data::ReadonlyArray<UInt8>**)ptr);
	case ItemType::Vector:
		return NewVector(*(Math::Geometry::Vector2D**)ptr);
	case ItemType::UUID:
		return NewUUID(*(Data::UUID**)ptr);
	case ItemType::Unknown:
	default:
		return NewNull();
	}
}

void Data::VariItem::SetFromPtr(NN<Data::VariItem> item, void *ptr, ItemType itemType)
{
	if (ptr == 0)
	{
		item->SetNull();
		return;
	}
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
	case ItemType::NI32:
		item->SetNI32(*(NInt32*)ptr);
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
		item->SetStr(*(Optional<Text::String>*)ptr);
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
	case ItemType::Date:
		item->SetDate(*(Data::Date*)ptr);
		return;
	case ItemType::ByteArr:
		item->SetByteArr(*(Data::ReadonlyArray<UInt8>**)ptr);
		return;
	case ItemType::Vector:
		{
			NN<Math::Geometry::Vector2D> vec;
			if ((*(Optional<Math::Geometry::Vector2D>*)ptr).SetTo(vec))
			{
				item->SetVector(vec);
			}
			else
			{
				item->SetNull();
			}
		}
		return;
	case ItemType::UUID:
		item->SetUUID(*(NN<Data::UUID>*)ptr);
		return;
	case ItemType::Unknown:
	default:
		return;
	}
}

void Data::VariItem::SetPtr(void *ptr, ItemType itemType, NN<VariItem> item)
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
	case ItemType::NI32:
		*(NInt32*)ptr = item->GetAsNI32();
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
				*(Optional<Text::String>*)ptr = 0;
			}
			else if (itemType == ItemType::Str)
			{
				*(Optional<Text::String>*)ptr = item->GetItemValue().str->Clone();
			}
			else if (itemType == ItemType::CStr)
			{
				const ItemValue ival = item->GetItemValue();
				*(Optional<Text::String>*)ptr = Text::String::New(ival.cstr.v, ival.cstr.leng);
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				item->GetAsString(sb);
				*(Optional<Text::String>*)ptr = Text::String::New(sb.ToCString());
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
	case ItemType::Date:
		*(Data::Date*)ptr = item->GetAsDate();
		break;
	case ItemType::ByteArr:
		*(Data::ReadonlyArray<UInt8>**)ptr = item->GetAsNewByteArr();
		break;
	case ItemType::Vector:
		*(Math::Geometry::Vector2D**)ptr = item->GetAsNewVector().OrNull();
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


void Data::VariItem::SetPtrAndNotKeep(void *ptr, ItemType itemType, NN<VariItem> item)
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
	case ItemType::NI32:
		*(NInt32*)ptr = item->GetAsNI32();
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
	case ItemType::Date:
		*(Data::Date*)ptr = item->GetAsDate();
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
			*(Math::Geometry::Vector2D**)ptr = item->GetAndRemoveVector().OrNull();
		}
		else
		{
			*(Math::Geometry::Vector2D**)ptr = item->GetAsNewVector().OrNull();
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
	if (ptr1 == ptr2)
		return true;
	if (ptr1 == 0 || ptr2 == 0)
		return false;
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
	case ItemType::NI32:
		return *(NInt32*)ptr1 == *(NInt32*)ptr2;
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
	case ItemType::Date:
		{
			Data::Date val1 = *(Data::Date*)ptr1;
			Data::Date val2 = *(Data::Date*)ptr2;
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
			NN<Math::Geometry::Vector2D> val1;
			NN<Math::Geometry::Vector2D> val2;
			if (!val1.Set(*(Math::Geometry::Vector2D**)ptr1) || !val2.Set(*(Math::Geometry::Vector2D**)ptr2))
			{
				return false;
			}
			return val1->Equals(val2, true, false);
		}
	case ItemType::UUID:
		{
			NN<Data::UUID> val1;
			NN<Data::UUID> val2;
			if (!val1.Set(*(Data::UUID**)ptr1) || !val2.Set(*(Data::UUID**)ptr2))
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
	case ItemType::NI32:
		return sizeof(NInt32);
	case ItemType::I64:
	case ItemType::U64:
		return sizeof(Int64);
	case ItemType::BOOL:
		return sizeof(Bool);
	case ItemType::CStr:
		return sizeof(Text::CString);
	case ItemType::Timestamp:
		return sizeof(Data::Timestamp);
	case ItemType::Date:
		return sizeof(Data::Date);
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

Text::CStringNN Data::VariItem::ItemTypeGetName(ItemType itemType)
{
	switch (itemType)
	{
	case ItemType::F32:
		return CSTR("Single");
	case ItemType::F64:
		return CSTR("Double");
	case ItemType::I8:
		return CSTR("Int8");
	case ItemType::U8:
		return CSTR("UInt8");
	case ItemType::I16:
		return CSTR("Int16");
	case ItemType::U16:
		return CSTR("UInt16");
	case ItemType::I32:
		return CSTR("Int32");
	case ItemType::U32:
		return CSTR("UInt32");
	case ItemType::NI32:
		return CSTR("NInt32");
	case ItemType::I64:
		return CSTR("Int64");
	case ItemType::U64:
		return CSTR("UInt64");
	case ItemType::BOOL:
		return CSTR("Bool");
	case ItemType::CStr:
		return CSTR("Text::CString");
	case ItemType::Timestamp:
		return CSTR("Data::Timestamp");
	case ItemType::Date:
		return CSTR("Data::Date");
	case ItemType::Null:
		return CSTR("Null");
	case ItemType::Str:
		return CSTR("Text::String");
	case ItemType::ByteArr:
		return CSTR("Data::ByteArray");
	case ItemType::Vector:
		return CSTR("Math::Geometry::Vector2D");
	case ItemType::UUID:
		return CSTR("Data::UUID");
	default:
	case ItemType::Unknown:
		return CSTR("Unknown");
	}
}
