#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListString.h"
#include "Text/MyString.h"

Data::ArrayListString::ArrayListString() : Data::SortableArrayList<Text::String*>()
{
}

Data::ArrayListString::ArrayListString(UOSInt capacity) : Data::SortableArrayList<Text::String*>(capacity)
{
}

NN<Data::ArrayList<Text::String*>> Data::ArrayListString::Clone() const
{
	NN<Data::ArrayListString> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListString(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListString::Compare(Text::String* obj1, Text::String* obj2) const
{
	if (obj1 == obj2)
		return 0;
	NN<Text::String> s1;
	NN<Text::String> s2;
	if (!s1.Set(obj1))
		return -1;
	if (!s2.Set(obj2))
		return 1;
	return s1->CompareTo(s2);
}

OSInt Data::ArrayListString::SortedIndexOfPtr(UnsafeArray<const UTF8Char> val, UOSInt len) const
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
		l = this->arr[k]->CompareTo(val);
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

NN<Text::String> Data::ArrayListString::JoinString() const
{
	NN<Text::String> newStr;
	UOSInt newStrLeng = 0;
	UOSInt j;
	UOSInt i;
	j = this->objCnt;
	i = 0;
	while (i < j)
	{
		newStrLeng += this->arr[i]->leng;
		i++;
	}
	UnsafeArray<UTF8Char> sptr;
	newStr = Text::String::New(newStrLeng);
	sptr = newStr->v;
	i = 0;
	while (i < j)
	{
		MemCopyNO(sptr.Ptr(), this->arr[i]->v.Ptr(), sizeof(UTF8Char) * this->arr[i]->leng);
		sptr += this->arr[i]->leng;
		i++;
	}
	*sptr = 0;
	return newStr;
}
