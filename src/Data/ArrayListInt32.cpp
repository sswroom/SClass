#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListInt32.h"

Data::ArrayListInt32::ArrayListInt32() : Data::SortableArrayListNative<Int32>()
{
}

Data::ArrayListInt32::ArrayListInt32(UOSInt capacity) : Data::SortableArrayListNative<Int32>(capacity)
{
}

NotNullPtr<Data::ArrayList<Int32>> Data::ArrayListInt32::Clone() const
{
	NotNullPtr<Data::ArrayListInt32> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListInt32(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}
