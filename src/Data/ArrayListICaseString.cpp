#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListICaseString.h"
#include "Text/MyString.h"

Data::ArrayListICaseString::ArrayListICaseString() : Data::ArrayListString()
{
}

Data::ArrayListICaseString::ArrayListICaseString(UOSInt capacity) : Data::ArrayListString(capacity)
{
}

NN<Data::ArrayList<Text::String*>> Data::ArrayListICaseString::Clone() const
{
	NN<Data::ArrayList<Text::String*>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListICaseString(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListICaseString::Compare(Text::String* obj1, Text::String* obj2) const
{
	if (obj1 == obj2)
		return 0;
	NN<Text::String> s1;
	NN<Text::String> s2;
	if (!s1.Set(obj1))
		return -1;
	if (!s2.Set(obj2))
		return 1;
	return s1->CompareToICase(s2);
}

OSInt Data::ArrayListICaseString::SortedIndexOfPtr(const UTF8Char *val, UOSInt len) const
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	i = 0;
	j = (OSInt)this->objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = this->arr[k]->CompareToICase(val);
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
