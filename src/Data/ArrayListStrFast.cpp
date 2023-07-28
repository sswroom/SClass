#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrFast.h"
#include "Text/MyString.h"

Data::ArrayListStrFast::ArrayListStrFast() : Data::SortableArrayList<Text::String*>()
{
}

Data::ArrayListStrFast::ArrayListStrFast(UOSInt capacity) : Data::SortableArrayList<Text::String*>(capacity)
{
}

NotNullPtr<Data::ArrayList<Text::String*>> Data::ArrayListStrFast::Clone() const
{
	NotNullPtr<Data::ArrayList<Text::String*>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStrFast(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListStrFast::Compare(Text::String *obj1, Text::String *obj2) const
{
	return obj1->CompareToFast(obj2->ToCString());
}

OSInt Data::ArrayListStrFast::SortedIndexOf(Text::CString val) const
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
