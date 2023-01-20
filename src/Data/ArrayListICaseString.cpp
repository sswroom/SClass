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

Data::ArrayList<Text::String*> *Data::ArrayListICaseString::Clone() const
{
	Data::ArrayListICaseString *newArr;
	NEW_CLASS(newArr, Data::ArrayListICaseString(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListICaseString::Compare(Text::String* obj1, Text::String* obj2) const
{
	return obj1->CompareToICase(obj2);
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
