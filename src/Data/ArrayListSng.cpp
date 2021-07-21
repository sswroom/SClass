#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListSng.h"
#include "Data/IComparable.h"
#include <stdlib.h>

Data::ArrayListSng::ArrayListSng() : Data::SortableArrayList<Single>()
{
}

Data::ArrayListSng::ArrayListSng(OSInt Capacity) : Data::SortableArrayList<Single>(Capacity)
{
}

Data::ArrayList<Single> *Data::ArrayListSng::Clone()
{
	Data::ArrayListSng *newArr;
	NEW_CLASS(newArr, Data::ArrayListSng(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

OSInt Data::ArrayListSng::CompareItem(Single obj1, Single obj2)
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
