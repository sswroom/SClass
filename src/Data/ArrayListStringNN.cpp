#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStringNN.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "Text/MyString.h"

Data::ArrayListStringNN::ArrayListStringNN() : Data::SortableArrayListNN<Text::String>()
{
}

Data::ArrayListStringNN::ArrayListStringNN(UIntOS capacity) : Data::SortableArrayListNN<Text::String>(capacity)
{
}

NN<Data::ArrayListNN<Text::String>> Data::ArrayListStringNN::Clone() const
{
	NN<Data::ArrayListStringNN> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStringNN(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListStringNN::Compare(NN<Text::String> obj1, NN<Text::String> obj2) const
{
	return obj1->CompareTo(obj2->ToCString());
}

IntOS Data::ArrayListStringNN::SortedIndexOfC(Text::CStringNN val) const
{
	IntOS i;
	IntOS j;
	IntOS k;
	IntOS l;
	i = 0;
	j = (IntOS)this->objCnt - 1;
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

UIntOS Data::ArrayListStringNN::IndexOfC(Text::CStringNN val) const
{
	UIntOS i = 0;
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
	UIntOS newStrLeng = 0;
	UIntOS j;
	UIntOS i;
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
	UIntOS newStrLeng = 0;
	UIntOS j;
	UIntOS i;
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
	UIntOS i = this->objCnt;
	while (i-- > 0)
	{
		this->arr[i]->Release();
	}
	this->objCnt = 0;
}

void Data::ArrayListStringNN::ValueCounts(NN<Data::FastStringMapNative<UInt32>> counts) const
{
	UInt32 cnt;
	NN<Text::String> s;
	UIntOS i = this->GetCount();
	if (i > 10)
	{
		UnsafeArray<NN<Text::String>> sarr = MemAllocArr(NN<Text::String>, i);
		MemCopyNO(sarr.Ptr(), this->arr.Ptr(), i * sizeof(NN<Text::String>));
		Data::Sort::ArtificialQuickSort::Sort<NN<Text::String>>(sarr, *this, 0, (IntOS)i - 1);
		UIntOS j = 0;
		UIntOS k = 0;
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

void Data::ArrayListStringNN::RemoveDuplicates()
{
	if (this->objCnt > 0)
	{
		NN<Text::String> lastS;
		NN<Text::String> s;
		UIntOS newCnt = 1;
		UIntOS objCnt = this->objCnt;
		UIntOS i;
		UnsafeArray<NN<Text::String>> newArr = MemAllocArr(NN<Text::String>, this->capacity);
		i = 1;
		lastS = newArr[0] = this->arr[0];
		while (i < objCnt)
		{
			s = this->arr[i];
			if (!lastS->Equals(s))
			{
				lastS = newArr[newCnt++] = s;
			}
			else
			{
				s->Release();
			}
			i++;
		}
		MemFreeArr(this->arr);
		this->arr = newArr;
		this->objCnt = newCnt;
	}
}

void Data::ArrayListStringNN::RemoveAndReleaseLast()
{
	NN<Text::String> s;
	if (this->Pop().SetTo(s))
		s->Release();
}
