#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListString.h"
#include "Text/MyString.h"

Data::ArrayListString::ArrayListString() : Data::SortableArrayListObj<Optional<Text::String>>()
{
}

Data::ArrayListString::ArrayListString(UIntOS capacity) : Data::SortableArrayListObj<Optional<Text::String>>(capacity)
{
}

NN<Data::ArrayListObj<Optional<Text::String>>> Data::ArrayListString::Clone() const
{
	NN<Data::ArrayListString> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListString(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListString::Compare(Optional<Text::String> obj1, Optional<Text::String> obj2) const
{
	if (obj1 == obj2)
		return 0;
	NN<Text::String> s1;
	NN<Text::String> s2;
	if (!obj1.SetTo(s1))
		return -1;
	if (!obj2.SetTo(s2))
		return 1;
	return s1->CompareTo(s2);
}

IntOS Data::ArrayListString::SortedIndexOfPtr(UnsafeArray<const UTF8Char> val, UIntOS len) const
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
		l = Text::String::OrEmpty(this->arr[k])->CompareTo(val);
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
	UIntOS newStrLeng = 0;
	UIntOS j;
	UIntOS i;
	NN<Text::String> s;
	j = this->objCnt;
	i = 0;
	while (i < j)
	{
		if (this->arr[i].SetTo(s))
			newStrLeng += s->leng;
		i++;
	}
	UnsafeArray<UTF8Char> sptr;
	newStr = Text::String::New(newStrLeng);
	sptr = newStr->v;
	i = 0;
	while (i < j)
	{
		if (this->arr[i].SetTo(s))
		{
			MemCopyNO(sptr.Ptr(), s->v.Ptr(), sizeof(UTF8Char) * s->leng);
			sptr += s->leng;
		}
		i++;
	}
	*sptr = 0;
	return newStr;
}

void Data::ArrayListString::FreeAll()
{
	NN<Text::String> s;
	UIntOS i = 0;
	UIntOS j = this->objCnt;
	while (i < j)
	{
		if (this->arr[i].SetTo(s))
			s->Release();
		i++;
	}
	this->objCnt = 0;
}
