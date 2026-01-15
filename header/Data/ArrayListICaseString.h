#ifndef _SM_DATA_ARRAYLISTICASESTRING
#define _SM_DATA_ARRAYLISTICASESTRING
#include "Data/ArrayListString.h"

namespace Data
{
	class ArrayListICaseString : public Data::ArrayListString
	{
	public:
		ArrayListICaseString();
		ArrayListICaseString(UIntOS capacity);

		virtual NN<Data::ArrayListObj<Optional<Text::String>>> Clone() const;
		virtual IntOS Compare(Optional<Text::String> obj1, Optional<Text::String> obj2) const;
		virtual IntOS SortedIndexOfPtr(UnsafeArray<const UTF8Char> val, UIntOS len) const;
	};
}
#endif
