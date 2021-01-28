#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/DocValidator.h"

Text::Doc::DocValidator::DocValidator()
{
}

Text::Doc::DocValidator::~DocValidator()
{
}

Text::Doc::DocItem::DocItemType Text::Doc::DocValidator::GetItemType()
{
	return Text::Doc::DocItem::DIT_VALIDATOR;
}

UOSInt Text::Doc::DocValidator::Add(Text::Doc::DocItem *item)
{
	DEL_CLASS(item);
	return -1;
}
