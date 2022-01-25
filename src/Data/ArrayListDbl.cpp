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

Data::ArrayList<Double> *Data::ArrayListDbl::Clone()
{
	Data::ArrayListDbl *newArr = new Data::ArrayListDbl(this->capacity);
	newArr->AddAll(this);
	return newArr;
}
