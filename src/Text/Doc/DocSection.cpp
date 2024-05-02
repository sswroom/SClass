#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/DocSection.h"

Text::Doc::DocSection::DocSection(Media::PaperSize::PaperType paperType, Bool isLandscape)
{
	this->paperType = paperType;
	this->isLandscape = isLandscape;
}

Text::Doc::DocSection::~DocSection()
{
	this->items.DeleteAll();
}

Media::PaperSize::PaperType Text::Doc::DocSection::GetPaperType()
{
	return this->paperType;
}

Bool Text::Doc::DocSection::IsLandscape()
{
	return this->isLandscape;
}

UOSInt Text::Doc::DocSection::Add(NN<Text::Doc::DocItem> item)
{
	return this->items.Add(item);
}

UOSInt Text::Doc::DocSection::GetCount() const
{
	return this->items.GetCount();
}

NN<Text::Doc::DocItem> Text::Doc::DocSection::GetItemNoCheck(UOSInt index) const
{
	return this->items.GetItemNoCheck(index);
}

Optional<Text::Doc::DocItem> Text::Doc::DocSection::GetItem(UOSInt index) const
{
	return this->items.GetItem(index);
}
