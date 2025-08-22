#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListCStrFast.h"
#include "Text/MyString.h"

Data::ArrayListCStrFast::ArrayListCStrFast() : Data::SortableArrayList<Text::CStringNN>()
{
}

Data::ArrayListCStrFast::ArrayListCStrFast(UOSInt capacity) : Data::SortableArrayList<Text::CStringNN>(capacity)
{
}

NN<Data::ArrayList<Text::CStringNN>> Data::ArrayListCStrFast::Clone() const
{
	NN<Data::ArrayList<Text::CStringNN>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListCStrFast(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListCStrFast::Compare(Text::CStringNN obj1, Text::CStringNN obj2) const
{
	return obj1.CompareToFast(obj2);
}
