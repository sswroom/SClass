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

Data::ArrayList<Text::String*> *Data::ArrayListString::Clone() const
{
	Data::ArrayListString *newArr;
	NEW_CLASS(newArr, Data::ArrayListString(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListString::Compare(Text::String* obj1, Text::String* obj2) const
{
	return obj1->CompareTo(obj2);
}

OSInt Data::ArrayListString::SortedIndexOfPtr(const UTF8Char *val, UOSInt len) const
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

Text::String *Data::ArrayListString::JoinString() const
{
	Text::String *newStr;
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
	UTF8Char *sptr;
	newStr = Text::String::New(newStrLeng);
	sptr = (UTF8Char*)newStr->v;
	i = 0;
	while (i < j)
	{
		MemCopyNO(sptr, this->arr[i]->v, sizeof(UTF8Char) * this->arr[i]->leng);
		sptr += this->arr[i]->leng;
		i++;
	}
	*sptr = 0;
	return newStr;
}
