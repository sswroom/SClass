#ifndef _SM_TEXT_STRINGCOMPARATORFAST
#define _SM_TEXT_STRINGCOMPARATORFAST
#include "Data/Comparator.h"
#include "Text/String.h"

namespace Text
{
	class StringComparatorFast : public Data::Comparator<Text::String*>
	{
	public:
		virtual OSInt Compare(Text::String *a, Text::String *b)
		{
			return a->CompareToFast(b->ToCString());
		}
	};
}
#endif
