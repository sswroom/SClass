#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/ArrayListCmp.h"

Data::ArrayListCmp::ArrayListCmp() : Data::SortableArrayList<Data::IComparable*>()
{
}

Data::ArrayListCmp::ArrayListCmp(UOSInt capacity) : Data::SortableArrayList<Data::IComparable*>(capacity)
{
}

NN<Data::ArrayList<Data::IComparable*>> Data::ArrayListCmp::Clone() const
{
	NN<Data::ArrayList<Data::IComparable*>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListCmp(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListCmp::Compare(Data::IComparable* obj1, Data::IComparable* obj2) const
{
	return obj1->CompareTo(obj2);
}
