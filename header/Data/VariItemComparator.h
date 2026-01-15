#ifndef _SM_DATA_VARIITEMCOMPARATOR
#define _SM_DATA_VARIITEMCOMPARATOR
#include "Data/Comparator.hpp"
#include "Data/DataComparer.h"
#include "Data/VariItem.h"

namespace Data
{
	class VariItemComparator : public Comparator<NN<Data::VariItem>>
	{
	public:
		VariItemComparator()
		{
		}

		virtual ~VariItemComparator()
		{
		}

		virtual IntOS Compare(NN<Data::VariItem> a, NN<Data::VariItem> b) const
		{
			if (a->GetItemType() != b->GetItemType())
				return 0;
			VariItem::ItemValue av = a->GetItemValue();
			VariItem::ItemValue bv = b->GetItemValue();
			switch (a->GetItemType())
			{
			default:
			case VariItem::ItemType::Unknown:
			case VariItem::ItemType::Null:
				return 0;
			case VariItem::ItemType::Timestamp:
				return av.ts.CompareTo(bv.ts);
			case VariItem::ItemType::F32:
				return Data::DataComparer::Compare(av.f32, bv.f32);
			case VariItem::ItemType::F64:
				return Data::DataComparer::Compare(av.f64, bv.f64);
			case VariItem::ItemType::I8:
				return Data::DataComparer::Compare(av.i8, bv.i8);
			case VariItem::ItemType::U8:
				return Data::DataComparer::Compare(av.u8, bv.u8);
			case VariItem::ItemType::I16:
				return Data::DataComparer::Compare(av.i16, bv.i16);
			case VariItem::ItemType::U16:
				return Data::DataComparer::Compare(av.u16, bv.u16);
			case VariItem::ItemType::I32:
				return Data::DataComparer::Compare(av.i32, bv.i32);
			case VariItem::ItemType::U32:
				return Data::DataComparer::Compare(av.u32, bv.u32);
			case VariItem::ItemType::I64:
				return Data::DataComparer::Compare(av.i64, bv.i64);
			case VariItem::ItemType::U64:
				return Data::DataComparer::Compare(av.u64, bv.u64);
			case VariItem::ItemType::BOOL:
				return (av.boolean == bv.boolean)?0:(av.boolean?1:-1);
			case VariItem::ItemType::CStr:
				return Text::CStringNN(av.cstr.v, av.cstr.leng).CompareTo(Text::CStringNN(bv.cstr.v, bv.cstr.leng));
			case VariItem::ItemType::Date:
				return av.date.CompareTo(bv.date);
			case VariItem::ItemType::NI32:
				return (av.i32 > bv.i32)?1:((av.i32 < bv.i32)?-1:0);
			case VariItem::ItemType::Str:
				return Data::DataComparer::Compare(av.str, bv.str);
			case VariItem::ItemType::ByteArr:
				return Data::DataComparer::Compare(av.byteArr, bv.byteArr);
			case VariItem::ItemType::Vector:
				return Data::DataComparer::Compare(av.vector, bv.vector);
			case VariItem::ItemType::UUID:
				return av.uuid->CompareTo(bv.uuid);
			}
		}
	};

	class VariItemComparatorInv : public Comparator<NN<Data::VariItem>>
	{
	public:
		VariItemComparatorInv()
		{
		}

		virtual ~VariItemComparatorInv()
		{
		}

		virtual IntOS Compare(NN<Data::VariItem> b, NN<Data::VariItem> a) const
		{
			if (a->GetItemType() != b->GetItemType())
				return 0;
			VariItem::ItemValue av = a->GetItemValue();
			VariItem::ItemValue bv = b->GetItemValue();
			switch (a->GetItemType())
			{
			default:
			case VariItem::ItemType::Unknown:
			case VariItem::ItemType::Null:
				return 0;
			case VariItem::ItemType::Timestamp:
				return av.ts.CompareTo(bv.ts);
			case VariItem::ItemType::F32:
				return Data::DataComparer::Compare(av.f32, bv.f32);
			case VariItem::ItemType::F64:
				return Data::DataComparer::Compare(av.f64, bv.f64);
			case VariItem::ItemType::I8:
				return Data::DataComparer::Compare(av.i8, bv.i8);
			case VariItem::ItemType::U8:
				return Data::DataComparer::Compare(av.u8, bv.u8);
			case VariItem::ItemType::I16:
				return Data::DataComparer::Compare(av.i16, bv.i16);
			case VariItem::ItemType::U16:
				return Data::DataComparer::Compare(av.u16, bv.u16);
			case VariItem::ItemType::I32:
				return Data::DataComparer::Compare(av.i32, bv.i32);
			case VariItem::ItemType::U32:
				return Data::DataComparer::Compare(av.u32, bv.u32);
			case VariItem::ItemType::I64:
				return Data::DataComparer::Compare(av.i64, bv.i64);
			case VariItem::ItemType::U64:
				return Data::DataComparer::Compare(av.u64, bv.u64);
			case VariItem::ItemType::BOOL:
				return (av.boolean == bv.boolean)?0:(av.boolean?1:-1);
			case VariItem::ItemType::CStr:
				return Text::CStringNN(av.cstr.v, av.cstr.leng).CompareTo(Text::CStringNN(bv.cstr.v, bv.cstr.leng));
			case VariItem::ItemType::Date:
				return av.date.CompareTo(bv.date);
			case VariItem::ItemType::NI32:
				return (av.i32 > bv.i32)?1:((av.i32 < bv.i32)?-1:0);
			case VariItem::ItemType::Str:
				return Data::DataComparer::Compare(av.str, bv.str);
			case VariItem::ItemType::ByteArr:
				return Data::DataComparer::Compare(av.byteArr, bv.byteArr);
			case VariItem::ItemType::Vector:
				return Data::DataComparer::Compare(av.vector, bv.vector);
			case VariItem::ItemType::UUID:
				return av.uuid->CompareTo(bv.uuid);
			}
		}
	};
}
#endif
