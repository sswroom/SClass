#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListCStr.h"
#include "Text/MyString.h"

Data::ArrayListCStr::ArrayListCStr() : Data::SortableArrayList<Text::CStringNN>()
{
}

Data::ArrayListCStr::ArrayListCStr(UOSInt capacity) : Data::SortableArrayList<Text::CStringNN>(capacity)
{
}

NN<Data::ArrayList<Text::CStringNN>> Data::ArrayListCStr::Clone() const
{
	NN<Data::ArrayList<Text::CStringNN>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListCStr(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListCStr::Compare(Text::CStringNN obj1, Text::CStringNN obj2) const
{
	return obj1.CompareTo(obj2);
}
