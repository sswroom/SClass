#include "Stdafx.h"
#include "AI/AITool.h"

Bool AI::AITool::GetUIntOSInput(NN<Data::VariObject> input, UnsafeArray<const UTF8Char> name, UIntOS defaultValue, OutParam<UIntOS> value)
{
	NN<Data::VariItem> item;
	if (!input->GetItem(name).SetTo(item))
	{
		value.Set(defaultValue);
		return true;
	}

	UInt64 u64Val;
	switch (item->GetItemType())
	{
	case Data::VariItem::ItemType::I8:
	case Data::VariItem::ItemType::I16:
	case Data::VariItem::ItemType::I32:
	case Data::VariItem::ItemType::I64:
	case Data::VariItem::ItemType::F32:
	case Data::VariItem::ItemType::F64:
	{
		Int64 i64Val = item->GetAsI64();
		if (i64Val < 0)
			return false;
		u64Val = (UInt64)i64Val;
		break;
	}
	case Data::VariItem::ItemType::U8:
	case Data::VariItem::ItemType::U16:
	case Data::VariItem::ItemType::U32:
	case Data::VariItem::ItemType::U64:
		u64Val = item->GetAsU64();
		break;
	case Data::VariItem::ItemType::Unknown:
	case Data::VariItem::ItemType::Null:
	case Data::VariItem::ItemType::Timestamp:
	case Data::VariItem::ItemType::BOOL:
	case Data::VariItem::ItemType::CStr:
	case Data::VariItem::ItemType::Date:
	case Data::VariItem::ItemType::NI32:
	case Data::VariItem::ItemType::Str:
	case Data::VariItem::ItemType::ByteArr:
	case Data::VariItem::ItemType::Vector:
	case Data::VariItem::ItemType::UUID:
	default:
		return false;
	}

	UIntOS osVal = (UIntOS)u64Val;
	if ((UInt64)osVal != u64Val)
		return false;
	value.Set(osVal);
	return true;
}
