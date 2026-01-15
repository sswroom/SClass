#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/DocLink.h"

Text::Doc::DocLink::DocLink(const UTF8Char *url)
{
	this->url = Text::StrCopyNew(url).Ptr();
}

Text::Doc::DocLink::~DocLink()
{
	Text::StrDelNew(this->url);
}

Text::Doc::DocItem::DocItemType Text::Doc::DocLink::GetItemType()
{
	return Text::Doc::DocItem::DIT_URL;
}

UIntOS Text::Doc::DocLink::Add(NN<Text::Doc::DocItem> item)
{
	if (item->GetItemType() == Text::Doc::DocItem::DIT_URL)
	{
		item.Delete();
		return (UIntOS)-1;
	}
	return this->items.Add(item);
}

const UTF8Char *Text::Doc::DocLink::GetLink()
{
	return this->url;
}
