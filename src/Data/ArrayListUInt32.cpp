#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListUInt32.h"

Data::ArrayListUInt32::ArrayListUInt32() : Data::SortableArrayList<UInt32>()
{
}

Data::ArrayListUInt32::ArrayListUInt32(UOSInt capacity) : Data::SortableArrayList<UInt32>(capacity)
{
}

Data::ArrayList<UInt32> *Data::ArrayListUInt32::Clone()
{
	Data::ArrayListUInt32 *newArr;
	NEW_CLASS(newArr, Data::ArrayListUInt32(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListUInt32::CompareItem(UInt32 obj1, UInt32 obj2)
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
