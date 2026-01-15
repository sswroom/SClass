#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListICaseString.h"
#include "Text/MyString.h"

Data::ArrayListICaseString::ArrayListICaseString() : Data::ArrayListString()
{
}

Data::ArrayListICaseString::ArrayListICaseString(UIntOS capacity) : Data::ArrayListString(capacity)
{
}

NN<Data::ArrayListObj<Optional<Text::String>>> Data::ArrayListICaseString::Clone() const
{
	NN<Data::ArrayListObj<Optional<Text::String>>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListICaseString(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListICaseString::Compare(Optional<Text::String> obj1, Optional<Text::String> obj2) const
{
	if (obj1 == obj2)
		return 0;
	NN<Text::String> s1;
	NN<Text::String> s2;
	if (!obj1.SetTo(s1))
		return -1;
	if (!obj2.SetTo(s2))
		return 1;
	return s1->CompareToICase(s2);
}

IntOS Data::ArrayListICaseString::SortedIndexOfPtr(UnsafeArray<const UTF8Char> val, UIntOS len) const
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
		l = Text::String::OrEmpty(this->arr[k])->CompareToICase(val);
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
