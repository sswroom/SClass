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

UOSInt Text::Doc::DocValidator::Add(NN<Text::Doc::DocItem> item)
{
	item.Delete();
	return (UOSInt)-1;
}
