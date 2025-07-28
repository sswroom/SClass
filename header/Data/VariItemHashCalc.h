#ifndef _SM_DATA_VARIITEMHASHCALC
#define _SM_DATA_VARIITEMHASHCALC
#include "Crypto/Hash/CRC32RC.h"
#include "Data/VariItem.h"

namespace Data
{
	class VariItemHashCalc
	{
	private:
		Crypto::Hash::CRC32RC crc;
	public:
		VariItemHashCalc()
		{
		}

		~VariItemHashCalc()
		{
		}

		Int64 Hash(NN<VariItem> item) const
		{
			VariItem::ItemValue iv = item->GetItemValue();
			switch (item->GetItemType())
			{
			default:
			case VariItem::ItemType::Unknown:
			case VariItem::ItemType::Null:
				return 0;
			case VariItem::ItemType::Timestamp:
				return iv.ts.ToTicks();
			case VariItem::ItemType::F32:
				return *(Int32*)&iv.f32;
			case VariItem::ItemType::F64:
				return *(Int64*)&iv.f64;
			case VariItem::ItemType::I8:
				return iv.i8;
			case VariItem::ItemType::U8:
				return (Int64)iv.u8;
			case VariItem::ItemType::I16:
				return iv.i16;
			case VariItem::ItemType::U16:
				return (Int64)iv.u16;
			case VariItem::ItemType::I32:
				return iv.i32;
			case VariItem::ItemType::U32:
				return (Int64)iv.u32;
			case VariItem::ItemType::I64:
				return iv.i64;
			case VariItem::ItemType::U64:
				return (Int64)iv.u64;
			case VariItem::ItemType::BOOL:
				return iv.boolean?1:0;
			case VariItem::ItemType::CStr:
				return crc.CalcDirect(iv.cstr.v, iv.cstr.leng);
			case VariItem::ItemType::Date:
				return iv.date.GetTotalDays();
			case VariItem::ItemType::NI32:
				return iv.i32;
			case VariItem::ItemType::Str:
				return (iv.str == 0)?0:crc.CalcDirect(iv.str->v, iv.str->leng);
			case VariItem::ItemType::ByteArr:
				return (iv.byteArr == 0)?0:crc.CalcDirect(iv.byteArr->GetArray(), iv.byteArr->GetCount());
			case VariItem::ItemType::Vector:
				return 0;
			case VariItem::ItemType::UUID:
				return crc.CalcDirect(iv.uuid->GetBytes(), 16);
			}
		}
	};
}
#endif
