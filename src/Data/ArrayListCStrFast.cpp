#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListCStrFast.h"
#include "Text/MyString.h"

Data::ArrayListCStrFast::ArrayListCStrFast() : Data::SortableArrayList<Text::CString>()
{
}

Data::ArrayListCStrFast::ArrayListCStrFast(UOSInt capacity) : Data::SortableArrayList<Text::CString>(capacity)
{
}

Data::ArrayList<Text::CString> *Data::ArrayListCStrFast::Clone()
{
	Data::ArrayListCStrFast *newArr;
	NEW_CLASS(newArr, Data::ArrayListCStrFast(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListCStrFast::CompareItem(Text::CString obj1, Text::CString obj2)
{
	return obj1.CompareToFast(obj2);
}
