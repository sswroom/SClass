#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListDbl.h"
#include "Data/IComparable.h"
#include <stdlib.h>

Data::ArrayListDbl::ArrayListDbl() : Data::SortableArrayList<Double>()
{
}

Data::ArrayListDbl::ArrayListDbl(UOSInt capacity) : Data::SortableArrayList<Double>(capacity)
{
}

Data::ArrayList<Double> *Data::ArrayListDbl::Clone()
{
	Data::ArrayListDbl *newArr = new Data::ArrayListDbl(this->capacity);
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListDbl::CompareItem(Double obj1, Double obj2)
{
	if (obj1 > obj2)
	{
		return 1;
	}
	else if (obj1 < obj2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}
