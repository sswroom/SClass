#include "Stdafx.h"
#include "Data/VariObject.h"
#include "Text/JSText.h"

void Data::VariObject::SetItem(UnsafeArray<const UTF8Char> name, NN<Data::VariItem> item)
{
	if (item.Set(this->items.Put(name, item.Ptr())))
	{
		item.Delete();
	}
}

Data::VariObject::VariObject(NameType nameType)
{
	this->nameType = nameType;
}

Data::VariObject::~VariObject()
{
	NN<const Data::ArrayList<Data::VariItem*>> itemList = this->items.GetValues();
	UOSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		VariItem *item = itemList->GetItem(i);
		DEL_CLASS(item);
	}
/*	UOSInt i;
	Data::VariItem **itemArr = this->items->ToArray(&i);
	while (i-- > 0)
	{
		DEL_CLASS(itemArr[i]);
	}
	MemFree(itemArr);*/
}

Data::VariObject::NameType Data::VariObject::GetNameType() const
{
	return this->nameType;
}

Bool Data::VariObject::HasItem(UnsafeArray<const UTF8Char> name) const
{
	return this->items.Get(name) != 0;
}

Data::VariItem *Data::VariObject::GetItem(UnsafeArray<const UTF8Char> name) const
{
	return this->items.Get(name);
}

void Data::VariObject::SetItemNull(UnsafeArray<const UTF8Char> name)
{
	this->SetItem(name, Data::VariItem::NewNull());
}

void Data::VariObject::SetItemStr(UnsafeArray<const UTF8Char> name, UnsafeArrayOpt<const UTF8Char> str)
{
	this->SetItem(name, Data::VariItem::NewStrSlow(str));
}

void Data::VariObject::SetItemStr(UnsafeArray<const UTF8Char> name, Optional<Text::String> str)
{
	this->SetItem(name, Data::VariItem::NewStr(str));
}

void Data::VariObject::SetItemDateTime(UnsafeArray<const UTF8Char> name, Data::DateTime *dt)
{
	this->SetItem(name, Data::VariItem::NewDateTime(dt));
}

void Data::VariObject::SetItemTS(UnsafeArray<const UTF8Char> name, const Data::Timestamp &ts)
{
	this->SetItem(name, Data::VariItem::NewTS(ts));
}

void Data::VariObject::SetItemF32(UnsafeArray<const UTF8Char> name, Single val)
{
	this->SetItem(name, Data::VariItem::NewF32(val));
}

void Data::VariObject::SetItemF64(UnsafeArray<const UTF8Char> name, Double val)
{
	this->SetItem(name, Data::VariItem::NewF64(val));
}

void Data::VariObject::SetItemI8(UnsafeArray<const UTF8Char> name, Int8 val)
{
	this->SetItem(name, Data::VariItem::NewI8(val));
}

void Data::VariObject::SetItemU8(UnsafeArray<const UTF8Char> name, UInt8 val)
{
	this->SetItem(name, Data::VariItem::NewU8(val));
}

void Data::VariObject::SetItemI16(UnsafeArray<const UTF8Char> name, Int16 val)
{
	this->SetItem(name, Data::VariItem::NewI16(val));
}

void Data::VariObject::SetItemU16(UnsafeArray<const UTF8Char> name, UInt16 val)
{
	this->SetItem(name, Data::VariItem::NewU16(val));
}

void Data::VariObject::SetItemI32(UnsafeArray<const UTF8Char> name, Int32 val)
{
	this->SetItem(name, Data::VariItem::NewI32(val));
}

void Data::VariObject::SetItemU32(UnsafeArray<const UTF8Char> name, UInt32 val)
{
	this->SetItem(name, Data::VariItem::NewU32(val));
}

void Data::VariObject::SetItemI64(UnsafeArray<const UTF8Char> name, Int64 val)
{
	this->SetItem(name, Data::VariItem::NewI64(val));
}

void Data::VariObject::SetItemU64(UnsafeArray<const UTF8Char> name, UInt64 val)
{
	this->SetItem(name, Data::VariItem::NewU64(val));
}

void Data::VariObject::SetItemBool(UnsafeArray<const UTF8Char> name, Bool val)
{
	this->SetItem(name, Data::VariItem::NewBool(val));
}

void Data::VariObject::SetItemByteArray(UnsafeArray<const UTF8Char> name, const UInt8 *arr, UOSInt cnt)
{
	this->SetItem(name, Data::VariItem::NewByteArr(arr, cnt));
}

void Data::VariObject::SetItemVector(UnsafeArray<const UTF8Char> name, Optional<Math::Geometry::Vector2D> vec)
{
	this->SetItem(name, Data::VariItem::NewVector(vec));
}

void Data::VariObject::SetItemUUID(UnsafeArray<const UTF8Char> name, Data::UUID *uuid)
{
	this->SetItem(name, Data::VariItem::NewUUID(uuid));
}

void Data::VariObject::SetItemVectorDirect(UnsafeArray<const UTF8Char> name, Optional<Math::Geometry::Vector2D> vec)
{
	this->SetItem(name, Data::VariItem::NewVectorDirect(vec));
}

void Data::VariObject::SetItemUUIDDirect(UnsafeArray<const UTF8Char> name, Data::UUID *uuid)
{
	this->SetItem(name, Data::VariItem::NewUUIDDirect(uuid));
}

void Data::VariObject::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sb->AppendUTF8Char('{');
	NN<Data::ArrayList<UnsafeArrayOpt<const UTF8Char>>> keys = this->items.GetKeys();
	NN<const Data::ArrayList<VariItem*>> values = this->items.GetValues();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
		}
		sptr = Text::JSText::ToJSTextDQuote(sbuff, keys->GetItem(i).Or(U8STR("")));
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendUTF8Char(':');
		values->GetItem(i)->ToString(sb);
		i++;
	}
/*	UOSInt i = 0;
	UOSInt j;
	UnsafeArray<const UTF8Char> *keys = this->items->ToNameArray(&j);
	VariItem **values = this->items->ToArray(&j);
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
		}
		Text::JSText::ToJSTextDQuote(sbuff, keys[i]);
		sb->Append(sbuff);
		sb->AppendUTF8Char(':');
		values[i]->ToString(sb);
		i++;
	}
	MemFree(keys);
	MemFree(values);*/
	sb->AppendUTF8Char('}');
}

NN<Data::Class> Data::VariObject::CreateClass() const
{
	NN<Data::Class> cls;
	OSInt currPos = 0;
	NEW_CLASSNN(cls, Data::Class(0));
	NN<Data::ArrayList<UnsafeArrayOpt<const UTF8Char>>> keys = this->items.GetKeys();
	NN<const Data::ArrayList<VariItem*>> values = this->items.GetValues();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		currPos += (OSInt)cls->AddField(Text::CStringNN::FromPtr(keys->GetItem(i).Or(U8STR(""))), currPos, values->GetItem(i)->GetItemType(), false);
		i++;
	}
/*	UOSInt i = 0;
	UOSInt j;
	UnsafeArray<const UTF8Char> *keys = this->items->ToNameArray(&j);
	VariItem **values = this->items->ToArray(&j);
	while (i < j)
	{
		currPos += (OSInt)cls->AddField(keys[i], currPos, values[i]->GetItemType());
		i++;
	}
	MemFree(keys);
	MemFree(values);*/
	return cls;
}
