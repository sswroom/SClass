#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListTS.h"

Data::ArrayListTS::ArrayListTS() : Data::SortableArrayListObj<Data::Timestamp>()
{
}

Data::ArrayListTS::ArrayListTS(UIntOS capacity) : Data::SortableArrayListObj<Data::Timestamp>(capacity)
{
}

NN<Data::ArrayListObj<Data::Timestamp>> Data::ArrayListTS::Clone() const
{
	NN<Data::ArrayListTS> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListTS(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListTS::Compare(Data::Timestamp a, Data::Timestamp b) const
{
	if (a < b)
	{
		return -1;
	}
	else if (a > b)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}