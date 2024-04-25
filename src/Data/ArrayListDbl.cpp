#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/IComparable.h"

Data::ArrayListDbl::ArrayListDbl() : Data::SortableArrayListNative<Double>()
{
}

Data::ArrayListDbl::ArrayListDbl(UOSInt capacity) : Data::SortableArrayListNative<Double>(capacity)
{
}

NN<Data::ArrayList<Double>> Data::ArrayListDbl::Clone() const
{
	NN<Data::ArrayList<Double>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListDbl(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}
