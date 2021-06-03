#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListString.h"
#include <memory.h>

Data::ArrayListString::ArrayListString() : Data::SortableArrayList<Text::String*>()
{
}

Data::ArrayListString::ArrayListString(UOSInt capacity) : Data::SortableArrayList<Text::String*>(capacity)
{
}

Data::ArrayList<Text::String*> *Data::ArrayListString::Clone()
{
	Data::ArrayListString *newArr;
	NEW_CLASS(newArr, Data::ArrayListString(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListString::SortedInsert(Text::String *val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	i = 0;
	j = (OSInt)objCnt - 1;
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
			i = k + 1;
			break;
		}
	}

	if (objCnt == this->capacity)
	{
		Text::String **newArr = MemAlloc(Text::String*, this->capacity << 1);
		k = (OSInt)this->objCnt;
		while (k-- > 0)
		{
			newArr[k] = arr[k];
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	j = (OSInt)objCnt;
	while (j > i)
	{
		arr[j] = arr[j - 1];
		j--;
	}
	objCnt++;
	arr[i] = val;
	return (UOSInt)i;
}

OSInt Data::ArrayListString::SortedIndexOf(Text::String *val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	i = 0;
	j = (OSInt)objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = arr[k]->CompareTo(val);
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

Text::String *Data::ArrayListString::JoinString()
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
	NEW_CLASS(newStr, Text::String(newStrLeng));
	sptr = (UTF8Char*)newStr->strVal;
	i = 0;
	while (i < j)
	{
		MemCopyNO(sptr, this->arr[i]->strVal, sizeof(UTF8Char) * this->arr[i]->leng);
		sptr += this->arr[i]->leng;
		i++;
	}
	*sptr = 0;
	return newStr;
}
