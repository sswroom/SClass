#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListICaseStrW.h"

Data::ArrayListICaseStrW::ArrayListICaseStrW() : Data::ArrayListStrW()
{
}

Data::ArrayListICaseStrW::ArrayListICaseStrW(OSInt Capacity) : Data::ArrayListStrW(Capacity)
{
}

Data::ArrayList<const WChar*> *Data::ArrayListICaseStrW::Clone()
{
	Data::ArrayListICaseStrW *newArr;
	NEW_CLASS(newArr, Data::ArrayListICaseStrW(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListICaseStrW::SortedInsert(const WChar *val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	i = 0;
	j = objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase(this->arr[k], val);
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
		const WChar **newArr = MemAlloc(const WChar*, this->capacity << 1);
		if (i > 0)
		{
			MemCopyNO(&newArr[0], &arr[0], (i) * sizeof(const WChar*));
		}
		newArr[i] = val;
		if ((UOSInt)i < this->objCnt)
		{
			MemCopyNO(&newArr[i + 1], &arr[i], (this->objCnt - i) * sizeof(const WChar*));
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	else
	{
		j = this->objCnt;
		while (j > i)
		{
			arr[j] = arr[j - 1];
			j--;
		}
		arr[i] = val;
	}
	this->objCnt++;
	return (UOSInt)i;
}

OSInt Data::ArrayListICaseStrW::SortedIndexOf(const WChar *val)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	i = 0;
	j = objCnt - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase(arr[k], val);
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
