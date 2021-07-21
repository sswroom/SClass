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

Data::ArrayList<const Char*> *Data::ArrayListStrC::Clone()
{
	Data::ArrayListStrC *newArr;
	NEW_CLASS(newArr, Data::ArrayListStrC(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

OSInt Data::ArrayListStrC::CompareItem(const Char* obj1, const Char* obj2)
{
	return Text::StrCompare(obj1, obj2);
}
