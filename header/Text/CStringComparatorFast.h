#ifndef _SM_TEXT_CSTRINGCOMPARATORFAST
#define _SM_TEXT_CSTRINGCOMPARATORFAST
#include "Data/Comparator.hpp"
#include "Text/CString.h"

namespace Text
{
	class CStringComparatorFast : public Data::Comparator<Text::CString>
	{
	public:
		virtual OSInt Compare(Text::CString a, Text::CString b)
		{
			return a.CompareToFast(b);
		}
	};
}
#endif
