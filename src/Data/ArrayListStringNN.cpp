#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStringNN.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Text/MyString.h"

Data::ArrayListStringNN::ArrayListStringNN() : Data::SortableArrayListNN<Text::String>()
{
}

Data::ArrayListStringNN::ArrayListStringNN(UOSInt capacity) : Data::SortableArrayListNN<Text::String>(capacity)
{
}

NN<Data::ArrayListNN<Text::String>> Data::ArrayListStringNN::Clone() const
{
	NN<Data::ArrayListStringNN> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStringNN(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListStringNN::Compare(NN<Text::String> obj1, NN<Text::String> obj2) const
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

NN<Text::String> Data::ArrayListStringNN::JoinString() const
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

NN<Text::String> Data::ArrayListStringNN::JoinString(Text::CStringNN s) const
{
	NN<Text::String> newStr;
	UOSInt newStrLeng = 0;
	UOSInt j;
	UOSInt i;
	j = this->objCnt;
	if (j == 0)
	{
		return Text::String::NewEmpty();
	}
	else if (j == 1)
	{
		return this->arr[0]->Clone();
	}
	newStrLeng = (j - 1) * s.leng;
	i = 0;
	while (i < j)
	{
		newStrLeng += this->arr[i]->leng;
		i++;
	}
	UnsafeArray<UTF8Char> sptr;
	newStr = Text::String::New(newStrLeng);
	sptr = newStr->v;
	MemCopyNO(sptr.Ptr(), this->arr[0]->v.Ptr(), sizeof(UTF8Char) * this->arr[0]->leng);
	sptr += this->arr[0]->leng;
	i = 1;
	while (i < j)
	{
		MemCopyNO(sptr.Ptr(), s.v.Ptr(), s.leng);
		sptr += s.leng;
		MemCopyNO(sptr.Ptr(), this->arr[i]->v.Ptr(), sizeof(UTF8Char) * this->arr[i]->leng);
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

void Data::ArrayListStringNN::ValueCounts(NN<Data::FastStringMap<UInt32>> counts) const
{
	UInt32 cnt;
	NN<Text::String> s;
	UOSInt i = this->GetCount();
	if (i > 10)
	{
		UnsafeArray<NN<Text::String>> sarr = MemAllocArr(NN<Text::String>, i);
		MemCopyNO(sarr.Ptr(), this->arr.Ptr(), i * sizeof(NN<Text::String>));
		Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(sarr, *this, 0, (OSInt)i - 1);
		UOSInt j = 0;
		UOSInt k = 0;
		while (j < i)
		{
			if (!sarr[j]->Equals(sarr[k]))
			{
				counts->PutNN(sarr[k], (UInt32)(j - k));
				k = j;
			}
			j++;
		}
		counts->PutNN(sarr[k], (UInt32)(i - k));
		MemFreeArr(sarr);
	}
	else
	{
		while (i-- > 0)
		{
			s = this->GetItemNoCheck(i);
			cnt = counts->GetNN(s);
			counts->PutNN(s, cnt + 1);
		}
	}
}
