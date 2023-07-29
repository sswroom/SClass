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

NotNullPtr<Data::ArrayList<Text::CString>> Data::ArrayListCStrFast::Clone() const
{
	NotNullPtr<Data::ArrayList<Text::CString>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListCStrFast(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListCStrFast::Compare(Text::CString obj1, Text::CString obj2) const
{
	return obj1.CompareToFast(obj2);
}
