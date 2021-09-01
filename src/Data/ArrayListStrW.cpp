#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Data/ArrayListStrW.h"

Data::ArrayListStrW::ArrayListStrW() : Data::SortableArrayList<const WChar*>()
{
}

Data::ArrayListStrW::ArrayListStrW(OSInt Capacity) : Data::SortableArrayList<const WChar*>(Capacity)
{
}

Data::ArrayList<const WChar*> *Data::ArrayListStrW::Clone()
{
	Data::ArrayListStrW *newArr;
	NEW_CLASS(newArr, Data::ArrayListStrW(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListStrW::CompareItem(const WChar* obj1, const WChar* obj2)
{
	return Text::StrCompare(obj1, obj2);
}

const WChar *Data::ArrayListStrW::JoinNewStr()
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
	WChar *sptr = sbuff;
	j = 0;
	while (j < k)
	{
		sptr = Text::StrConcat(sptr, arr[j]);
		j++;
	}
	return sbuff;
}
