#ifndef _SM_TEXT_STRINGCOMPARATORFASTNN
#define _SM_TEXT_STRINGCOMPARATORFASTNN
#include "Data/Comparator.hpp"
#include "Text/String.h"

namespace Text
{
	class StringComparatorFastNN : public Data::Comparator<NN<Text::String>>
	{
	public:
		virtual IntOS Compare(NN<Text::String> a, NN<Text::String> b) const
		{
			return a->CompareToFast(b->ToCString());
		}
	};
}
#endif
