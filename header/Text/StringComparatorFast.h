#ifndef _SM_TEXT_STRINGCOMPARATORFAST
#define _SM_TEXT_STRINGCOMPARATORFAST
#include "Data/Comparator.h"
#include "Text/String.h"

namespace Text
{
	class StringComparatorFast : public Data::Comparator<NN<Text::String>>
	{
	public:
		virtual OSInt Compare(NN<Text::String> a, NN<Text::String> b) const
		{
			return a->CompareToFast(b->ToCString());
		}
	};
}
#endif
