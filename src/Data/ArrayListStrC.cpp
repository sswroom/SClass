#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListStrC.h"

Data::ArrayListStrC::ArrayListStrC() : Data::SortableArrayList<const Char*>()
{
}

Data::ArrayListStrC::ArrayListStrC(UOSInt capacity) : Data::SortableArrayList<const Char*>(capacity)
{
}

Data::ArrayList<const Char*> *Data::ArrayListStrC::Clone()
{
	Data::ArrayListStrC *newArr;
	NEW_CLASS(newArr, Data::ArrayListStrC(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

UOSInt Data::ArrayListStrC::SortedInsert(const Char *val)
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
		const Char **newArr = MemAlloc(const Char*, this->capacity << 1);
		if (i > 0)
		{
			MemCopyNO(&newArr[0], &arr[0], (UOSInt)i * sizeof(const Char*));
		}
		newArr[i] = val;
		if ((UOSInt)i < this->objCnt)
		{
			MemCopyNO(&newArr[i + 1], &arr[i], (this->objCnt - (UOSInt)i) * sizeof(const Char*));
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

OSInt Data::ArrayListStrC::SortedIndexOf(const Char *val)
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
