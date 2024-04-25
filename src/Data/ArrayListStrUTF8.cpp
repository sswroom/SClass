#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListStrUTF8.h"

Data::ArrayListStrUTF8::ArrayListStrUTF8() : Data::SortableArrayList<const UTF8Char*>()
{
}

Data::ArrayListStrUTF8::ArrayListStrUTF8(UOSInt capacity) : Data::SortableArrayList<const UTF8Char*>(capacity)
{
}

NN<Data::ArrayList<const UTF8Char*>> Data::ArrayListStrUTF8::Clone() const
{
	NN<Data::ArrayListStrUTF8> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListStrUTF8(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListStrUTF8::Compare(const UTF8Char* obj1, const UTF8Char* obj2) const
{
	return Text::StrCompare(obj1, obj2);
}

const UTF8Char *Data::ArrayListStrUTF8::JoinNewStr() const
{
	UOSInt j;
	UOSInt k;
	k = j = this->objCnt;
	UOSInt strSize = 0;
	while (j-- > 0)
	{
		strSize += Text::StrCharCnt(arr[j]);
	}
	UTF8Char *sbuff = MemAlloc(UTF8Char, strSize + 1);
	UTF8Char *sptr = sbuff;
	j = 0;
	while (j < k)
	{
		sptr = Text::StrConcat(sptr, arr[j]);
		j++;
	}
	return sbuff;
}
