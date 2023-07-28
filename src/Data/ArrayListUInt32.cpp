#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListUInt32.h"

Data::ArrayListUInt32::ArrayListUInt32() : Data::SortableArrayListNative<UInt32>()
{
}

Data::ArrayListUInt32::ArrayListUInt32(UOSInt capacity) : Data::SortableArrayListNative<UInt32>(capacity)
{
}

Data::ArrayListUInt32::~ArrayListUInt32()
{
}

NotNullPtr<Data::ArrayList<UInt32>> Data::ArrayListUInt32::Clone() const
{
	NotNullPtr<Data::ArrayListUInt32> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListUInt32(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}
