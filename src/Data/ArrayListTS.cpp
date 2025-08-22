#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListTS.h"

Data::ArrayListTS::ArrayListTS() : Data::SortableArrayListNative<Data::Timestamp>()
{
}

Data::ArrayListTS::ArrayListTS(UOSInt capacity) : Data::SortableArrayListNative<Data::Timestamp>(capacity)
{
}

NN<Data::ArrayList<Data::Timestamp>> Data::ArrayListTS::Clone() const
{
	NN<Data::ArrayListTS> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListTS(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}
