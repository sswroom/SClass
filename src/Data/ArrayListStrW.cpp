#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Data/ArrayListStrW.h"

Data::ArrayListStrW::ArrayListStrW() : Data::SortableArrayList<const WChar*>()
{
}

Data::ArrayListStrW::ArrayListStrW(UOSInt Capacity) : Data::SortableArrayList<const WChar*>(Capacity)
{
}

NN<Data::ArrayList<const WChar*>> Data::ArrayListStrW::Clone() const
{
	NN<Data::ArrayListStrW> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStrW(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListStrW::Compare(const WChar* obj1, const WChar* obj2) const
{
	return Text::StrCompare(obj1, obj2);
}

UnsafeArray<const WChar> Data::ArrayListStrW::JoinNewStr() const
{
	OSInt j;
	OSInt k;
	k = j = this->objCnt;
	OSInt strSize = 0;
	while (j-- > 0)
	{
		strSize += Text::StrCharCnt(arr[j]);
	}
	UnsafeArray<WChar> sbuff = MemAllocArr(WChar, strSize + 1);
	UnsafeArray<WChar> wptr = sbuff;
	j = 0;
	while (j < k)
	{
		wptr = Text::StrConcat(wptr, arr[j]);
		j++;
	}
	return sbuff;
}
