#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"

Data::ArrayListInt32::ArrayListInt32() : Data::SortableArrayList<Int32>()
{
}

Data::ArrayListInt32::ArrayListInt32(UOSInt capacity) : Data::SortableArrayList<Int32>(capacity)
{
}

Data::ArrayList<Int32> *Data::ArrayListInt32::Clone()
{
	Data::ArrayListInt32 *newArr;
	NEW_CLASS(newArr, Data::ArrayListInt32(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

OSInt Data::ArrayListInt32::CompareItem(Int32 obj1, Int32 obj2)
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
