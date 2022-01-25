#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListUInt32.h"

Data::ArrayListUInt32::ArrayListUInt32() : Data::SortableArrayListNative<UInt32>()
{
}

Data::ArrayListUInt32::ArrayListUInt32(UOSInt capacity) : Data::SortableArrayListNative<UInt32>(capacity)
{
}

Data::ArrayList<UInt32> *Data::ArrayListUInt32::Clone()
{
	Data::ArrayListUInt32 *newArr;
	NEW_CLASS(newArr, Data::ArrayListUInt32(this->capacity));
	newArr->AddAll(this);
	return newArr;
}
