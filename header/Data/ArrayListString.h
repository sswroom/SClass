#ifndef _SM_DATA_ARRAYLISTSTRING
#define _SM_DATA_ARRAYLISTSTRING
#include "Data/SortableArrayList.hpp"
#include "Text/String.h"

namespace Data
{
	class ArrayListString : public Data::SortableArrayList<Optional<Text::String>>
	{
	public:
		ArrayListString();
		ArrayListString(UOSInt capacity);

		virtual NN<Data::ArrayList<Optional<Text::String>>> Clone() const;
		virtual OSInt Compare(Optional<Text::String> obj1, Optional<Text::String> obj2) const;
		virtual OSInt SortedIndexOfPtr(UnsafeArray<const UTF8Char> val, UOSInt len) const;

		NN<Text::String> JoinString() const;
		void FreeAll();
	};
}
#endif
