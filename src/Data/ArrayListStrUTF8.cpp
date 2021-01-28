#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListStrUTF8.h"

Data::ArrayListStrUTF8::ArrayListStrUTF8() : Data::SortableArrayList<const UTF8Char*>()
{
}

Data::ArrayListStrUTF8::ArrayListStrUTF8(OSInt Capacity) : Data::SortableArrayList<const UTF8Char*>(Capacity)
{
}

Data::ArrayList<const UTF8Char*> *Data::ArrayListStrUTF8::Clone()
{
	Data::ArrayListStrUTF8 *newArr;
	NEW_CLASS(newArr, Data::ArrayListStrUTF8(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListStrUTF8::SortedInsert(const UTF8Char *val)
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
		l = Text::StrCompare(this->arr[k], val);
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
			MemCopyNO(&newArr[0], &arr[0], (i) * sizeof(const UTF8Char*));
		}
		newArr[i] = val;
		if ((UOSInt)i < this->objCnt)
		{
			MemCopyNO(&newArr[i + 1], &arr[i], (this->objCnt - i) * sizeof(const UTF8Char*));
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

OSInt Data::ArrayListStrUTF8::SortedIndexOf(const UTF8Char *val)
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
		l = Text::StrCompare(arr[k], val);
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

const UTF8Char *Data::ArrayListStrUTF8::JoinNewStr()
{
	OSInt j;
	OSInt k;
	k = j = this->objCnt;
	OSInt strSize = 0;
	while (j-- > 0)
	{
		strSize += Text::StrCharCnt(arr[j]);
	}
	UTF8Char *sbuff = MemAlloc(UTF8Char, strSize + 1);
	UTF8Char *sptr = sbuff;
	j = 0;
	while (j < k)
	{
		sptr = Text::StrConcat(sptr, arr[j]);
		j++;
	}
	return sbuff;
}
