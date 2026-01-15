#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"

Data::ArrayListInt32::ArrayListInt32() : Data::SortableArrayListNative<Int32>()
{
}

Data::ArrayListInt32::ArrayListInt32(UIntOS capacity) : Data::SortableArrayListNative<Int32>(capacity)
{
}

NN<Data::ArrayListNative<Int32>> Data::ArrayListInt32::Clone() const
{
	NN<Data::ArrayListInt32> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListInt32(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}
