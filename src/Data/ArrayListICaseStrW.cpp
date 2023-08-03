#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Data/ArrayListICaseStrW.h"

Data::ArrayListICaseStrW::ArrayListICaseStrW() : Data::ArrayListStrW()
{
}

Data::ArrayListICaseStrW::ArrayListICaseStrW(UOSInt Capacity) : Data::ArrayListStrW(Capacity)
{
}

NotNullPtr<Data::ArrayList<const WChar*>> Data::ArrayListICaseStrW::Clone() const
{
	NotNullPtr<Data::ArrayListICaseStrW> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListICaseStrW(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

OSInt Data::ArrayListICaseStrW::Compare(const WChar* obj1, const WChar* obj2) const
{
	return Text::StrCompareICase(obj1, obj2);
}
