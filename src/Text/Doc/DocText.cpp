#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/Doc/DocText.h"

Text::Doc::DocText::DocText(const UTF8Char *text)
{
	this->text = Text::StrCopyNew(text).Ptr();
}

Text::Doc::DocText::~DocText()
{
	Text::StrDelNew(this->text);
}

Text::Doc::DocItem::DocItemType Text::Doc::DocText::GetItemType()
{
	return Text::Doc::DocItem::DIT_TEXT;
}

UIntOS Text::Doc::DocText::Add(NN<Text::Doc::DocItem> item)
{
	item.Delete();
	return (UIntOS)-1;
}

const UTF8Char *Text::Doc::DocText::GetText()
{
	return this->text;
}
