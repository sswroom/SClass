#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStringNN.h"
#include "Text/MyString.h"

Data::ArrayListStringNN::ArrayListStringNN() : Data::SortableArrayListNN<Text::String>()
{
}

Data::ArrayListStringNN::ArrayListStringNN(UOSInt capacity) : Data::SortableArrayListNN<Text::String>(capacity)
{
}

NotNullPtr<Data::ArrayListNN<Text::String>> Data::ArrayListStringNN::Clone() const
{
	NotNullPtr<Data::ArrayListStringNN> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStringNN(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListStringNN::Compare(NotNullPtr<Text::String> obj1, NotNullPtr<Text::String> obj2) const
{
	return obj1->CompareTo(obj2->ToCString());
}

OSInt Data::ArrayListStringNN::SortedIndexOfC(Text::CStringNN val) const
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

UOSInt Data::ArrayListStringNN::IndexOfC(Text::CStringNN val) const
{
	UOSInt i = 0;
	while (i < this->objCnt)
	{
		if (this->arr[i]->Equals(val))
			return i;
		i++;
	}
	return INVALID_INDEX;
}

NotNullPtr<Text::String> Data::ArrayListStringNN::JoinString() const
{
	NotNullPtr<Text::String> newStr;
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

void Data::ArrayListStringNN::FreeAll()
{
	UOSInt i = this->objCnt;
	while (i-- > 0)
	{
		this->arr[i]->Release();
	}
	this->objCnt = 0;
}
