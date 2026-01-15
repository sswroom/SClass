#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListICaseStringNN.h"
#include "Text/MyString.h"

Data::ArrayListICaseStringNN::ArrayListICaseStringNN() : Data::ArrayListStringNN()
{
}

Data::ArrayListICaseStringNN::ArrayListICaseStringNN(UIntOS capacity) : Data::ArrayListStringNN(capacity)
{
}

NN<Data::ArrayListNN<Text::String>> Data::ArrayListICaseStringNN::Clone() const
{
	NN<Data::ArrayListNN<Text::String>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListICaseStringNN(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListICaseStringNN::Compare(NN<Text::String> obj1, NN<Text::String> obj2) const
{
	if (obj1 == obj2)
		return 0;
	return obj1->CompareToICase(obj2);
}

IntOS Data::ArrayListICaseStringNN::SortedIndexOfC(Text::CStringNN val) const
{
	IntOS i;
	IntOS j;
	IntOS k;
	IntOS l;
	i = 0;
	j = (IntOS)this->objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = this->arr[k]->CompareToICase(val.v);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return k;
		}
	}
	return -i - 1;
}
