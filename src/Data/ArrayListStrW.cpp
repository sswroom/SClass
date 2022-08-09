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

Data::ArrayList<const WChar*> *Data::ArrayListStrW::Clone() const
{
	Data::ArrayListStrW *newArr;
	NEW_CLASS(newArr, Data::ArrayListStrW(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListStrW::CompareItem(const WChar* obj1, const WChar* obj2) const
{
	return Text::StrCompare(obj1, obj2);
}

const WChar *Data::ArrayListStrW::JoinNewStr() const
{
	OSInt j;
	OSInt k;
	k = j = this->objCnt;
	OSInt strSize = 0;
	while (j-- > 0)
	{
		strSize += Text::StrCharCnt(arr[j]);
	}
	WChar *sbuff = MemAlloc(WChar, strSize + 1);
	WChar *wptr = sbuff;
	j = 0;
	while (j < k)
	{
		wptr = Text::StrConcat(wptr, arr[j]);
		j++;
	}
	return sbuff;
}
