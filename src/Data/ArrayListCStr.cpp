#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListCStr.h"
#include "Text/MyString.h"

Data::ArrayListCStr::ArrayListCStr() : Data::SortableArrayList<Text::CString>()
{
}

Data::ArrayListCStr::ArrayListCStr(UOSInt capacity) : Data::SortableArrayList<Text::CString>(capacity)
{
}

Data::ArrayList<Text::CString> *Data::ArrayListCStr::Clone()
{
	Data::ArrayListCStr *newArr;
	NEW_CLASS(newArr, Data::ArrayListCStr(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListCStr::CompareItem(Text::CString obj1, Text::CString obj2)
{
	return obj1.CompareTo(obj2);
}
