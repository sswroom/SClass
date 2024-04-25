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

NN<Data::ArrayList<Text::CString>> Data::ArrayListCStr::Clone() const
{
	NN<Data::ArrayList<Text::CString>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListCStr(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListCStr::Compare(Text::CString obj1, Text::CString obj2) const
{
	return obj1.CompareTo(obj2);
}
