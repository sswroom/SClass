#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/ArrayListCmp.h"
#include <stdlib.h>

Data::ArrayListCmp::ArrayListCmp() : Data::SortableArrayList<Data::IComparable*>()
{
}

Data::ArrayListCmp::ArrayListCmp(UOSInt capacity) : Data::SortableArrayList<Data::IComparable*>(capacity)
{
}

Data::ArrayList<Data::IComparable*> *Data::ArrayListCmp::Clone()
{
	Data::ArrayListCmp *newArr;
	NEW_CLASS(newArr, Data::ArrayListCmp(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListCmp::CompareItem(Data::IComparable* obj1, Data::IComparable* obj2)
{
	return obj1->CompareTo(obj2);
}
