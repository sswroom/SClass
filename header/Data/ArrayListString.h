#ifndef _SM_DATA_ARRAYLISTSTRING
#define _SM_DATA_ARRAYLISTSTRING
#include "Data/SortableArrayListObj.hpp"
#include "Text/String.h"

namespace Data
{
	class ArrayListString : public Data::SortableArrayListObj<Optional<Text::String>>
	{
	public:
		ArrayListString();
		ArrayListString(UIntOS capacity);

		virtual NN<Data::ArrayListObj<Optional<Text::String>>> Clone() const;
		virtual IntOS Compare(Optional<Text::String> obj1, Optional<Text::String> obj2) const;
		virtual IntOS SortedIndexOfPtr(UnsafeArray<const UTF8Char> val, UIntOS len) const;

		NN<Text::String> JoinString() const;
		void FreeAll();
	};
}
#endif
