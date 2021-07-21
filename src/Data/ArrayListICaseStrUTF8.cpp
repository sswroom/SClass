#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListICaseStrUTF8.h"

Data::ArrayListICaseStrUTF8::ArrayListICaseStrUTF8() : Data::ArrayListStrUTF8()
{
}

Data::ArrayListICaseStrUTF8::ArrayListICaseStrUTF8(UOSInt capacity) : Data::ArrayListStrUTF8(capacity)
{
}

Data::ArrayList<const UTF8Char*> *Data::ArrayListICaseStrUTF8::Clone()
{
	Data::ArrayListICaseStrUTF8 *newArr;
	NEW_CLASS(newArr, Data::ArrayListICaseStrUTF8(this->capacity));
	newArr->AddRange(this);
	return newArr;
}

OSInt Data::ArrayListICaseStrUTF8::CompareItem(const UTF8Char* obj1, const UTF8Char* obj2)
{
	return Text::StrCompareICase(obj1, obj2);
}
