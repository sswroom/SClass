#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/DocText.h"

Text::Doc::DocText::DocText(const UTF8Char *text)
{
	this->text = Text::StrCopyNew(text);
}

Text::Doc::DocText::~DocText()
{
	Text::StrDelNew(this->text);
}

Text::Doc::DocItem::DocItemType Text::Doc::DocText::GetItemType()
{
	return Text::Doc::DocItem::DIT_TEXT;
}

UOSInt Text::Doc::DocText::Add(Text::Doc::DocItem *item)
{
	DEL_CLASS(item);
	return (UOSInt)-1;
}

const UTF8Char *Text::Doc::DocText::GetText()
{
	return this->text;
}
