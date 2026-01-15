#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrFast.h"
#include "Text/MyString.h"

Data::ArrayListStrFast::ArrayListStrFast() : Data::SortableArrayListObj<Text::String*>()
{
}

Data::ArrayListStrFast::ArrayListStrFast(UIntOS capacity) : Data::SortableArrayListObj<Text::String*>(capacity)
{
}

NN<Data::ArrayListObj<Text::String*>> Data::ArrayListStrFast::Clone() const
{
	NN<Data::ArrayListObj<Text::String*>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStrFast(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListStrFast::Compare(Text::String *obj1, Text::String *obj2) const
{
	return obj1->CompareToFast(obj2->ToCString());
}

IntOS Data::ArrayListStrFast::SortedIndexOf(Text::CStringNN val) const
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
