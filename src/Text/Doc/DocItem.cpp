#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/DocItem.h"

Text::Doc::DocItem::DocItem()
{
}

Text::Doc::DocItem::~DocItem()
{
	this->items.DeleteAll();
}

UOSInt Text::Doc::DocItem::Add(NN<Text::Doc::DocItem> item)
{
	return this->items.Add(item);
}

UOSInt Text::Doc::DocItem::GetCount() const
{
	return this->items.GetCount();
}

NN<Text::Doc::DocItem> Text::Doc::DocItem::GetItemNoCheck(UOSInt index) const
{
	return this->items.GetItemNoCheck(index);
}

Optional<Text::Doc::DocItem> Text::Doc::DocItem::GetItem(UOSInt index) const
{
	return this->items.GetItem(index);
}
