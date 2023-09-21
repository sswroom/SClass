#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListStrC.h"

Data::ArrayListStrC::ArrayListStrC() : Data::SortableArrayList<const Char*>()
{
}

Data::ArrayListStrC::ArrayListStrC(UOSInt capacity) : Data::SortableArrayList<const Char*>(capacity)
{
}

NotNullPtr<Data::ArrayList<const Char*>> Data::ArrayListStrC::Clone() const
{
	NotNullPtr<Data::ArrayListStrC> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStrC(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListStrC::Compare(const Char* obj1, const Char* obj2) const
{
	return Text::StrCompare(obj1, obj2);
}
