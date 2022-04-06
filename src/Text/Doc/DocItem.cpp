#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/DocItem.h"

Text::Doc::DocItem::DocItem()
{
	NEW_CLASS(this->items, Data::ArrayList<Text::Doc::DocItem*>());
}

Text::Doc::DocItem::~DocItem()
{
	DocItem *item;
	UOSInt i;
	i = this->items->GetCount();
	while (i-- > 0)
	{
		item = this->items->RemoveAt(i);
		DEL_CLASS(item);
	}
	DEL_CLASS(this->items);
}

UOSInt Text::Doc::DocItem::Add(Text::Doc::DocItem *item)
{
	return this->items->Add(item);
}

UOSInt Text::Doc::DocItem::GetCount() const
{
	return this->items->GetCount();
}

Text::Doc::DocItem *Text::Doc::DocItem::GetItem(UOSInt index) const
{
	return this->items->GetItem(index);
}
