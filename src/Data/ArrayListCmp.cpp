#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/IComparable.h"
#include "Data/ArrayListCmp.h"

Data::ArrayListCmp::ArrayListCmp() : Data::SortableArrayListNN<Data::IComparable>()
{
}

Data::ArrayListCmp::ArrayListCmp(UOSInt capacity) : Data::SortableArrayListNN<Data::IComparable>(capacity)
{
}

NN<Data::ArrayListNN<Data::IComparable>> Data::ArrayListCmp::Clone() const
{
	NN<Data::ArrayListNN<Data::IComparable>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListCmp(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListCmp::Compare(NN<Data::IComparable> obj1, NN<Data::IComparable> obj2) const
{
	return obj1->CompareTo(obj2);
}
