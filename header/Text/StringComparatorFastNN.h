#ifndef _SM_TEXT_STRINGCOMPARATORFASTNN
#define _SM_TEXT_STRINGCOMPARATORFASTNN
#include "Data/Comparator.h"
#include "Text/String.h"

namespace Text
{
	class StringComparatorFastNN : public Data::Comparator<NotNullPtr<Text::String>>
	{
	public:
		virtual OSInt Compare(NotNullPtr<Text::String> a, NotNullPtr<Text::String> b) const
		{
			return a->CompareToFast(b->ToCString());
		}
	};
}
#endif
