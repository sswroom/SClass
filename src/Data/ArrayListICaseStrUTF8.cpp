#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Data/ArrayListICaseStrUTF8.h"

Data::ArrayListICaseStrUTF8::ArrayListICaseStrUTF8() : Data::ArrayListStrUTF8()
{
}

Data::ArrayListICaseStrUTF8::ArrayListICaseStrUTF8(UIntOS capacity) : Data::ArrayListStrUTF8(capacity)
{
}

NN<Data::ArrayListObj<UnsafeArrayOpt<const UTF8Char>>> Data::ArrayListICaseStrUTF8::Clone() const
{
	NN<Data::ArrayListObj<UnsafeArrayOpt<const UTF8Char>>> newArr;
	NEW_CLASSNN(newArr, Data::ArrayListICaseStrUTF8(this->capacity));
	newArr->AddAll(*this);
	return newArr;
}

IntOS Data::ArrayListICaseStrUTF8::CompareItem(UnsafeArrayOpt<const UTF8Char> obj1, UnsafeArrayOpt<const UTF8Char> obj2) const
{
	return Text::StrCompareICase(obj1.Ptr(), obj2.Ptr());
}
