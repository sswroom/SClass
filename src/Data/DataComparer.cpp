
#include "Stdafx.h"
#include "Data/DataComparer.h"
#include "Text/MyString.h"

OSInt Data::DataComparer::Compare(const UTF8Char *val1, const UTF8Char *val2)
{
	return Text::StrCompare(val1, val2);
}


