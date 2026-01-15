#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Comparable.h"
#include "Data/ArrayListCmp.h"

Data::ArrayListCmp::ArrayListCmp() : Data::SortableArrayListNN<Data::Comparable>()
{
}

Data::ArrayListCmp::ArrayListCmp(UIntOS capacity) : Data::SortableArrayListNN<Data::Comparable>(capacity)
{
}

NN<Data::ArrayListNN<Data::Comparable>> Data::ArrayListCmp::Clone() const
{
	NN<Data::ArrayListNN<Data::Comparable>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListCmp(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListCmp::Compare(NN<Data::Comparable> obj1, NN<Data::Comparable> obj2) const
{
	return obj1->CompareTo(obj2);
}
