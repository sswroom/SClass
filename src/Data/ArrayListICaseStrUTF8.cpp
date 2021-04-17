#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListICaseStrUTF8.h"

Data::ArrayListICaseStrUTF8::ArrayListICaseStrUTF8() : Data::ArrayListStrUTF8()
{
}

Data::ArrayListICaseStrUTF8::ArrayListICaseStrUTF8(UOSInt capacity) : Data::ArrayListStrUTF8(capacity)
{
}

Data::ArrayList<const UTF8Char*> *Data::ArrayListICaseStrUTF8::Clone()
{
	Data::ArrayListICaseStrUTF8 *newArr;
	NEW_CLASS(newArr, Data::ArrayListICaseStrUTF8(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListICaseStrUTF8::SortedInsert(const UTF8Char *val)
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
		const UTF8Char **newArr = MemAlloc(const UTF8Char*, this->capacity << 1);
		if (i > 0)
		{
			MemCopyNO(&newArr[0], &arr[0], (UOSInt)(i) * sizeof(const UTF8Char*));
		}
		newArr[i] = val;
		if ((UOSInt)i < this->objCnt)
		{
			MemCopyNO(&newArr[i + 1], &arr[i], (this->objCnt - (UOSInt)i) * sizeof(const UTF8Char*));
		}
		this->capacity = this->capacity << 1;
		MemFree(arr);
		arr = newArr;
	}
	else
	{
		j = (OSInt)this->objCnt;
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

OSInt Data::ArrayListICaseStrUTF8::SortedIndexOf(const UTF8Char *val)
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
