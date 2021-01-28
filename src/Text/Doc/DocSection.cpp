#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/DocSection.h"

Text::Doc::DocSection::DocSection(Media::PaperSize::PaperType paperType, Bool isLandscape)
{
	this->paperType = paperType;
	this->isLandscape = isLandscape;
	NEW_CLASS(this->items, Data::ArrayList<Text::Doc::DocItem*>());
}

Text::Doc::DocSection::~DocSection()
{
	Text::Doc::DocItem *item;
	OSInt i;
	i = this->items->GetCount();
	while (i-- > 0)
	{
		item = this->items->GetItem(i);
		DEL_CLASS(item);
	}
	DEL_CLASS(this->items);
}

Media::PaperSize::PaperType Text::Doc::DocSection::GetPaperType()
{
	return this->paperType;
}

Bool Text::Doc::DocSection::IsLandscape()
{
	return this->isLandscape;
}

UOSInt Text::Doc::DocSection::Add(Text::Doc::DocItem *item)
{
	return this->items->Add(item);
}

UOSInt Text::Doc::DocSection::GetCount()
{
	return this->items->GetCount();
}

Text::Doc::DocItem *Text::Doc::DocSection::GetItem(UOSInt index)
{
	return this->items->GetItem(index);
}
