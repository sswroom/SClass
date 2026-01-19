#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Data/ArrayListStrW.h"

Data::ArrayListStrW::ArrayListStrW() : Data::SortableArrayListObj<const WChar*>()
{
}

Data::ArrayListStrW::ArrayListStrW(UIntOS Capacity) : Data::SortableArrayListObj<const WChar*>(Capacity)
{
}

NN<Data::ArrayListObj<const WChar*>> Data::ArrayListStrW::Clone() const
{
	NN<Data::ArrayListStrW> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStrW(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListStrW::Compare(const WChar* obj1, const WChar* obj2) const
{
	return Text::StrCompare(obj1, obj2);
}

UnsafeArray<const WChar> Data::ArrayListStrW::JoinNewStr() const
{
	IntOS j;
	IntOS k;
	k = j = this->objCnt;
	IntOS strSize = 0;
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
