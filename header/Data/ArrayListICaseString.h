#ifndef _SM_DATA_ARRAYLISTICASESTRING
#define _SM_DATA_ARRAYLISTICASESTRING
#include "Data/ArrayListString.h"

namespace Data
{
	class ArrayListICaseString : public Data::ArrayListString
	{
	public:
		ArrayListICaseString();
		ArrayListICaseString(UOSInt capacity);

		virtual NN<Data::ArrayListObj<Optional<Text::String>>> Clone() const;
		virtual OSInt Compare(Optional<Text::String> obj1, Optional<Text::String> obj2) const;
		virtual OSInt SortedIndexOfPtr(UnsafeArray<const UTF8Char> val, UOSInt len) const;
	};
}
#endif
