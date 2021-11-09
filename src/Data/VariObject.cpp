#include "Stdafx.h"
#include "Data/VariObject.h"

void Data::VariObject::SetItem(const UTF8Char *name, Data::VariItem *item)
{
	item = this->items->Put(name, item);
	SDEL_CLASS(item);
}

Data::VariObject::VariObject()
{
	NEW_CLASS(this->items, Data::StringUTF8Map<Data::VariItem*>());
}

Data::VariObject::~VariObject()
{
	Data::ArrayList<Data::VariItem*> *itemList = this->items->GetValues();
	LIST_FREE_FUNC(itemList, DEL_CLASS);
	DEL_CLASS(itemList);
}

Bool Data::VariObject::HasItem(const UTF8Char *name)
{
	return this->items->Get(name) != 0;
}

Data::VariItem *Data::VariObject::GetItem(const UTF8Char *name)
{
	return this->items->Get(name);
}

void Data::VariObject::SetItemNull(const UTF8Char *name)
{
	this->SetItem(name, Data::VariItem::NewNull());
}

void Data::VariObject::SetItemStr(const UTF8Char *name, const UTF8Char *str)
{
	this->SetItem(name, Data::VariItem::NewStr(str));
}

void Data::VariObject::SetItemDate(const UTF8Char *name, Data::DateTime *dt)
{
	this->SetItem(name, Data::VariItem::NewDate(dt));
}

void Data::VariObject::SetItemF32(const UTF8Char *name, Single val)
{
	this->SetItem(name, Data::VariItem::NewF32(val));
}

void Data::VariObject::SetItemF64(const UTF8Char *name, Double val)
{
	this->SetItem(name, Data::VariItem::NewF64(val));
}

void Data::VariObject::SetItemI8(const UTF8Char *name, Int8 val)
{
	this->SetItem(name, Data::VariItem::NewI8(val));
}

void Data::VariObject::SetItemU8(const UTF8Char *name, UInt8 val)
{
	this->SetItem(name, Data::VariItem::NewU8(val));
}

void Data::VariObject::SetItemI16(const UTF8Char *name, Int16 val)
{
	this->SetItem(name, Data::VariItem::NewI16(val));
}

void Data::VariObject::SetItemU16(const UTF8Char *name, UInt16 val)
{
	this->SetItem(name, Data::VariItem::NewU16(val));
}

void Data::VariObject::SetItemI32(const UTF8Char *name, Int32 val)
{
	this->SetItem(name, Data::VariItem::NewI32(val));
}

void Data::VariObject::SetItemU32(const UTF8Char *name, UInt32 val)
{
	this->SetItem(name, Data::VariItem::NewU32(val));
}

void Data::VariObject::SetItemI64(const UTF8Char *name, Int64 val)
{
	this->SetItem(name, Data::VariItem::NewI64(val));
}

void Data::VariObject::SetItemU64(const UTF8Char *name, UInt64 val)
{
	this->SetItem(name, Data::VariItem::NewU64(val));
}

void Data::VariObject::SetItemBool(const UTF8Char *name, Bool val)
{
	this->SetItem(name, Data::VariItem::NewBool(val));
}

void Data::VariObject::SetItemByteArray(const UTF8Char *name, const UInt8 *arr, UOSInt cnt)
{
	this->SetItem(name, Data::VariItem::NewByteArr(arr, cnt));
}

void Data::VariObject::SetItemVector(const UTF8Char *name, Math::Vector2D *vec)
{
	this->SetItem(name, Data::VariItem::NewVector(vec));
}

void Data::VariObject::SetItemUUID(const UTF8Char *name, Data::UUID *uuid)
{
	this->SetItem(name, Data::VariItem::NewUUID(uuid));
}
