#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListSng.h"
#include "Data/IComparable.h"
#include <stdlib.h>

Data::ArrayListSng::ArrayListSng() : Data::SortableArrayListNative<Single>()
{
}

Data::ArrayListSng::ArrayListSng(OSInt Capacity) : Data::SortableArrayListNative<Single>(Capacity)
{
}

NN<Data::ArrayList<Single>> Data::ArrayListSng::Clone() const
{
	NN<Data::ArrayListSng> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListSng(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}
