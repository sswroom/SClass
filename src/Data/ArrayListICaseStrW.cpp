#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Data/ArrayListICaseStrW.h"

Data::ArrayListICaseStrW::ArrayListICaseStrW() : Data::ArrayListStrW()
{
}

Data::ArrayListICaseStrW::ArrayListICaseStrW(OSInt Capacity) : Data::ArrayListStrW(Capacity)
{
}

Data::ArrayList<const WChar*> *Data::ArrayListICaseStrW::Clone()
{
	Data::ArrayListICaseStrW *newArr;
	NEW_CLASS(newArr, Data::ArrayListICaseStrW(this->capacity));
	newArr->AddAll(this);
	return newArr;
}

OSInt Data::ArrayListICaseStrW::CompareItem(const WChar* obj1, const WChar* obj2)
{
	return Text::StrCompareICase(obj1, obj2);
}
