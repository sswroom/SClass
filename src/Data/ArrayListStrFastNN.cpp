#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrFastNN.h"
#include "Text/MyString.h"

Data::ArrayListStrFastNN::ArrayListStrFastNN() : Data::SortableArrayListNN<Text::String>()
{
}

Data::ArrayListStrFastNN::ArrayListStrFastNN(UIntOS capacity) : Data::SortableArrayListNN<Text::String>(capacity)
{
}

NN<Data::ArrayListNN<Text::String>> Data::ArrayListStrFastNN::Clone() const
{
	NN<Data::ArrayListStrFastNN> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStrFastNN(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListStrFastNN::Compare(NN<Text::String> obj1, NN<Text::String> obj2) const
{
	return obj1->CompareToFast(obj2->ToCString());
}

IntOS Data::ArrayListStrFastNN::SortedIndexOf(Text::CStringNN val) const
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
		l = this->arr[k]->CompareToFast(val);
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

void Data::ArrayListStrFastNN::FreeAll()
{
	UIntOS i = this->objCnt;
	while (i-- > 0)
	{
		this->arr[i]->Release();
	}
	this->objCnt = 0;
}
