#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListStrUTF8.h"

Data::ArrayListStrUTF8::ArrayListStrUTF8() : Data::SortableArrayListObj<UnsafeArrayOpt<const UTF8Char>>()
{
}

Data::ArrayListStrUTF8::ArrayListStrUTF8(UOSInt capacity) : Data::SortableArrayListObj<UnsafeArrayOpt<const UTF8Char>>(capacity)
{
}

NN<Data::ArrayListObj<UnsafeArrayOpt<const UTF8Char>>> Data::ArrayListStrUTF8::Clone() const
{
	NN<Data::ArrayListStrUTF8> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStrUTF8(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListStrUTF8::Compare(UnsafeArrayOpt<const UTF8Char> obj1, UnsafeArrayOpt<const UTF8Char> obj2) const
{
	return Text::StrCompare(obj1.Ptr(), obj2.Ptr());
}

UnsafeArray<const UTF8Char> Data::ArrayListStrUTF8::JoinNewStr() const
{
	UOSInt j;
	UOSInt k;
	k = j = this->objCnt;
	UOSInt strSize = 0;
	UnsafeArray<const UTF8Char> nns;
	while (j-- > 0)
	{
		if (arr[j].SetTo(nns))
			strSize += Text::StrCharCnt(nns);
	}
	UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, strSize + 1);
	UnsafeArray<UTF8Char> sptr = sbuff;
	j = 0;
	while (j < k)
	{
		if (arr[j].SetTo(nns))
			sptr = Text::StrConcat(sptr, nns);
		j++;
	}
	return sbuff;
}

void Data::ArrayListStrUTF8::DeleteAll()
{
	UOSInt i = 0;
	UOSInt j = this->objCnt;
	while (i < j)
	{
		UnsafeArray<const UTF8Char> nns;
		if (this->arr[i].SetTo(nns))
			Text::StrDelNew(nns);
		i++;
	}
	this->Clear();
}
