#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListUInt32.h"

Data::ArrayListUInt32::ArrayListUInt32() : Data::SortableArrayListNative<UInt32>()
{
}

Data::ArrayListUInt32::ArrayListUInt32(UIntOS capacity) : Data::SortableArrayListNative<UInt32>(capacity)
{
}

Data::ArrayListUInt32::~ArrayListUInt32()
{
}

NN<Data::ArrayListNative<UInt32>> Data::ArrayListUInt32::Clone() const
{
	NN<Data::ArrayListUInt32> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListUInt32(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}
