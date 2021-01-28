#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/DocHeading.h"

Text::Doc::DocHeading::DocHeading()
{
	this->halign = Text::Doc::DocItem::HALIGN_NONE;
}

Text::Doc::DocHeading::~DocHeading()
{
}

Text::Doc::DocItem::DocItemType Text::Doc::DocHeading::GetItemType()
{
	return Text::Doc::DocItem::DIT_HEADING;
}

void Text::Doc::DocHeading::SetHAlignment(DocItem::HorizontalAlign halign)
{
	this->halign = halign;
}

Text::Doc::DocItem::HorizontalAlign Text::Doc::DocHeading::GetHAlignment()
{
	return this->halign;
}
