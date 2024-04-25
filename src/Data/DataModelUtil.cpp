#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Data/DataModelUtil.h"
#include <stdio.h>

Bool Data::DataModelUtil::FillFromDBReader(NN<Data::Class> cls, AnyType value, NN<DB::DBReader> r)
{
	NN<Data::Class::FieldInfo> field;
	UOSInt i;
	UOSInt j = r->ColCount();
	if (j != cls->GetFieldCount())
	{
		printf("Class count not match with reader count: %d != %d\r\n", (UInt32)cls->GetFieldCount(), (UInt32)j);
		return false;
	}
	Text::StringBuilderUTF8 sb;
	Data::VariItem item;
	i = 0;
	while (i < j)
	{
		if (!cls->GetFieldInfo(i).SetTo(field))
		{
			printf("Field index %d not found\r\n", (UInt32)i);
			return false;
		}
		void *ptr = (void*)(field->ofst + (OSInt)value.p);
		if (field->typeName.NotNull())
		{
			if (field->itemType == Data::VariItem::ItemType::Str && field->byNameFunc)
			{
				sb.ClearStr();
				r->GetStr(i, sb);
				item.SetI64(field->byNameFunc(sb.ToCString()));
				if (_OSINT_SIZE == 64)
				{
					Data::VariItem::SetPtr(ptr, Data::VariItem::ItemType::I64, item);
				}
				else
				{
					Data::VariItem::SetPtr(ptr, Data::VariItem::ItemType::I32, item);
				}
			}
			else
			{
				item.SetI32(r->GetInt32(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
			}
		}
		else 
		{
			switch (field->itemType)
			{
			case Data::VariItem::ItemType::Null:
				item.SetNull();
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::Timestamp:
				item.SetDate(r->GetTimestamp(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::F32:
			case Data::VariItem::ItemType::F64:
				item.SetF64(r->GetDbl(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::I8:
				item.SetI8((Int8)r->GetInt32(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::U8:
				item.SetU8((UInt8)r->GetInt32(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::I16:
				item.SetI16((Int16)r->GetInt32(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::U16:
				item.SetU16((UInt16)r->GetInt32(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::I32:
				if (field->notNull)
				{
					item.SetI32(r->GetInt32(i));
					Data::VariItem::SetPtr(ptr, field->itemType, item);
				}
				else
				{
					item.SetNI32(r->GetNInt32(i));
					Data::VariItem::SetPtr(ptr, Data::VariItem::ItemType::NI32, item);
				}
				break;
			case Data::VariItem::ItemType::U32:
				item.SetU32((UInt32)r->GetInt32(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::NI32:
				item.SetNI32(r->GetNInt32(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::I64:
				item.SetI64(r->GetInt64(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::U64:
				item.SetU64((UInt64)r->GetInt64(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::CStr:
			case Data::VariItem::ItemType::Str:
			case Data::VariItem::ItemType::Unknown:
			default:
				sb.ClearStr();
				if (r->GetStr(i, sb))
				{
					item.SetStr(sb.v, sb.leng);
				}
				else
				{
					item.SetNull();
				}
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::BOOL:
				item.SetBool(r->GetBool(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::Date:
				item.SetDate(r->GetDate(i));
				Data::VariItem::SetPtr(ptr, field->itemType, item);
				break;
			case Data::VariItem::ItemType::ByteArr:
				if (r->IsNull(i))
				{
					item.SetNull();
					Data::VariItem::SetPtr(ptr, field->itemType, item);
					break;
				}
				else
				{
					Data::ByteBuffer bytes(r->GetBinarySize(i));
					r->GetBinary(i, bytes.Ptr());
					item.SetByteArr(bytes.Ptr(), bytes.GetSize());
					Data::VariItem::SetPtr(ptr, field->itemType, item);
					break;
				}
			case Data::VariItem::ItemType::Vector:
				{
					NN<Math::Geometry::Vector2D> vec;
					if (r->GetVector(i).SetTo(vec))
					{
						item.SetVectorDirect(vec);
					}
					else
					{
						item.SetNull();
					}
					Data::VariItem::SetPtr(ptr, field->itemType, item);
					break;
				}
			case Data::VariItem::ItemType::UUID:
				{
					Data::UUID uuid;
					r->GetUUID(i, uuid);
					item.SetUUID(uuid);
					Data::VariItem::SetPtr(ptr, field->itemType, item);
					break;
				}
			}
		}
		i++;
	}
	return true;
}
