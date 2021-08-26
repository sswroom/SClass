#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListString.h"
#include <memory.h>

Data::ArrayListString::ArrayListString() : Data::SortableArrayList<Text::String*>()
{
}

Data::ArrayListString::ArrayListString(UOSInt capacity) : Data::SortableArrayList<Text::String*>(capacity)
{
}

Data::ArrayList<Text::String*> *Data::ArrayListString::Clone()
{
	Data::ArrayListString *newArr;
	NEW_CLASS(newArr, Data::ArrayListString(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListString::CompareItem(Text::String* obj1, Text::String* obj2)
{
	return obj1->CompareTo(obj2);
}

Text::String *Data::ArrayListString::JoinString()
{
	Text::String *newStr;
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
	NEW_CLASS(newStr, Text::String(newStrLeng));
	sptr = (UTF8Char*)newStr->strVal;
	i = 0;
	while (i < j)
	{
		MemCopyNO(sptr, this->arr[i]->strVal, sizeof(UTF8Char) * this->arr[i]->leng);
		sptr += this->arr[i]->leng;
		i++;
	}
	*sptr = 0;
	return newStr;
}
