#include "Stdafx.h"
#include "Data/VariObject.h"
#include "Text/JSText.h"

Data::VariObject::~VariObject()
{
	NN<const Data::ArrayList<Data::VariItem*>> itemList = this->items.GetValues();
	UOSInt i = itemList->GetCount();
	while (i-- > 0)
	{
		VariItem *item = itemList->GetItem(i);
		DEL_CLASS(item);
	}
}

void Data::VariObject::ToString(NN<Text::StringBuilderUTF8> sb) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sb->AppendUTF8Char('{');
	NN<Data::ArrayList<UnsafeArrayOpt<const UTF8Char>>> keys = this->items.GetKeys();
	NN<const Data::ArrayList<VariItem*>> values = this->items.GetValues();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		if (i > 0)
		{
			sb->AppendUTF8Char(',');
		}
		sptr = Text::JSText::ToJSTextDQuote(sbuff, keys->GetItem(i).Or(U8STR("")));
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb->AppendUTF8Char(':');
		values->GetItem(i)->ToString(sb);
		i++;
	}
	sb->AppendUTF8Char('}');
}

NN<Data::Class> Data::VariObject::CreateClass() const
{
	NN<Data::Class> cls;
	OSInt currPos = 0;
	NEW_CLASSNN(cls, Data::Class(0));
	NN<Data::ArrayList<UnsafeArrayOpt<const UTF8Char>>> keys = this->items.GetKeys();
	NN<const Data::ArrayList<VariItem*>> values = this->items.GetValues();
	UOSInt i = 0;
	UOSInt j = keys->GetCount();
	while (i < j)
	{
		currPos += (OSInt)cls->AddField(Text::CStringNN::FromPtr(keys->GetItem(i).Or(U8STR(""))), currPos, values->GetItem(i)->GetItemType(), false);
		i++;
	}
	return cls;
}
